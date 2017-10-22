/**
 * \file InsetMathSpace.cpp
 * This file is part of LyX, the document processor.
 * Licence details can be found in the file COPYING.
 *
 * \author André Pönitz
 *
 * Full author contact details are available in file CREDITS.
 */

#include <config.h>

#include "InsetMathSpace.h"
#include "MathData.h"
#include "MathFactory.h"
#include "MathStream.h"
#include "MathSupport.h"

#include "BufferView.h"
#include "Cursor.h"
#include "FuncRequest.h"
#include "FuncStatus.h"
#include "LaTeXFeatures.h"
#include "MetricsInfo.h"

#include "insets/InsetSpace.h"

#include "frontends/Application.h"
#include "frontends/Painter.h"

#include "support/lassert.h"

using namespace std;

namespace lyx {

namespace {

struct SpaceInfo {
	string name;
	int width;
	InsetSpaceParams::Kind kind;
	bool negative;
	bool visible;
	bool custom;
	bool escape; ///< whether a backslash needs to be added for writing
};

SpaceInfo space_info[] = {
	// name          width (mu)      kind                   negative visible custom escape
	{"!",                3, InsetSpaceParams::NEGTHIN,         true,  true,  false, true},
	{"negthinspace",     3, InsetSpaceParams::NEGTHIN,         true,  true,  false, true},
	{"negmedspace",      4, InsetSpaceParams::NEGMEDIUM,       true,  true,  false, true},
	{"negthickspace",    5, InsetSpaceParams::NEGTHICK,        true,  true,  false, true},
	{",",                3, InsetSpaceParams::THIN,            false, true,  false, true},
	{"thinspace",        3, InsetSpaceParams::THIN,            false, true,  false, true},
	{":",                4, InsetSpaceParams::MEDIUM,          false, true,  false, true},
	{"medspace",         4, InsetSpaceParams::MEDIUM,          false, true,  false, true},
	{";",                5, InsetSpaceParams::THICK,           false, true,  false, true},
	{"thickspace",       5, InsetSpaceParams::THICK,           false, true,  false, true},
	{"enskip",           5, InsetSpaceParams::ENSKIP,          false, true,  false, true},
	{"enspace",          5, InsetSpaceParams::ENSPACE,         false, true,  false, true},
	{"quad",            18, InsetSpaceParams::QUAD,            false, true,  false, true},
	{"qquad",           36, InsetSpaceParams::QQUAD,           false, true,  false, true},
	{"lyxnegspace",     -1, InsetSpaceParams::NEGTHIN,         true,  false, false, true},
	{"lyxposspace",      1, InsetSpaceParams::THIN,            false, false, false, true},
	{"hfill",           72, InsetSpaceParams::HFILL,           false, true,  false, true},
	{"hspace*{\\fill}", 72, InsetSpaceParams::HFILL_PROTECTED, false, true,  false, true},
	{"hspace*",          0, InsetSpaceParams::CUSTOM_PROTECTED,false, true,  true,  true},
	{"hspace",           0, InsetSpaceParams::CUSTOM,          false, true,  true,  true},
	{" ",                5, InsetSpaceParams::NORMAL,          false, true,  false, true},
	{"~",                5, InsetSpaceParams::PROTECTED,       false, true,  false, false},
};

int const nSpace = sizeof(space_info)/sizeof(SpaceInfo);
int const defaultSpace = 4;

} // namespace

InsetMathSpace::InsetMathSpace()
	: space_(defaultSpace)
{
}


InsetMathSpace::InsetMathSpace(string const & name, string const & length)
	: space_(defaultSpace)
{
	for (int i = 0; i < nSpace; ++i)
		if (space_info[i].name == name) {
			space_ = i;
			break;
		}
	if (space_info[space_].custom) {
		length_ = Length(length);
		if (length_.zero() || length_.empty()) {
			length_.value(1.0);
			length_.unit(Length::EM);
		}
	}
}


InsetMathSpace::InsetMathSpace(Length const & length, bool const prot)
	: space_(defaultSpace), length_(length)
{
	for (int i = 0; i < nSpace; ++i)
		if ((prot && space_info[i].name == "hspace*")
			|| (!prot && space_info[i].name == "hspace")) {
			space_ = i;
			break;
		}
}


Inset * InsetMathSpace::clone() const
{
	return new InsetMathSpace(*this);
}


void InsetMathSpace::metrics(MetricsInfo & mi, Dimension & dim) const
{
	Changer dummy = mi.base.changeEnsureMath();
	dim.asc = mi.base.mu(4);
	dim.des = 1 + (int) mi.base.solidLineThickness();
	if (space_info[space_].custom)
		dim.wid = abs(length_.inPixels(mi.base));
	else
		dim.wid = mi.base.mu(space_info[space_].width);
}


void InsetMathSpace::draw(PainterInfo & pi, int x, int y) const
{
	if (!space_info[space_].visible)
		return;
	Changer dummy = pi.base.changeEnsureMath();
	double t = pi.base.solidLineThickness();
	Dimension const dim = dimension(*pi.base.bv);

	double const x1 = x + t/2;
	double const y1 = y;
	double const x3 = x + dim.wid - t/2 - 1;
	double const y3 = y - dim.asc;
	double xp[4] = {x1, x1, x3, x3};
	double yp[4] = {y3, y1, y1, y3};

	Color const col = space_info[space_].custom ?
		Color_special : (isNegative() ? Color_latex : Color_math);
	pi.pain.linesDouble(xp, yp, 4, col,
	                    pi.pain.fill_none, pi.pain.line_solid, t);
}


void InsetMathSpace::incSpace()
{
	int const oldwidth = space_info[space_].width;
	do
		space_ = (space_ + 1) % nSpace;
	while ((space_info[space_].width == oldwidth && !space_info[space_].custom) ||
	       !space_info[space_].visible);
	if (space_info[space_].custom && (length_.zero() || length_.empty())) {
		length_.value(1.0);
		length_.unit(Length::EM);
	}
}


void InsetMathSpace::validate(LaTeXFeatures & features) const
{
	if (space_info[space_].name == "negmedspace" ||
	    space_info[space_].name == "negthickspace")
		features.require("amsmath");
}


void InsetMathSpace::maple(MapleStream & os) const
{
	os << ' ';
}

void InsetMathSpace::mathematica(MathematicaStream & os) const
{
	os << ' ';
}


void InsetMathSpace::octave(OctaveStream & os) const
{
	os << ' ';
}


void InsetMathSpace::mathmlize(MathStream & ms) const
{
	SpaceInfo const & si = space_info[space_];
	if (si.negative || !si.visible)
		return;
	string l;
	if (si.custom)
		l = length_.asHTMLString();
	else if (si.kind != InsetSpaceParams::MEDIUM) {
		stringstream ss;
		ss << si.width;
		l = ss.str() + "px";
	}

	ms << "<mspace";
	if (!l.empty())
		ms << " width=\"" << from_ascii(l) << "\"";
	ms << " />";
}


void InsetMathSpace::htmlize(HtmlStream & ms) const
{
	SpaceInfo const & si = space_info[space_];
	switch (si.kind) {
	case InsetSpaceParams::THIN:
		ms << from_ascii("&thinsp;");
		break;
	case InsetSpaceParams::MEDIUM:
		ms << from_ascii("&nbsp;");
		break;
	case InsetSpaceParams::THICK:
		ms << from_ascii("&emsp;");
		break;
	case InsetSpaceParams::ENSKIP:
	case InsetSpaceParams::ENSPACE:
		ms << from_ascii("&ensp;");
		break;
	case InsetSpaceParams::QUAD:
		ms << from_ascii("&emsp;");
		break;
	case InsetSpaceParams::QQUAD:
		ms << from_ascii("&emsp;&emsp;");
		break;
	case InsetSpaceParams::HFILL:
	case InsetSpaceParams::HFILL_PROTECTED:
		// FIXME: is there a useful HTML entity?
		break;
	case InsetSpaceParams::CUSTOM:
	case InsetSpaceParams::CUSTOM_PROTECTED: {
		string l = length_.asHTMLString();
		ms << MTag("span", "width='" + l + "'")
		   << from_ascii("&nbsp;") << ETag("span");
		break;
	}
	case InsetSpaceParams::NORMAL:
	case InsetSpaceParams::PROTECTED:
		ms << from_ascii("&nbsp;");
		break;
	default:
		break;
	}
}


void InsetMathSpace::normalize(NormalStream & os) const
{
	os << "[space " << int(space_) << "] ";
}


void InsetMathSpace::write(WriteStream & os) const
{
	// All kinds work in text and math mode, so simply suspend
	// writing a possibly pending mode closing brace.
	MathEnsurer ensurer(os, false);
	if (space_info[space_].escape)
		os << '\\';
	os << space_info[space_].name.c_str();
	if (space_info[space_].custom)
		os << '{' << length_.asLatexString().c_str() << '}';
	else if (space_info[space_].escape && space_info[space_].name.length() > 1)
		os.pendingSpace(true);
}


InsetSpaceParams InsetMathSpace::params() const
{
	InsetSpaceParams isp(true);
	LASSERT(space_info[space_].visible, return isp);
	isp.kind = space_info[space_].kind;
	isp.length = GlueLength(length_);
	return isp;
}


string InsetMathSpace::contextMenuName() const
{
	return "context-mathspace";
}


bool InsetMathSpace::getStatus(Cursor & cur, FuncRequest const & cmd,
                               FuncStatus & status) const
{
	switch (cmd.action()) {
	// we handle these
	case LFUN_INSET_MODIFY:
	case LFUN_INSET_DIALOG_UPDATE:
	case LFUN_MOUSE_RELEASE:
		status.setEnabled(true);
		return true;
	default:
		bool retval = InsetMath::getStatus(cur, cmd, status);
		return retval;
	}
}


void InsetMathSpace::doDispatch(Cursor & cur, FuncRequest & cmd)
{
	switch (cmd.action()) {
	case LFUN_INSET_MODIFY:
		if (cmd.getArg(0) == "mathspace") {
			MathData ar;
			if (createInsetMath_fromDialogStr(cmd.argument(), ar)) {
				cur.recordUndo();
				*this = *ar[0].nucleus()->asSpaceInset();
				break;
			}
		}
		cur.undispatched();
		break;

	case LFUN_MOUSE_RELEASE:
		if (cmd.button() == mouse_button::button1 && !cur.selection()) {
			showInsetDialog(&cur.bv());
			break;
		}
		cur.undispatched();
		break;

	default:
		InsetMath::doDispatch(cur, cmd);
		break;
	}
}


bool InsetMathSpace::isNegative() const
{
	if (space_info[space_].custom)
		return length_.value() < 0;
	return space_info[space_].negative;
}

} // namespace lyx

/**
 * \file InsetPhantom.cpp
 * This file is part of LyX, the document processor.
 * Licence details can be found in the file COPYING.
 *
 * \author Uwe Stöhr
 *
 * Full author contact details are available in file CREDITS.
 */

#include <config.h>

#include "InsetPhantom.h"

#include "Buffer.h"
#include "BufferParams.h"
#include "BufferView.h"
#include "BufferParams.h"
#include "Cursor.h"
#include "Dimension.h"
#include "DispatchResult.h"
#include "Exporter.h"
#include "FuncRequest.h"
#include "FuncStatus.h"
#include "InsetIterator.h"
#include "LaTeXFeatures.h"
#include "Lexer.h"
#include "MetricsInfo.h"
#include "OutputParams.h"
#include "texstream.h"
#include "TextClass.h"

#include "mathed/MathSupport.h"

#include "support/docstream.h"
#include "support/gettext.h"
#include "support/lstrings.h"
#include "support/Translator.h"

#include "frontends/Application.h"
#include "frontends/FontMetrics.h"
#include "frontends/Painter.h"

#include <algorithm>
#include <cmath>
#include <sstream>

using namespace std;

namespace lyx {

namespace {

typedef Translator<string, InsetPhantomParams::Type> PhantomTranslator;
typedef Translator<docstring, InsetPhantomParams::Type> PhantomTranslatorLoc;

PhantomTranslator const init_phantomtranslator()
{
	PhantomTranslator translator("Phantom", InsetPhantomParams::Phantom);
	translator.addPair("HPhantom", InsetPhantomParams::HPhantom);
	translator.addPair("VPhantom", InsetPhantomParams::VPhantom);
	return translator;
}


PhantomTranslatorLoc const init_phantomtranslator_loc()
{
	PhantomTranslatorLoc translator(_("Phantom"), InsetPhantomParams::Phantom);
	translator.addPair(_("HPhantom"), InsetPhantomParams::HPhantom);
	translator.addPair(_("VPhantom"), InsetPhantomParams::VPhantom);
	return translator;
}


PhantomTranslator const & phantomtranslator()
{
	static PhantomTranslator const translator =
	    init_phantomtranslator();
	return translator;
}


PhantomTranslatorLoc const & phantomtranslator_loc()
{
	static PhantomTranslatorLoc const translator =
	    init_phantomtranslator_loc();
	return translator;
}

} // namespace


InsetPhantomParams::InsetPhantomParams()
	: type(Phantom)
{}


void InsetPhantomParams::write(ostream & os) const
{
	string const label = phantomtranslator().find(type);
	os << "Phantom " << label << "\n";
}


void InsetPhantomParams::read(Lexer & lex)
{
	string label;
	lex >> label;
	if (lex)
		type = phantomtranslator().find(label);
}


/////////////////////////////////////////////////////////////////////
//
// InsetPhantom
//
/////////////////////////////////////////////////////////////////////

InsetPhantom::InsetPhantom(Buffer * buf, string const & label)
	: InsetCollapsible(buf)
{
	setDrawFrame(false);
	params_.type = phantomtranslator().find(label);
}


InsetPhantom::~InsetPhantom()
{
	hideDialogs("phantom", this);
}


docstring InsetPhantom::layoutName() const
{
	return from_ascii("Phantom:" + phantomtranslator().find(params_.type));
}


void InsetPhantom::draw(PainterInfo & pi, int x, int y) const
{
	// draw the text
	InsetCollapsible::draw(pi, x, y);

	// draw the inset marker
	drawMarkers(pi, x, y);

	// draw the arrow(s)
	int const arrow_size = pi.base.em(1./9) * 2; //even number for grid alignment
	double const dt = mathed_deco_thickness(pi.base);

	ColorCode const origcol = pi.base.font.color();
	pi.base.font.setColor(Color_special);
	pi.base.font.setColor(origcol);
	Dimension const dim = dimension(*pi.base.bv);

	double const xv = round(x + (dim.wid - arrow_size) / 2. - 0.5);
	double const yv = y - dim.asc;
	double const w = arrow_size - dt;
	double const yh = round(y - (dim.asc - dim.des + arrow_size) / 2. - 0.5);
	if (params_.type == InsetPhantomParams::Phantom ||
		params_.type == InsetPhantomParams::VPhantom)
		mathed_draw_deco(pi, xv, yv, w, dim.height() - dt,
		                 from_ascii("updownarrow"));

	if (params_.type == InsetPhantomParams::Phantom ||
		params_.type == InsetPhantomParams::HPhantom)
		mathed_draw_deco(pi, x, yh, dim.wid - dt - 1, w,
		                 from_ascii("xleftrightarrow"));
}


void InsetPhantom::write(ostream & os) const
{
	params_.write(os);
	InsetCollapsible::write(os);
}


void InsetPhantom::read(Lexer & lex)
{
	params_.read(lex);
	InsetCollapsible::read(lex);
}


void InsetPhantom::setButtonLabel()
{
	docstring const label = phantomtranslator_loc().find(params_.type);
	setLabel(label);
}


bool InsetPhantom::showInsetDialog(BufferView * bv) const
{
	bv->showDialog("phantom", params2string(params()),
		const_cast<InsetPhantom *>(this));
	return true;
}


void InsetPhantom::doDispatch(Cursor & cur, FuncRequest & cmd)
{
	switch (cmd.action()) {

	case LFUN_INSET_MODIFY:
		cur.recordUndoInset(this);
		string2params(to_utf8(cmd.argument()), params_);
		setButtonLabel();
		cur.forceBufferUpdate();
		break;

	case LFUN_INSET_DIALOG_UPDATE:
		cur.bv().updateDialog("phantom", params2string(params()));
		break;

	default:
		InsetCollapsible::doDispatch(cur, cmd);
		break;
	}
}


bool InsetPhantom::getStatus(Cursor & cur, FuncRequest const & cmd,
		FuncStatus & flag) const
{
	switch (cmd.action()) {

	case LFUN_INSET_MODIFY:
		if (cmd.getArg(0) == "phantom") {
			InsetPhantomParams params;
			string2params(to_utf8(cmd.argument()), params);
			flag.setOnOff(params_.type == params.type);
		}
		flag.setEnabled(true);
		return true;

	case LFUN_INSET_DIALOG_UPDATE:
		flag.setEnabled(true);
		return true;

	default:
		return InsetCollapsible::getStatus(cur, cmd, flag);
	}
}


docstring InsetPhantom::toolTip(BufferView const &, int, int) const
{
	docstring const res = phantomtranslator_loc().find(params_.type);
	return toolTipText(res + from_ascii(": "));
}


void InsetPhantom::latex(otexstream & os, OutputParams const & runparams) const
{
	if (runparams.moving_arg)
		os << "\\protect";

	switch (params_.type) {
	case InsetPhantomParams::Phantom:
		os << "\\phantom{";
		break;
	case InsetPhantomParams::HPhantom:
		os << "\\hphantom{";
		break;
	case InsetPhantomParams::VPhantom:
		os << "\\vphantom{";
		break;
	default:
		os << "\\phantom{";
		break;
	}
	InsetCollapsible::latex(os, runparams);
	os << "}";
}


int InsetPhantom::plaintext(odocstringstream & os,
			    OutputParams const & runparams, size_t max_length) const
{
	switch (params_.type) {
	case InsetPhantomParams::Phantom:
		os << '[' << buffer().B_("phantom") << ":";
		break;
	case InsetPhantomParams::HPhantom:
		os << '[' << buffer().B_("hphantom") << ":";
		break;
	case InsetPhantomParams::VPhantom:
		os << '[' << buffer().B_("vphantom") << ":";
		break;
	default:
		os << '[' << buffer().B_("phantom") << ":";
		break;
	}
	InsetCollapsible::plaintext(os, runparams, max_length);
	os << "]";

	return PLAINTEXT_NEWLINE;
}


int InsetPhantom::docbook(odocstream & os, OutputParams const & runparams) const
{
	docstring cmdname;
	switch (params_.type) {
	case InsetPhantomParams::Phantom:
	case InsetPhantomParams::HPhantom:
	case InsetPhantomParams::VPhantom:
	default:
		cmdname = from_ascii("phantom");
		break;
	}
	os << "<" + cmdname + ">";
	int const i = InsetCollapsible::docbook(os, runparams);
	os << "</" + cmdname + ">";

	return i;
}


docstring InsetPhantom::xhtml(XHTMLStream &, OutputParams const &) const
{
	return docstring();
}

string InsetPhantom::contextMenuName() const
{
	return "context-phantom";
}


string InsetPhantom::params2string(InsetPhantomParams const & params)
{
	ostringstream data;
	data << "phantom" << ' ';
	params.write(data);
	return data.str();
}


void InsetPhantom::string2params(string const & in, InsetPhantomParams & params)
{
	params = InsetPhantomParams();

	if (in.empty())
		return;

	istringstream data(in);
	Lexer lex;
	lex.setStream(data);
	lex.setContext("InsetPhantom::string2params");
	lex >> "phantom" >> "Phantom";

	params.read(lex);
}


} // namespace lyx

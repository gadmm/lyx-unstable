/**
 * \file InsetSpace.cpp
 * This file is part of LyX, the document processor.
 * Licence details can be found in the file COPYING.
 *
 * \author Asger Alstrup Nielsen
 * \author Jean-Marc Lasgouttes
 * \author Lars Gullik Bjønnes
 * \author Jürgen Spitzmüller
 *
 * Full author contact details are available in file CREDITS.
 */

#include <config.h>

#include "InsetSpace.h"

#include "BufferView.h"
#include "Cursor.h"
#include "Dimension.h"
#include "FuncRequest.h"
#include "FuncStatus.h"
#include "Language.h"
#include "LaTeXFeatures.h"
#include "Length.h"
#include "Lexer.h"
#include "MetricsInfo.h"
#include "OutputParams.h"
#include "output_xhtml.h"
#include "texstream.h"

#include "support/debug.h"
#include "support/docstream.h"
#include "support/gettext.h"
#include "support/lassert.h"
#include "support/lstrings.h"

#include "frontends/Application.h"
#include "frontends/FontMetrics.h"
#include "frontends/Painter.h"

#include <cmath>

using namespace std;

namespace lyx {


InsetSpace::InsetSpace(InsetSpaceParams const & params)
	: Inset(0), params_(params)
{}


InsetSpaceParams::Kind InsetSpace::kind() const
{
	return params_.kind;
}


GlueLength InsetSpace::length() const
{
	return params_.length;
}


docstring InsetSpace::toolTip(BufferView const &, int, int) const
{
	docstring message;
	switch (params_.kind) {
	case InsetSpaceParams::NORMAL:
		message = _("Interword Space");
		break;
	case InsetSpaceParams::PROTECTED:
		message = _("Protected Space");
		break;
	case InsetSpaceParams::VISIBLE:
		message = _("Visible Space");
		break;
	case InsetSpaceParams::THIN:
		message = _("Thin Space");
		break;
	case InsetSpaceParams::MEDIUM:
		message = _("Medium Space");
		break;
	case InsetSpaceParams::THICK:
		message = _("Thick Space");
		break;
	case InsetSpaceParams::QUAD:
		message = _("Quad Space");
		break;
	case InsetSpaceParams::QQUAD:
		message = _("Double Quad Space");
		break;
	case InsetSpaceParams::ENSPACE:
		message = _("Enspace");
		break;
	case InsetSpaceParams::ENSKIP:
		message = _("Enskip");
		break;
	case InsetSpaceParams::NEGTHIN:
		message = _("Negative Thin Space");
		break;
	case InsetSpaceParams::NEGMEDIUM:
		message = _("Negative Medium Space");
		break;
	case InsetSpaceParams::NEGTHICK:
		message = _("Negative Thick Space");
		break;
	case InsetSpaceParams::HFILL:
		message = _("Horizontal Fill");
		break;
	case InsetSpaceParams::HFILL_PROTECTED:
		message = _("Protected Horizontal Fill");
		break;
	case InsetSpaceParams::DOTFILL:
		message = _("Horizontal Fill (Dots)");
		break;
	case InsetSpaceParams::HRULEFILL:
		message = _("Horizontal Fill (Rule)");
		break;
	case InsetSpaceParams::LEFTARROWFILL:
		message = _("Horizontal Fill (Left Arrow)");
		break;
	case InsetSpaceParams::RIGHTARROWFILL:
		message = _("Horizontal Fill (Right Arrow)");
		break;
	case InsetSpaceParams::UPBRACEFILL:
		message = _("Horizontal Fill (Up Brace)");
		break;
	case InsetSpaceParams::DOWNBRACEFILL:
		message = _("Horizontal Fill (Down Brace)");
		break;
	case InsetSpaceParams::CUSTOM:
		// FIXME unicode
		message = support::bformat(_("Horizontal Space (%1$s)"),
				from_ascii(params_.length.asString()));
		break;
	case InsetSpaceParams::CUSTOM_PROTECTED:
		// FIXME unicode
		message = support::bformat(_("Protected Horizontal Space (%1$s)"),
				from_ascii(params_.length.asString()));
		break;
	}
	return message;
}


void InsetSpace::doDispatch(Cursor & cur, FuncRequest & cmd)
{
	switch (cmd.action()) {

	case LFUN_INSET_MODIFY: {
		cur.recordUndo();
		string arg = to_utf8(cmd.argument());
		if (arg == "space \\hspace{}")
			arg += params_.length.len().empty()
				? " \\length 1" + string(stringFromUnit(Length::defaultUnit()))
				: " \\length " + params_.length.asString();
		string2params(arg, params_);
		break;
	}

	case LFUN_INSET_DIALOG_UPDATE:
		cur.bv().updateDialog("space", params2string(params()));
		break;

	default:
		Inset::doDispatch(cur, cmd);
		break;
	}
}


bool InsetSpace::getStatus(Cursor & cur, FuncRequest const & cmd,
	FuncStatus & status) const
{
	switch (cmd.action()) {
	// we handle these
	case LFUN_INSET_MODIFY:
		if (cmd.getArg(0) == "space") {
			InsetSpaceParams params;
			string2params(to_utf8(cmd.argument()), params);
			status.setOnOff(params_.kind == params.kind);
			status.setEnabled(true);
		} else
			status.setEnabled(false);
		return true;

	case LFUN_INSET_DIALOG_UPDATE:
		status.setEnabled(true);
		return true;
	default:
		return Inset::getStatus(cur, cmd, status);
	}
}


void InsetSpace::metrics(MetricsInfo & mi, Dimension & dim) const
{
	frontend::FontMetrics const & fm = theFontMetrics(mi.base.font);
	dim.asc = fm.maxAscent();
	dim.des = fm.maxDescent();

	switch (params_.kind) {
		case InsetSpaceParams::THIN:
		case InsetSpaceParams::NEGTHIN:
			dim.wid = mi.base.mu(3);
			break;
		case InsetSpaceParams::MEDIUM:
		case InsetSpaceParams::NEGMEDIUM:
			dim.wid = mi.base.mu(4);
			break;
		case InsetSpaceParams::THICK:
		case InsetSpaceParams::NEGTHICK:
			dim.wid = mi.base.mu(5);
			break;
		case InsetSpaceParams::PROTECTED:
		case InsetSpaceParams::VISIBLE:
		case InsetSpaceParams::NORMAL:
			dim.wid = fm.width(char_type(' '));
			break;
		case InsetSpaceParams::QUAD:
			dim.wid = mi.base.em(1);
			break;
		case InsetSpaceParams::QQUAD:
			dim.wid = mi.base.em(2);
			break;
		case InsetSpaceParams::ENSPACE:
		case InsetSpaceParams::ENSKIP:
			dim.wid = mi.base.em(0.5);
			break;
		case InsetSpaceParams::CUSTOM:
		case InsetSpaceParams::CUSTOM_PROTECTED: {
			int const w =
				params_.length.len().inPixels(mi.base);
			int const minw = (w < 0)
				? 3 * mi.base.em(0.4) // arrow_size
				: 0;
			dim.wid = max(minw, abs(w));
			break;
		}
		case InsetSpaceParams::HFILL:
		case InsetSpaceParams::HFILL_PROTECTED:
		case InsetSpaceParams::DOTFILL:
		case InsetSpaceParams::HRULEFILL:
		case InsetSpaceParams::LEFTARROWFILL:
		case InsetSpaceParams::RIGHTARROWFILL:
		case InsetSpaceParams::UPBRACEFILL:
		case InsetSpaceParams::DOWNBRACEFILL:
			// The width for hfills is calculated externally in
			// TextMetrics::computeRowMetrics. This is the width when the hfill
			// is not active.
			dim.wid = 5 * mi.base.solidLineThickness();
			break;
	}
}


void InsetSpace::draw(PainterInfo & pi, int x, int y) const
{
	Dimension const dim = dimension(*pi.base.bv);
	double const t = pi.base.solidLineThickness();

	if (isHfill() || params_.length.len().value() < 0) {
		int const asc = theFontMetrics(pi.base.font).ascent('M');
		int const desc = theFontMetrics(pi.base.font).descent('M');
		double const x0 = round(x + t/2);
		double const x1 = round(x + dim.wid - 1.5 * t);
		double const y0 = round(y + desc - t/2);
		double const y1 = round(y - asc - t/2);
		double const y2 = round((y0 + y1) / 2);
		double xoffset = (y0 - y1) / 2;

		// Two tests for very narrow insets
		if (xoffset > x1 - x0
		     && (params_.kind == InsetSpaceParams::LEFTARROWFILL
			 || params_.kind == InsetSpaceParams::RIGHTARROWFILL))
				xoffset = x1 - x0;
		if (xoffset * 6 > (x1 - x0)
		     && (params_.kind == InsetSpaceParams::UPBRACEFILL
			 || params_.kind == InsetSpaceParams::DOWNBRACEFILL))
				xoffset = (x1 - x0) / 6;

		double const x2 = round(x0 + xoffset);
		double const x3 = round(x1 - xoffset);
		double const xm = round((x0 + x1) / 2);
		double const xml = round(xm - xoffset);
		double const xmr = round(xm + xoffset);

		auto draw_bars = [&](Color col, bool dash) {
			auto ls = dash ? frontend::Painter::line_onoffdash :
			    frontend::Painter::line_solid;
			pi.pain.lineDouble(x0, y1, x0, y0, col, t);
			pi.pain.lineDouble(x0, y2, x1, y2, col, t, ls);
			pi.pain.lineDouble(x1, y1, x1, y0, col, t);
		};
		if (params_.kind == InsetSpaceParams::HFILL)
			draw_bars(Color_added_space, true);
		else if (params_.kind == InsetSpaceParams::HFILL_PROTECTED)
			draw_bars(Color_latex, true);
		else if (params_.kind == InsetSpaceParams::DOTFILL)
			draw_bars(Color_special, true);
		else if (params_.kind == InsetSpaceParams::HRULEFILL)
			draw_bars(Color_special, false);
		else if (params_.kind == InsetSpaceParams::LEFTARROWFILL) {
			pi.pain.lineDouble(x2, y1, x0, y2, Color_special, t);
			pi.pain.lineDouble(x0, y2, x2, y0, Color_special, t);
			pi.pain.lineDouble(x0, y2, x1, y2, Color_special, t);
		} else if (params_.kind == InsetSpaceParams::RIGHTARROWFILL) {
			pi.pain.lineDouble(x3, y1, x1, y2, Color_special, t);
			pi.pain.lineDouble(x1, y2, x3, y0, Color_special, t);
			pi.pain.lineDouble(x0, y2, x1, y2, Color_special, t);
		} else if (params_.kind == InsetSpaceParams::UPBRACEFILL) {
			double const xp[7] = {x0, x2, xml, xm, xmr, x3, x1};
			double const yp[7] = {y1, y2, y2,  y0, y2,  y2, y1};
			pi.pain.linesDouble(xp, yp, 7, Color_special, t);
		} else if (params_.kind == InsetSpaceParams::DOWNBRACEFILL) {
			double const xp[7] = {x0, x2, xml, xm, xmr, x3, x1};
			double const yp[7] = {y0, y2, y2,  y1, y2,  y2, y0};
			pi.pain.linesDouble(xp, yp, 7, Color_special, t);
		} else if (params_.kind == InsetSpaceParams::CUSTOM
		           || params_.kind == InsetSpaceParams::CUSTOM_PROTECTED) {
			Color col = params_.kind == InsetSpaceParams::CUSTOM_PROTECTED ?
				Color_latex : Color_special;
			double const xl[3] = {x0, x2, x0};
			double const yl[3] = {y1, y2, y0};
			pi.pain.linesDouble(xl, yl, 3, col, t);
			double const xr[3] = {x1, x3, x1};
			double const yr[3] = {y1, y2, y0};
			pi.pain.linesDouble(xr, yr, 3, col, t);
			pi.pain.lineDouble(x2, y2 , x3, y2, col, t);
		}
		return;
	}

	double const x0 = round(x + t/2);
	double const x1 = round(x + dim.wid - t);
	double const h = theFontMetrics(pi.base.font).ascent('x');
	double const h4 = round(max(h / 4, 1.) - 1.5 * t);
	double xp[4], yp[4];

	xp[0] = x0;
	yp[0] = y - h4;
	if (params_.kind == InsetSpaceParams::NORMAL ||
	    params_.kind == InsetSpaceParams::PROTECTED ||
	    params_.kind == InsetSpaceParams::VISIBLE) {
		xp[1] = x0; yp[1] = y;
		xp[2] = x1; yp[2] = y;
	} else {
		xp[1] = x0; yp[1] = y + h4;
		xp[2] = x1; yp[2] = y + h4;
	}
	xp[3] = x1;
	yp[3] = y - h4;

	Color col = Color_special;
	if (params_.kind == InsetSpaceParams::PROTECTED ||
	    params_.kind == InsetSpaceParams::ENSPACE ||
	    params_.kind == InsetSpaceParams::NEGTHIN ||
	    params_.kind == InsetSpaceParams::NEGMEDIUM ||
	    params_.kind == InsetSpaceParams::NEGTHICK ||
	    params_.kind == InsetSpaceParams::CUSTOM_PROTECTED)
		col = Color_latex;
	else if (params_.kind == InsetSpaceParams::VISIBLE)
		col = Color_foreground;

	pi.pain.linesDouble(xp, yp, 4, col, t);
}


void InsetSpaceParams::write(ostream & os) const
{
	switch (kind) {
	case InsetSpaceParams::NORMAL:
		os << "\\space{}";
		break;
	case InsetSpaceParams::PROTECTED:
		os <<  "~";
		break;
	case InsetSpaceParams::VISIBLE:
		os <<  "\\textvisiblespace{}";
		break;
	case InsetSpaceParams::THIN:
		os <<  "\\thinspace{}";
		break;
	case InsetSpaceParams::MEDIUM:
		os <<  "\\medspace{}";
		break;
	case InsetSpaceParams::THICK:
		os <<  "\\thickspace{}";
		break;
	case InsetSpaceParams::QUAD:
		os <<  "\\quad{}";
		break;
	case InsetSpaceParams::QQUAD:
		os <<  "\\qquad{}";
		break;
	case InsetSpaceParams::ENSPACE:
		os <<  "\\enspace{}";
		break;
	case InsetSpaceParams::ENSKIP:
		os <<  "\\enskip{}";
		break;
	case InsetSpaceParams::NEGTHIN:
		os <<  "\\negthinspace{}";
		break;
	case InsetSpaceParams::NEGMEDIUM:
		os <<  "\\negmedspace{}";
		break;
	case InsetSpaceParams::NEGTHICK:
		os <<  "\\negthickspace{}";
		break;
	case InsetSpaceParams::HFILL:
		os <<  "\\hfill{}";
		break;
	case InsetSpaceParams::HFILL_PROTECTED:
		os <<  "\\hspace*{\\fill}";
		break;
	case InsetSpaceParams::DOTFILL:
		os <<  "\\dotfill{}";
		break;
	case InsetSpaceParams::HRULEFILL:
		os <<  "\\hrulefill{}";
		break;
	case InsetSpaceParams::LEFTARROWFILL:
		os <<  "\\leftarrowfill{}";
		break;
	case InsetSpaceParams::RIGHTARROWFILL:
		os <<  "\\rightarrowfill{}";
		break;
	case InsetSpaceParams::UPBRACEFILL:
		os <<  "\\upbracefill{}";
		break;
	case InsetSpaceParams::DOWNBRACEFILL:
		os <<  "\\downbracefill{}";
		break;
	case InsetSpaceParams::CUSTOM:
		os <<  "\\hspace{}";
		break;
	case InsetSpaceParams::CUSTOM_PROTECTED:
		os <<  "\\hspace*{}";
		break;
	}

	if (!length.len().empty())
		os << "\n\\length " << length.asString();
}


void InsetSpaceParams::read(Lexer & lex)
{
	lex.setContext("InsetSpaceParams::read");
	string command;
	lex >> command;

	// The tests for math might be disabled after a file format change
	if (command == "\\space{}")
		kind = InsetSpaceParams::NORMAL;
	else if (command == "~")
		kind = InsetSpaceParams::PROTECTED;
	else if (command == "\\textvisiblespace{}")
		kind = InsetSpaceParams::VISIBLE;
	else if (command == "\\thinspace{}")
		kind = InsetSpaceParams::THIN;
	else if (math && command == "\\medspace{}")
		kind = InsetSpaceParams::MEDIUM;
	else if (math && command == "\\thickspace{}")
		kind = InsetSpaceParams::THICK;
	else if (command == "\\quad{}")
		kind = InsetSpaceParams::QUAD;
	else if (command == "\\qquad{}")
		kind = InsetSpaceParams::QQUAD;
	else if (command == "\\enspace{}")
		kind = InsetSpaceParams::ENSPACE;
	else if (command == "\\enskip{}")
		kind = InsetSpaceParams::ENSKIP;
	else if (command == "\\negthinspace{}")
		kind = InsetSpaceParams::NEGTHIN;
	else if (command == "\\negmedspace{}")
		kind = InsetSpaceParams::NEGMEDIUM;
	else if (command == "\\negthickspace{}")
		kind = InsetSpaceParams::NEGTHICK;
	else if (command == "\\hfill{}")
		kind = InsetSpaceParams::HFILL;
	else if (command == "\\hspace*{\\fill}")
		kind = InsetSpaceParams::HFILL_PROTECTED;
	else if (command == "\\dotfill{}")
		kind = InsetSpaceParams::DOTFILL;
	else if (command == "\\hrulefill{}")
		kind = InsetSpaceParams::HRULEFILL;
	else if (command == "\\hspace{}")
		kind = InsetSpaceParams::CUSTOM;
	else if (command == "\\leftarrowfill{}")
		kind = InsetSpaceParams::LEFTARROWFILL;
	else if (command == "\\rightarrowfill{}")
		kind = InsetSpaceParams::RIGHTARROWFILL;
	else if (command == "\\upbracefill{}")
		kind = InsetSpaceParams::UPBRACEFILL;
	else if (command == "\\downbracefill{}")
		kind = InsetSpaceParams::DOWNBRACEFILL;
	else if (command == "\\hspace*{}")
		kind = InsetSpaceParams::CUSTOM_PROTECTED;
	else
		lex.printError("InsetSpace: Unknown kind: `$$Token'");

	if (lex.checkFor("\\length"))
		lex >> length;
}


void InsetSpace::write(ostream & os) const
{
	os << "space ";
	params_.write(os);
}


void InsetSpace::read(Lexer & lex)
{
	params_.read(lex);
	lex >> "\\end_inset";
}


void InsetSpace::latex(otexstream & os, OutputParams const & runparams) const
{
	switch (params_.kind) {
	case InsetSpaceParams::NORMAL:
		os << (runparams.free_spacing ? " " : "\\ ");
		break;
	case InsetSpaceParams::PROTECTED:
		if (runparams.local_font &&
		    runparams.local_font->language()->lang() == "polutonikogreek")
			// in babel's polutonikogreek, ~ is active
			os << (runparams.free_spacing ? " " : "\\nobreakspace{}");
		else
			os << (runparams.free_spacing ? ' ' : '~');
		break;
	case InsetSpaceParams::VISIBLE:
		os << (runparams.free_spacing ? " " : "\\textvisiblespace{}");
		break;
	case InsetSpaceParams::THIN:
		os << (runparams.free_spacing ? " " : "\\,");
		break;
	case InsetSpaceParams::MEDIUM:
		os << (runparams.free_spacing ? " " : "\\:");
		break;
	case InsetSpaceParams::THICK:
		os << (runparams.free_spacing ? " " : "\\;");
		break;
	case InsetSpaceParams::QUAD:
		os << (runparams.free_spacing ? " " : "\\quad{}");
		break;
	case InsetSpaceParams::QQUAD:
		os << (runparams.free_spacing ? " " : "\\qquad{}");
		break;
	case InsetSpaceParams::ENSPACE:
		os << (runparams.free_spacing ? " " : "\\enspace{}");
		break;
	case InsetSpaceParams::ENSKIP:
		os << (runparams.free_spacing ? " " : "\\enskip{}");
		break;
	case InsetSpaceParams::NEGTHIN:
		os << (runparams.free_spacing ? " " : "\\negthinspace{}");
		break;
	case InsetSpaceParams::NEGMEDIUM:
		os << (runparams.free_spacing ? " " : "\\negmedspace{}");
		break;
	case InsetSpaceParams::NEGTHICK:
		os << (runparams.free_spacing ? " " : "\\negthickspace{}");
		break;
	case InsetSpaceParams::HFILL:
		os << (runparams.free_spacing ? " " : "\\hfill{}");
		break;
	case InsetSpaceParams::HFILL_PROTECTED:
		os << (runparams.free_spacing ? " " : "\\hspace*{\\fill}");
		break;
	case InsetSpaceParams::DOTFILL:
		os << (runparams.free_spacing ? " " : "\\dotfill{}");
		break;
	case InsetSpaceParams::HRULEFILL:
		os << (runparams.free_spacing ? " " : "\\hrulefill{}");
		break;
	case InsetSpaceParams::LEFTARROWFILL:
		os << (runparams.free_spacing ? " " : "\\leftarrowfill{}");
		break;
	case InsetSpaceParams::RIGHTARROWFILL:
		os << (runparams.free_spacing ? " " : "\\rightarrowfill{}");
		break;
	case InsetSpaceParams::UPBRACEFILL:
		os << (runparams.free_spacing ? " " : "\\upbracefill{}");
		break;
	case InsetSpaceParams::DOWNBRACEFILL:
		os << (runparams.free_spacing ? " " : "\\downbracefill{}");
		break;
	case InsetSpaceParams::CUSTOM:
		if (runparams.free_spacing)
			os << " ";
		else
			os << "\\hspace{" << from_ascii(params_.length.asLatexString()) << "}";
		break;
	case InsetSpaceParams::CUSTOM_PROTECTED:
		if (runparams.free_spacing)
			os << " ";
		else
			os << "\\hspace*{" << from_ascii(params_.length.asLatexString()) << "}";
		break;
	}
}


int InsetSpace::plaintext(odocstringstream & os,
        OutputParams const &, size_t) const
{
	switch (params_.kind) {
	case InsetSpaceParams::HFILL:
	case InsetSpaceParams::HFILL_PROTECTED:
		os << "     ";
		return 5;
	case InsetSpaceParams::DOTFILL:
		os << ".....";
		return 5;
	case InsetSpaceParams::HRULEFILL:
		os << "_____";
		return 5;
	case InsetSpaceParams::LEFTARROWFILL:
		os << "<----";
		return 5;
	case InsetSpaceParams::RIGHTARROWFILL:
		os << "---->";
		return 5;
	case InsetSpaceParams::UPBRACEFILL:
		os << "\\-v-/";
		return 5;
	case InsetSpaceParams::DOWNBRACEFILL:
		os << "/-^-\\";
		return 5;
	case InsetSpaceParams::VISIBLE:
		os.put(0x2423);
		return 1;
	case InsetSpaceParams::ENSKIP:
		os.put(0x2002);
		return 1;
	case InsetSpaceParams::ENSPACE:
		os.put(0x2060); // WORD JOINER, makes the breakable en space unbreakable
		os.put(0x2002);
		os.put(0x2060); // WORD JOINER, makes the breakable en space unbreakable
		return 3;
	case InsetSpaceParams::QUAD:
		os.put(0x2003);
		return 1;
	case InsetSpaceParams::QQUAD:
		os.put(0x2003);
		os.put(0x2003);
		return 2;
	case InsetSpaceParams::THIN:
		os.put(0x202f);
		return 1;
	case InsetSpaceParams::MEDIUM:
		os.put(0x200b); // ZERO WIDTH SPACE, makes the unbreakable medium space breakable
		os.put(0x2005);
		os.put(0x200b); // ZERO WIDTH SPACE, makes the unbreakable medium space breakable
		return 1;
	case InsetSpaceParams::THICK:
		os.put(0x200b); // ZERO WIDTH SPACE, makes the unbreakable thick space breakable
		os.put(0x2004);
		os.put(0x200b); // ZERO WIDTH SPACE, makes the unbreakable thick space breakable
		return 1;
	case InsetSpaceParams::PROTECTED:
	case InsetSpaceParams::CUSTOM_PROTECTED:
		os.put(0x00a0);
		return 1;
	case InsetSpaceParams::NEGTHIN:
	case InsetSpaceParams::NEGMEDIUM:
	case InsetSpaceParams::NEGTHICK:
		return 0;
	default:
		os << ' ';
		return 1;
	}
}


int InsetSpace::docbook(odocstream & os, OutputParams const &) const
{
	switch (params_.kind) {
	case InsetSpaceParams::NORMAL:
		os << " ";
		break;
	case InsetSpaceParams::QUAD:
		os << "&emsp;";
		break;
	case InsetSpaceParams::QQUAD:
		os << "&emsp;&emsp;";
		break;
	case InsetSpaceParams::ENSKIP:
		os << "&ensp;";
		break;
	case InsetSpaceParams::PROTECTED:
		os << "&nbsp;";
		break;
	case InsetSpaceParams::VISIBLE:
		os << "&#x2423;";
		break;
	case InsetSpaceParams::ENSPACE:
		os << "&#x2060;&ensp;&#x2060;";
		break;
	case InsetSpaceParams::THIN:
		os << "&thinsp;";
		break;
	case InsetSpaceParams::MEDIUM:
		os << "&emsp14;";
		break;
	case InsetSpaceParams::THICK:
		os << "&emsp13;";
		break;
	case InsetSpaceParams::NEGTHIN:
	case InsetSpaceParams::NEGMEDIUM:
	case InsetSpaceParams::NEGTHICK:
		// FIXME
		os << "&nbsp;";
		break;
	case InsetSpaceParams::HFILL:
	case InsetSpaceParams::HFILL_PROTECTED:
		os << '\n';
		break;
	case InsetSpaceParams::DOTFILL:
		// FIXME
		os << '\n';
		break;
	case InsetSpaceParams::HRULEFILL:
		// FIXME
		os << '\n';
		break;
	case InsetSpaceParams::LEFTARROWFILL:
	case InsetSpaceParams::RIGHTARROWFILL:
	case InsetSpaceParams::UPBRACEFILL:
	case InsetSpaceParams::DOWNBRACEFILL:
	case InsetSpaceParams::CUSTOM:
	case InsetSpaceParams::CUSTOM_PROTECTED:
		// FIXME
		os << '\n';
		break;
	}
	return 0;
}


docstring InsetSpace::xhtml(XHTMLStream & xs, OutputParams const &) const
{
	string output;
	switch (params_.kind) {
	case InsetSpaceParams::NORMAL:
		output = " ";
		break;
	case InsetSpaceParams::ENSKIP:
		output ="&ensp;";
		break;
	case InsetSpaceParams::ENSPACE:
		output ="&#x2060;&ensp;&#x2060;";
		break;
	case InsetSpaceParams::QQUAD:
		output ="&emsp;&emsp;";
		break;
	case InsetSpaceParams::THICK:
		output ="&#x2004;";
		break;
	case InsetSpaceParams::QUAD:
		output ="&emsp;";
		break;
	case InsetSpaceParams::MEDIUM:
		output ="&#x2005;";
		break;
	case InsetSpaceParams::THIN:
		output ="&thinsp;";
		break;
	case InsetSpaceParams::PROTECTED:
	case InsetSpaceParams::NEGTHIN:
	case InsetSpaceParams::NEGMEDIUM:
	case InsetSpaceParams::NEGTHICK:
		output ="&nbsp;";
		break;
	// no XHTML entity, only unicode code for space character exists
	case InsetSpaceParams::VISIBLE:
		output ="&#x2423;";
		break;
	case InsetSpaceParams::HFILL:
	case InsetSpaceParams::HFILL_PROTECTED:
	case InsetSpaceParams::DOTFILL:
	case InsetSpaceParams::HRULEFILL:
	case InsetSpaceParams::LEFTARROWFILL:
	case InsetSpaceParams::RIGHTARROWFILL:
	case InsetSpaceParams::UPBRACEFILL:
	case InsetSpaceParams::DOWNBRACEFILL:
		// FIXME XHTML
		// Can we do anything with those in HTML?
		break;
	case InsetSpaceParams::CUSTOM:
		// FIXME XHTML
		// Probably we could do some sort of blank span?
		break;
	case InsetSpaceParams::CUSTOM_PROTECTED:
		// FIXME XHTML
		// Probably we could do some sort of blank span?
		output ="&nbsp;";
		break;
	}
	// don't escape the entities!
	xs << XHTMLStream::ESCAPE_NONE << from_ascii(output);
	return docstring();
}


void InsetSpace::validate(LaTeXFeatures & features) const
{
	if (params_.kind == InsetSpaceParams::NEGMEDIUM ||
	    params_.kind == InsetSpaceParams::NEGTHICK)
		features.require("amsmath");
}


void InsetSpace::toString(odocstream & os) const
{
	odocstringstream ods;
	plaintext(ods, OutputParams(0));
	os << ods.str();
}


void InsetSpace::forOutliner(docstring & os, size_t const, bool const) const
{
	// There's no need to be cute here.
	os += " ";
}


bool InsetSpace::isHfill() const
{
	return params_.kind == InsetSpaceParams::HFILL
		|| params_.kind == InsetSpaceParams::HFILL_PROTECTED
		|| params_.kind == InsetSpaceParams::DOTFILL
		|| params_.kind == InsetSpaceParams::HRULEFILL
		|| params_.kind == InsetSpaceParams::LEFTARROWFILL
		|| params_.kind == InsetSpaceParams::RIGHTARROWFILL
		|| params_.kind == InsetSpaceParams::UPBRACEFILL
		|| params_.kind == InsetSpaceParams::DOWNBRACEFILL;
}


string InsetSpace::contextMenuName() const
{
	return "context-space";
}


void InsetSpace::string2params(string const & in, InsetSpaceParams & params)
{
	params = InsetSpaceParams();
	if (in.empty())
		return;

	istringstream data(in);
	Lexer lex;
	lex.setStream(data);
	lex.setContext("InsetSpace::string2params");
	lex.next();
	string const name = lex.getString();
	if (name == "mathspace")
		params.math = true;
	else {
		params.math = false;
		// we can try to read this even if the name is wrong
		LATTEST(name == "space");
	}

	// There are cases, such as when we are called via getStatus() from
	// Dialog::canApply(), where we are just called with "space" rather
	// than a full "space \type{}\n\\end_inset".
	if (lex.isOK())
		params.read(lex);
}


string InsetSpace::params2string(InsetSpaceParams const & params)
{
	ostringstream data;
	if (params.math)
		data << "math";
	data << "space" << ' ';
	params.write(data);
	return data.str();
}


} // namespace lyx

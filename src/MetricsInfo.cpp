/**
 * \file MetricsInfo.cpp
 * This file is part of LyX, the document processor.
 * Licence details can be found in the file COPYING.
 *
 * \author André Pönitz
 *
 * Full author contact details are available in file CREDITS.
 */

#include <config.h>

#include "BufferView.h"
#include "ColorSet.h"
#include "LyXRC.h"
#include "MetricsInfo.h"

#include "insets/Inset.h"

#include "mathed/MathSupport.h"

#include "frontends/FontMetrics.h"
#include "frontends/Painter.h"

#include "support/docstring.h"
#include "support/lassert.h"
#include "support/RefChanger.h"

using namespace std;


namespace lyx {

/////////////////////////////////////////////////////////////////////////
//
// MetricsBase
//
/////////////////////////////////////////////////////////////////////////

MetricsBase::MetricsBase(BufferView * b, FontInfo f, int w)
	: bv(b), font(move(f)), fontname("mathnormal"),
	  textwidth(w), macro_nesting(0),
	  solid_line_thickness_(b ? b->inPixelsDouble(Length(0.5, Length::PT)) : 1)
{}


double MetricsBase::thinLineThickness() const
{
	double const t = 0.55 * solid_line_thickness_;
	return (t < 1) ? min(1., solid_line_thickness_) : t;
}


Changer MetricsBase::changeFontSet(string const & name)
{
	RefChanger<MetricsBase> rc = make_save(*this);
	ColorCode oldcolor = font.color();
	string const oldname = fontname;
	fontname = name;
	if (isMathFont(name) || isMathFont(oldname))
		font = sane_font;
	augmentFont(font, name);
	font.setSize(rc->old.font.size());
	font.setStyle(rc->old.font.style());
	if (name != "lyxtex"
	    && ((isTextFont(oldname) && oldcolor != Color_foreground)
	        || (isMathFont(oldname) && oldcolor != Color_math)))
		font.setColor(oldcolor);
	return move(rc);
}


Changer MetricsBase::changeEnsureMath(Inset::mode_type mode)
{
	switch (mode) {
	case Inset::UNDECIDED_MODE:
		return Changer();
	case Inset::TEXT_MODE:
		return isMathFont(fontname) ? changeFontSet("textnormal") : Changer();
	case Inset::MATH_MODE:
		// FIXME:
		//   \textit{\ensuremath{\text{a}}}
		// should appear in italics
		return isTextFont(fontname) ? changeFontSet("mathnormal"): Changer();
	}
	return Changer();
}


int MetricsBase::inPixels(Length const & len) const
{
	return (int) std::round(inPixelsDouble(len));
}


int MetricsBase::inPixelsDouble(Length const & len) const
{
	FontInfo fi = font;
	if (len.unit() == Length::MU)
		// mu is 1/18th of an em in the math symbol font
		fi.setFamily(SYMBOL_FAMILY);
	else
		// Math style is only taken into account in the case of mu
		fi.setStyle(LM_ST_TEXT);
	return len.inPixelsDouble(textwidth, theFontMetrics(fi).em());
}


int MetricsBase::mu(double len) const
{
	return mathed_mu(font, len);
}


int MetricsBase::em(double len) const
{
	return Length(len, Length::EM).inPixels(*this);
}


/////////////////////////////////////////////////////////////////////////
//
// MetricsInfo
//
/////////////////////////////////////////////////////////////////////////

MetricsInfo::MetricsInfo(BufferView * bv, FontInfo font, int textwidth,
                         MacroContext const & mc)
	: base(bv, font, textwidth), macrocontext(mc)
{}


/////////////////////////////////////////////////////////////////////////
//
// PainterInfo
//
/////////////////////////////////////////////////////////////////////////

PainterInfo::PainterInfo(BufferView * bv, lyx::frontend::Painter & painter)
	: base(bv), pain(painter), ltr_pos(false), change_(), selected(false),
	do_spellcheck(true), full_repaint(true), background_color(Color_background)
{}


void PainterInfo::draw(int x, int y, char_type c)
{
	pain.text(x, y, c, base.font);
}


void PainterInfo::draw(int x, int y, docstring const & str)
{
	pain.text(x, y, str, base.font);
}


ColorCode PainterInfo::backgroundColor(Inset const * inset, bool sel) const
{
	ColorCode const color_bg = inset->backgroundColor(*this);

	if (selected && sel)
		// This inset is in a selection
		return Color_selection;
	else {
		if (color_bg != Color_none)
			// This inset has its own color
			return color_bg;
		else {
			if (background_color == Color_none)
				// This inset has no own color and does not inherit a color
				return Color_background;
			else
				// This inset has no own color, but inherits a color
				return background_color;
		}
	}
}


Color PainterInfo::textColor(Color const & color) const
{
	if (change_.changed())
		return change_.color();
	if (selected)
		return Color_selectiontext;
	return color;
}


Changer MetricsBase::changeScript()
{
	switch (font.style()) {
	case LM_ST_DISPLAY:
	case LM_ST_TEXT:
		return font.changeStyle(LM_ST_SCRIPT);
	case LM_ST_SCRIPT:
	case LM_ST_SCRIPTSCRIPT:
		return font.changeStyle(LM_ST_SCRIPTSCRIPT);
	}
	//remove Warning
	return Changer();
}


Changer MetricsBase::changeFrac()
{
	switch (font.style()) {
	case LM_ST_DISPLAY:
		return font.changeStyle(LM_ST_TEXT);
	case LM_ST_TEXT:
		return font.changeStyle(LM_ST_SCRIPT);
	case LM_ST_SCRIPT:
	case LM_ST_SCRIPTSCRIPT:
		return font.changeStyle(LM_ST_SCRIPTSCRIPT);
	}
	//remove Warning
	return Changer();
}


Changer MetricsBase::changeArray()
{
	return (font.style() == LM_ST_DISPLAY) ? font.changeStyle(LM_ST_TEXT)
		: Changer();
}


} // namespace lyx

/**
 * \file RenderButton.cpp
 * This file is part of LyX, the document processor.
 * Licence details can be found in the file COPYING.
 *
 * \author Angus Leeming
 *
 * Full author contact details are available in file CREDITS.
 */

#include <config.h>

#include "RenderButton.h"

#include "MetricsInfo.h"

#include "frontends/FontMetrics.h"
#include "frontends/Painter.h"


namespace lyx {


RenderButton::RenderButton()
	: editable_(false), inherit_font_(false)
{}


RenderBase * RenderButton::clone(Inset const *) const
{
	return new RenderButton(*this);
}


void RenderButton::update(docstring const & text, bool editable,
                          bool inherit)
{
	text_ = text;
	editable_ = editable;
	inherit_font_ = inherit;
}


void RenderButton::elideMode(std::pair<ElideMode, double> mode)
{
	elide_mode_ = mode.first;
	elide_length_ = mode.second;
}


void RenderButton::metrics(MetricsInfo & mi, Dimension & dim) const
{
	FontInfo font = inherit_font_ ? mi.base.font : sane_font;
	font.decSize();
	frontend::FontMetrics const & fm = theFontMetrics(font);

	docstring const text = fm.elideText(text_, elide_mode_, elide_length_);

	fm.buttonText(text, mi.base.textToInsetOffset(), dim.wid, dim.asc, dim.des);

	dim_ = dim;
}


void RenderButton::draw(PainterInfo & pi, int x, int y) const
{
	// Draw it as a box with the LaTeX text
	FontInfo font = inherit_font_ ? pi.base.font : sane_font;
	font.setColor(Color_command);
	font.decSize();

	frontend::FontMetrics const & fm = theFontMetrics(font);
	docstring const text = fm.elideText(text_, elide_mode_, elide_length_);

	double const t = pi.base.solidLineThickness();
	int const offset = pi.base.textToInsetOffset();
	if (editable_) {
		pi.pain.buttonText(x, y, text, font,
		                   renderState() ? Color_buttonhoverbg : Color_buttonbg,
		                   Color_buttonframe, offset, t);
	} else {
		pi.pain.buttonText(x, y, text, font,
		                   Color_commandbg, Color_commandframe,
		                   offset, t);
	}
}


} // namespace lyx

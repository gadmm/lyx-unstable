// -*- C++ -*-
/**
 * \file RenderButton.h
 * This file is part of LyX, the document processor.
 * Licence details can be found in the file COPYING.
 *
 * \author Angus Leeming
 *
 * Full author contact details are available in file CREDITS.
 */

#ifndef RENDERBUTTON_H
#define RENDERBUTTON_H

#include "RenderBase.h"
#include "Box.h"

#include "support/docstring.h"

#include "frontends/FontMetrics.h"

namespace lyx {


class RenderButton : public RenderBase
{
public:
	RenderButton();

	RenderBase * clone(Inset const *) const;

	/// compute the size of the object returned in dim
	virtual void metrics(MetricsInfo & mi, Dimension & dim) const;
	/// draw inset and update (xo, yo)-cache
	virtual void draw(PainterInfo & pi, int x, int y) const;

	/// Provide the text for the button
	void update(docstring const &, bool editable, bool inherit_font);
	void elideMode(std::pair<ElideMode, double> mode);

	/// The "sensitive area" box, i.e., the button area
	Box box() const { return button_box_; }
	///
	void setBox(Box b) { button_box_ = b; }

	/// equivalent to dynamic_cast
	virtual RenderButton * asButton() { return this; }

private:
	/// The stored data.
	docstring text_;
	bool editable_;
	bool inherit_font_;
	Box button_box_;
	// How to elide the label
	ElideMode elide_mode_ = ElideNone;
	// Length of the elided label in ems
	double elide_length_ = 0;
};


} // namespace lyx

#endif // NOT RENDERBUTTON_H

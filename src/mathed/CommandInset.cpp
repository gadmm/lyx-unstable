/**
 * \file CommandInset.cpp
 * This file is part of LyX, the document processor.
 * Licence details can be found in the file COPYING.
 *
 * \author André Pönitz
 *
 * Full author contact details are available in file CREDITS.
 */

#include <config.h>

#include "CommandInset.h"
#include "MathData.h"
#include "MathStream.h"
#include "DispatchResult.h"

#include <sstream>

using namespace std;

namespace lyx {


CommandInset::CommandInset(Buffer * buf, docstring const & name, bool needs_math_mode)
	: InsetMathNest(buf, 2), name_(name), needs_math_mode_(needs_math_mode),
	  set_label_(false)
{
	lock_ = true;
}


Inset * CommandInset::clone() const
{
	return new CommandInset(*this);
}


void CommandInset::metrics(MetricsInfo & mi, Dimension & dim) const
{
	if (!set_label_) {
		set_label_ = true;
		button_.update(screenLabel(), true, false);
	}
	button_.metrics(mi, dim);
}


Inset * CommandInset::editXY(Cursor & cur, int /*x*/, int /*y*/)
{
	edit(cur, true);
	return this;
}


void CommandInset::draw(PainterInfo & pi, int x, int y) const
{
	button_.draw(pi, x, y);
}


void CommandInset::write(WriteStream & os) const
{
	ModeSpecifier specifier(os, currentMode(), lockedMode(), asciiOnly());
	MathEnsurer ensurer(os, needs_math_mode_);
	os << '\\' << name_;
	if (!cell(1).empty())
		os << '[' << cell(1) << ']';
	os << '{' << cell(0) << '}';
}


docstring const CommandInset::screenLabel() const
{
	return name_;
}

} // namespace lyx

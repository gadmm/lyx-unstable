// -*- C++ -*-
/**
 * \file InsetMathRoot.h
 * This file is part of LyX, the document processor.
 * Licence details can be found in the file COPYING.
 *
 * \author Alejandro Aguilar Sierra
 * \author André Pönitz
 *
 * Full author contact details are available in file CREDITS.
 */

#ifndef MATH_ROOT_H
#define MATH_ROOT_H

#include "InsetMathNest.h"


namespace lyx {


/// The general n-th root inset.
class InsetMathRoot : public InsetMathNest {
public:
	///
	InsetMathRoot(Buffer * buf);
	///
	bool idxUpDown(Cursor & cur, bool up) const;
	///
	void metrics(MetricsInfo & mi, Dimension & dim) const;
	///
	void draw(PainterInfo & pi, int x, int y) const;
	///
	idx_type firstIdx() const { return 1; }
	///
	idx_type lastIdx() const { return 1; }

	///
	void write(WriteStream & os) const;
	///
	void normalize(NormalStream &) const;
	///
	void mathmlize(MathStream &) const;
	///
	void htmlize(HtmlStream &) const;
	///
	void maple(MapleStream &) const;
	///
	void mathematica(MathematicaStream &) const;
	///
	void octave(OctaveStream &) const;
	///
	InsetCode lyxCode() const { return MATH_ROOT_CODE; }
	///
	void validate(LaTeXFeatures &) const;

private:
	virtual Inset * clone() const;
};



} // namespace lyx
#endif

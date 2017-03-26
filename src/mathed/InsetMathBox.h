// -*- C++ -*-
/**
 * \file InsetMathBox.h
 * This file is part of LyX, the document processor.
 * Licence details can be found in the file COPYING.
 *
 * \author André Pönitz
 *
 * Full author contact details are available in file CREDITS.
 */

#ifndef MATH_BOXINSET_H
#define MATH_BOXINSET_H

#include "InsetMathNest.h"


namespace lyx {

/// Support for \\mbox
class InsetMathBox : public InsetMathNest {
public:
	///
	explicit InsetMathBox(Buffer * buf, docstring const & name);
	///
	mode_type currentMode() const { return TEXT_MODE; }
	///
	void metrics(MetricsInfo & mi, Dimension & dim) const;
	///
	void draw(PainterInfo & pi, int x, int y) const;
	///
	void write(WriteStream & os) const;
	///
	void normalize(NormalStream & ns) const;
	///
	void mathmlize(MathStream & ms) const;
	///
	void htmlize(HtmlStream & ms) const;
	///
	void infoize(odocstream & os) const;
	///
	void validate(LaTeXFeatures & features) const;
	///
	InsetCode lyxCode() const { return MATH_BOX_CODE; }

private:
	Inset * clone() const { return new InsetMathBox(*this); }
	///
	docstring name_;
};


/// Non-AMS-style frame
class InsetMathFBox : public InsetMathNest {
public:
	///
	InsetMathFBox(Buffer * buf);
	///
	mode_type currentMode() const { return TEXT_MODE; }
	///
	marker_type marker(BufferView const *) const { return NO_MARKER; }
	///
	void metrics(MetricsInfo & mi, Dimension & dim) const;
	///
	void draw(PainterInfo & pi, int x, int y) const;
	///
	void write(WriteStream & os) const;
	/// write normalized content
	void normalize(NormalStream & ns) const;
	///
	void mathmlize(MathStream & ms) const;
	///
	void htmlize(HtmlStream & ms) const;
	///
	void infoize(odocstream & os) const;
	///
	void validate(LaTeXFeatures & features) const;
private:
	///
	Inset * clone() const { return new InsetMathFBox(*this); }
};


/// Extra nesting: \\makebox or \\framebox.
class InsetMathMakebox : public InsetMathNest {
public:
	///
	InsetMathMakebox(Buffer * buf, bool framebox);
	///
	void metrics(MetricsInfo & mi, Dimension & dim) const;
	///
	void draw(PainterInfo & pi, int x, int y) const;
	///
	void write(WriteStream & os) const;
	/// write normalized content
	void normalize(NormalStream & ns) const;
	///
	void mathmlize(MathStream & ms) const;
	///
	void htmlize(HtmlStream & ms) const;
	///
	mode_type currentMode() const { return TEXT_MODE; }
	///
	void infoize(odocstream & os) const;
	///
	void validate(LaTeXFeatures & features) const;
private:
	Inset * clone() const { return new InsetMathMakebox(*this); }
	///
	bool framebox_;
};



/// AMS-style frame
class InsetMathBoxed : public InsetMathNest {
public:
	///
	InsetMathBoxed(Buffer * buf);
	///
	marker_type marker(BufferView const *) const { return NO_MARKER; }
	///
	void validate(LaTeXFeatures & features) const;
	///
	void metrics(MetricsInfo & mi, Dimension & dim) const;
	///
	void draw(PainterInfo & pi, int x, int y) const;
	///
	void write(WriteStream & os) const;
	///
	void mathmlize(MathStream & ms) const;
	///
	void htmlize(HtmlStream & ms) const;
	/// write normalized content
	void normalize(NormalStream & ns) const;
	///
	void infoize(odocstream & os) const;
private:
	Inset * clone() const { return new InsetMathBoxed(*this); }
};


} // namespace lyx

#endif // MATH_MBOX

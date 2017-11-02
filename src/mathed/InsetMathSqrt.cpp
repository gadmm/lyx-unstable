/**
 * \file InsetMathSqrt.cpp
 * This file is part of LyX, the document processor.
 * Licence details can be found in the file COPYING.
 *
 * \author André Pönitz
 *
 * Full author contact details are available in file CREDITS.
 */

#include <config.h>

#include "InsetMathSqrt.h"

#include "MathData.h"
#include "MathStream.h"
#include "MathSupport.h"

#include "LaTeXFeatures.h"
#include "MetricsInfo.h"
#include "TextPainter.h"

#include "frontends/Painter.h"

#include <cmath>


namespace lyx {

InsetMathSqrt::InsetMathSqrt(Buffer * buf)
	: InsetMathNest(buf, 1)
{}


Inset * InsetMathSqrt::clone() const
{
	return new InsetMathSqrt(*this);
}


void InsetMathSqrt::metrics(MetricsInfo & mi, Dimension & dim) const
{
	Changer dummy = mi.base.changeEnsureMath();
	cell(0).metrics(mi, dim);
	// make sure that the dim is high enough for any character
	Dimension fontDim;
	math_font_max_dim(mi.base.font, fontDim.asc, fontDim.des);
	dim += fontDim;
	// Some room for the decoration
	dim.asc += (int) 3 * mi.base.solidLineThickness();
	dim.wid += mi.base.mu(4);
}


void InsetMathSqrt::draw(PainterInfo & pi, int x, int y) const
{
	Changer dummy = pi.base.changeEnsureMath();
	double const t = pi.base.solidLineThickness();
	double const deco_w = pi.base.mu(4);
	cell(0).draw(pi, x + (int) (deco_w + 2 * t), y);
	Dimension const dim = dimension(*pi.base.bv);
	double const a = dim.ascent();
	double const d = dim.descent();
	// coords of the end of the decoration
	double const x0 = x + dim.width() - t/2;
	double const y0 = round(y - a + t/2);
	// shape of the decoration
	double const xp[4] = {x0, x + deco_w, x + deco_w * 4/7, x + t/2};
	double const yp[4] = {y0, y0,         y + d,            y + (d - a)/2};
	pi.pain.linesDouble(xp, yp, 4, pi.base.font.color(), t);
}


void InsetMathSqrt::metricsT(TextMetricsInfo const & mi, Dimension & dim) const
{
	cell(0).metricsT(mi, dim);
	dim.asc += 1;
	dim.wid += 2;
}


void InsetMathSqrt::drawT(TextPainter & /*pain*/, int /*x*/, int /*y*/) const
{
	/*
	cell(0).drawT(pain, x + 2, y);
	Dimension const & dim0 = cell(0).dimension(*pi.base.bv);
	pain.horizontalLine(x + 2, y - dim0.ascent(), dim0.width(), '_');
	pain.verticalLine  (x + 1, y - dim0.ascent() + 1, dim0.height());
	pain.draw(x, y + dim0.descent(), '\\');
	*/
}


void InsetMathSqrt::write(WriteStream & os) const
{
	MathEnsurer ensurer(os);
	os << "\\sqrt{" << cell(0) << '}';
}


void InsetMathSqrt::normalize(NormalStream & os) const
{
	os << "[sqrt " << cell(0) << ']';
}

void InsetMathSqrt::maple(MapleStream & os) const
{
	os << "sqrt(" << cell(0) << ')';
}

void InsetMathSqrt::mathematica(MathematicaStream & os) const
{
	os << "Sqrt[" << cell(0) << ']';
}


void InsetMathSqrt::octave(OctaveStream & os) const
{
	os << "sqrt(" << cell(0) << ')';
}


void InsetMathSqrt::mathmlize(MathStream & os) const
{
	os << MTag("msqrt") << cell(0) << ETag("msqrt");
}


void InsetMathSqrt::htmlize(HtmlStream & os) const
{
	os << MTag("span", "class='sqrt'")
	   << from_ascii("&radic;")
	   << MTag("span", "class='sqrtof'")	<< cell(0) << ETag("span")
		 << ETag("span");
}


void InsetMathSqrt::validate(LaTeXFeatures & features) const
{
	if (features.runparams().math_flavor == OutputParams::MathAsHTML)
		features.addCSSSnippet("span.sqrtof{border-top: thin solid black;}");
	InsetMathNest::validate(features);
}

} // namespace lyx

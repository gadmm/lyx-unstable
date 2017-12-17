/**
 * \file InsetMathRoot.cpp
 * This file is part of LyX, the document processor.
 * Licence details can be found in the file COPYING.
 *
 * \author Alejandro Aguilar Sierra
 * \author André Pönitz
 *
 * Full author contact details are available in file CREDITS.
 */

#include <config.h>

#include "InsetMathRoot.h"

#include "MathData.h"
#include "MathStream.h"
#include "MathSupport.h"

#include "Cursor.h"
#include "LaTeXFeatures.h"
#include "MetricsInfo.h"

#include "frontends/Painter.h"

#include <cmath>

using namespace std;

namespace lyx {


InsetMathRoot::InsetMathRoot(Buffer * buf)
	: InsetMathNest(buf, 2)
{}


Inset * InsetMathRoot::clone() const
{
	return new InsetMathRoot(*this);
}


void InsetMathRoot::metrics(MetricsInfo & mi, Dimension & dim) const
{
	Changer dummy = mi.base.changeEnsureMath();
	Dimension dim0;
	{
		Changer script = mi.base.font.changeStyle(LM_ST_SCRIPTSCRIPT);
		cell(0).metrics(mi, dim0);
		// make sure that the dim is high enough for any character
		Dimension fontDim;
		math_font_max_dim(mi.base.font, fontDim.asc, fontDim.des);
		dim0 += fontDim;
	}

	cell(1).metrics(mi, dim);

	Dimension const dim_rad = mathedRule11RadicalDim(mi.base, dim);
	dim.asc = max(dim_rad.asc, dim0.asc + dim0.des + (dim.asc - dim.des)/2);
	dim.des = dim_rad.des;
	dim.wid += max(dim_rad.wid, dim0.width() + mi.base.mu(4));
}


void InsetMathRoot::draw(PainterInfo & pi, int x, int y) const
{
	Changer dummy = pi.base.changeEnsureMath();
	double const t = pi.base.solidLineThickness();
	Dimension const dim0 = cell(0).dimension(*pi.base.bv);
	Dimension const dim1 = cell(1).dimension(*pi.base.bv);
	Dimension const dim_rad = mathedRule11RadicalDim(pi.base, dim1);
	// the "exponent"
	{
		Changer script = pi.base.font.changeStyle(LM_ST_SCRIPTSCRIPT);
		// FIXME: ym is incorrect wrt TeX
		int const ym = round(y + (dim_rad.des - dim_rad.asc)/2.
		                     - dim0.descent() - t);
		cell(0).draw(pi, x, ym);
	}
	// the "base"
	int const x_rad = x + max(0, dim0.width() + pi.base.mu(4) - dim_rad.wid);
	int const x0 = mathedDrawRadical(pi, x_rad, y, dim1);
	cell(1).draw(pi, x0, y);
}


void InsetMathRoot::write(WriteStream & os) const
{
	MathEnsurer ensurer(os);
	os << "\\sqrt[" << cell(0) << "]{" << cell(1) << '}';
}


void InsetMathRoot::normalize(NormalStream & os) const
{
	os << "[root " << cell(0) << ' ' << cell(1) << ']';
}


bool InsetMathRoot::idxUpDown(Cursor & cur, bool up) const
{
	Cursor::idx_type const target = up ? 0 : 1;
	if (cur.idx() == target)
		return false;
	cur.idx() = target;
	cur.pos() = up ? cur.lastpos() : 0;
	return true;
}


void InsetMathRoot::maple(MapleStream & os) const
{
	os << '(' << cell(1) << ")^(1/(" << cell(0) <<"))";
}


void InsetMathRoot::mathematica(MathematicaStream & os) const
{
	os << '(' << cell(1) << ")^(1/(" << cell(0) <<"))";
}


void InsetMathRoot::octave(OctaveStream & os) const
{
	os << '(' << cell(1) << ")^(1/(" << cell(0) <<"))";
}


void InsetMathRoot::mathmlize(MathStream & os) const
{
	os << MTag("mroot") << cell(1) << cell(0) << ETag("mroot");
}


void InsetMathRoot::htmlize(HtmlStream & os) const
{
	os << MTag("span", "class='root'")
	   << MTag("sup") << cell(0) << ETag("sup")
	   << from_ascii("&radic;")
	   << MTag("span", "class='rootof'")	<< cell(1) << ETag("span")
		 << ETag("span");
}


void InsetMathRoot::validate(LaTeXFeatures & features) const
{
	if (features.runparams().math_flavor == OutputParams::MathAsHTML)
		features.addCSSSnippet(
			"span.rootof{border-top: thin solid black;}\n"
			"span.root sup{font-size: 75%;}");
	InsetMathNest::validate(features);
}

} // namespace lyx

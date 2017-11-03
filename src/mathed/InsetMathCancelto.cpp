/**
 * \file InsetMathCancelto.cpp
 * This file is part of LyX, the document processor.
 * Licence details can be found in the file COPYING.
 *
 * \author Uwe Stöhr
 *
 * Full author contact details are available in file CREDITS.
 */

#include <config.h>

#include "InsetMathCancelto.h"

#include "MathData.h"
#include "MathStream.h"

#include "Cursor.h"
#include "LaTeXFeatures.h"
#include "MetricsInfo.h"

#include "frontends/Painter.h"

#include <ostream>

using namespace std;

namespace lyx {


InsetMathCancelto::InsetMathCancelto(Buffer * buf)
	: InsetMathNest(buf, 2)
{}


Inset * InsetMathCancelto::clone() const
{
	return new InsetMathCancelto(*this);
}


void InsetMathCancelto::metrics(MetricsInfo & mi, Dimension & dim) const
{
	Changer dummy = mi.base.changeEnsureMath();
	cellsMetrics(mi);
	Dimension const & dim0 = cell(0).dimension(*mi.base.bv);
	Dimension const & dim1 = cell(1).dimension(*mi.base.bv);
	int const arrow_size = mi.base.mu(6);
	dim.asc = max(dim0.ascent() + 2, dim0.ascent() + dim1.ascent()) + 2
		+ arrow_size;
	dim.des = max(dim0.descent() - 2, dim1.descent()) + 2;
	dim.wid = dim0.width() + dim1.width() + 2 + arrow_size;
}


void InsetMathCancelto::draw(PainterInfo & pi, int x, int y) const
{
	Changer dummy = pi.base.changeEnsureMath();
	ColorCode const origcol = pi.base.font.color();
	int const arrow_size = pi.base.mu(6);

	// We first draw the text and then an arrow
	Dimension const & dim0 = cell(0).dimension(*pi.base.bv);
	cell(0).draw(pi, x, y);
	cell(1).draw(pi, x + dim0.wid + 1 + arrow_size, y - dim0.asc - arrow_size);

	//Dimension const dim = dimension(*pi.base.bv);

	// y3____ ___
  	//          /|
	// y2_     / |
	//        /
	//       /
	//      /
	//     /
	// y1 /    | |
	//    x1  x2 x3

	double const x2 = x + dim0.wid;
	double const x3 = x2 + arrow_size;
	double const x1 = x;
	double const y1 = y + dim0.des;
	double const y2 = y - dim0.asc;
	double const y3 = y2 - arrow_size;

	double const t = pi.base.solidLineThickness();
	// the main line
	pi.pain.lineDouble(x3, y3, x1, y1, origcol, t);
	// the arrow bars
	pi.pain.lineDouble(x3, y3, x2 + 2, y3, origcol, t);
	pi.pain.lineDouble(x3, y3, x3 - 2, y2 - 2, origcol, t);
}


void InsetMathCancelto::write(WriteStream & os) const
{
	MathEnsurer ensurer(os);
	os << "\\cancelto{" << cell(1) << "}{" << cell(0) << '}';
}


void InsetMathCancelto::normalize(NormalStream & os) const
{
	os << "[cancelto " << cell(1) << ' ' << cell(0) << ']';
}

bool InsetMathCancelto::idxUpDown(Cursor & cur, bool up) const
{
	Cursor::idx_type const target = up ? 1 : 0;
	if (cur.idx() == target)
		return false;
	cur.idx() = target;
	cur.pos() = up ? cur.lastpos() : 0;
	return true;
}

void InsetMathCancelto::infoize(odocstream & os) const
{
	os << "Cancelto";
}

void InsetMathCancelto::validate(LaTeXFeatures & features) const
{
	InsetMathNest::validate(features);
	if (features.runparams().isLaTeX())
		features.require("cancel");
	InsetMathNest::validate(features);
}


} // namespace lyx

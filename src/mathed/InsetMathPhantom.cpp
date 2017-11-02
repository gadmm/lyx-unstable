/**
 * \file InsetMathPhantom.cpp
 * This file is part of LyX, the document processor.
 * Licence details can be found in the file COPYING.
 *
 * \author Georg Baum
 * \author Guillaume Munch
 *
 * Full author contact details are available in file CREDITS.
 */

#include <config.h>

#include "InsetMathPhantom.h"

#include "LaTeXFeatures.h"
#include "MathStream.h"
#include "MathSupport.h"

#include "MetricsInfo.h"

#include "frontends/Painter.h"

#include <algorithm>
#include <ostream>

#include <cmath>

namespace lyx {


InsetMathPhantom::InsetMathPhantom(Buffer * buf, Kind k)
	: InsetMathNest(buf, 1), kind_(k)
{}


Inset * InsetMathPhantom::clone() const
{
	return new InsetMathPhantom(*this);
}


void InsetMathPhantom::metrics(MetricsInfo & mi, Dimension & dim) const
{
	Changer dummy = mi.base.changeEnsureMath();
	cell(0).metrics(mi, dim);
	int const arrow_size = mi.base.mu(3);
	dim.wid = std::max(dim.wid, arrow_size);
	if (kind_ != smashb)
		dim.asc = std::max(dim.asc, arrow_size);
}


void InsetMathPhantom::draw(PainterInfo & pi, int x, int y) const
{
	Changer dummy = pi.base.changeEnsureMath();
	int const arrow_size = pi.base.mu(1.5) * 2; //even number for grid alignment
	double const t = pi.base.solidLineThickness();
	double const dt = mathed_deco_thickness(pi.base);

	// We first draw the text and then an arrow
	{
		Changer dummy0 = visibleContents()
			? pi.base.font.changeColor(Color_special)
			: Changer();
		cell(0).draw(pi, x, y);
	}
	Dimension const dim = dimension(*pi.base.bv);

	Changer dummy0 = pi.base.font.changeColor(Color_added_space);
	double const xv = round(x + (dim.wid - arrow_size) / 2. - 0.5);
	double const yv = y - dim.asc;
	double const w = arrow_size - dt;
	if (kind_ == phantom || kind_ == vphantom)
		mathed_draw_deco(pi, xv, yv, w, dim.height() - dt,
		                 from_ascii("updownarrow"));
	double const yh = round(y - (dim.asc - dim.des + arrow_size) / 2. - 0.5);
	double const w2 = (dim.wid - t) / 2;
	if (kind_ == phantom || kind_ == hphantom ||
	    kind_ == mathllap || kind_ == mathrlap)
		mathed_draw_deco(pi, x, yh, dim.wid - dt - 1, w,
		                 from_ascii(kind_ == mathllap ? "xrightarrow" :
		                            kind_ == mathrlap ? "xleftarrow" :
		                            "xleftrightarrow"));
	if (kind_ == mathclap) {
		mathed_draw_deco(pi, x, yh, w2 - dt, w,
		                 from_ascii("xrightarrow"));
		mathed_draw_deco(pi, x + w2 + t, yh, w2 - dt, w,
		                 from_ascii("xleftarrow"));
	}
	if (kind_ == smash || kind_ == smasht)
		mathed_draw_deco(pi, xv, yv, w, dim.asc - dt,
		                 from_ascii("downarrow"));
	if (kind_ == smash || kind_ == smashb)
		mathed_draw_deco(pi, xv, y - 1, w, std::max(w, dim.des - dt + t),
		                 from_ascii("uparrow"));
}


void InsetMathPhantom::write(WriteStream & os) const
{
	MathEnsurer ensurer(os);
	if (os.fragile())
		os << "\\protect";
	switch (kind_) {
	case phantom:
		os << "\\phantom{";
		break;
	case vphantom:
		os << "\\vphantom{";
		break;
	case hphantom:
		os << "\\hphantom{";
		break;
	case smash:
		os << "\\smash{";
		break;
	case smasht:
		os << "\\smash[t]{";
		break;
	case smashb:
		os << "\\smash[b]{";
		break;
	case mathclap:
		os << "\\mathclap{";
		break;
	case mathllap:
		os << "\\mathllap{";
		break;
	case mathrlap:
		os << "\\mathrlap{";
		break;
	}
	os << cell(0) << '}';
}


void InsetMathPhantom::normalize(NormalStream & os) const
{
	switch (kind_) {
	case phantom:
		os << "[phantom ";
		break;
	case vphantom:
		os << "[vphantom ";
		break;
	case hphantom:
		os << "[hphantom ";
		break;
	case smash:
		os << "[smash ";
		break;
	case smasht:
		os << "[smasht ";
		break;
	case smashb:
		os << "[smashb ";
		break;
	case mathclap:
		os << "[mathclap ";
		break;
	case mathllap:
		os << "[mathllap ";
		break;
	case mathrlap:
		os << "[mathrlap ";
		break;
	}
	os << cell(0) << ']';
}


void InsetMathPhantom::infoize(odocstream & os) const
{
	switch (kind_) {
	case phantom:
		os << "Phantom";
		break;
	case vphantom:
		os << "Vphantom";
		break;
	case hphantom:
		os << "Hphantom";
		break;
	case smash:
		os << "Smash";
		break;
	case smasht:
		os << "Smashtop";
		break;
	case smashb:
		os << "Smashbottom";
		break;
	case mathllap:
		os << "Mathllap";
		break;
	case mathclap:
		os << "Mathclap";
		break;
	case mathrlap:
		os << "Mathrlap";
		break;
	}
}


void InsetMathPhantom::validate(LaTeXFeatures & features) const
{
	InsetMathNest::validate(features);
	switch (kind_) {
	case phantom:
	case vphantom:
	case hphantom:
	case smash:
		break;
	case smasht:
	case smashb:
		features.require("amsmath");
		break;
	case mathclap:
	case mathllap:
	case mathrlap:
		features.require("mathtools");
		break;
	}
}


bool InsetMathPhantom::visibleContents() const
{
	return kind_ == phantom || kind_ == vphantom || kind_ == hphantom;
}


} // namespace lyx

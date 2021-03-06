/**
 * \file ColorCache.cpp
 * This file is part of LyX, the document processor.
 * Licence details can be found in the file COPYING.
 *
 * \author John Levon
 *
 * Full author contact details are available in file CREDITS.
 */

#include <config.h>

#include "LyXRC.h"

#include "ColorCache.h"
#include "ColorSet.h"

#include <cmath>

namespace lyx {

namespace {

QPalette::ColorRole role(ColorCode col)
{
	switch (ColorCode(col)) {
	case Color_background:
	case Color_commentbg:
	case Color_greyedoutbg:
	case Color_mathbg:
	case Color_graphicsbg:
	case Color_mathmacrobg:
	case Color_mathcorners:
		return QPalette::Base;
		break;

	case Color_foreground:
	case Color_cursor:
	case Color_preview:
	case Color_tabularline:
	case Color_previewframe:
		return QPalette::Text;
		break;

	case Color_selection:
		return QPalette::Highlight;
		break;
	case Color_selectiontext:
		return QPalette::HighlightedText;
		break;
	case Color_urllabel:
	case Color_urltext:
		return QPalette::Link;
	default:
		return QPalette::NoRole;
	}
}


QColor invert(QColor col)
{
	static std::vector<int> gamma = [](){
		std::vector<int> gamma(256);
		double const gamma_val = 2.4;
		for (int i = 0; i <= 255; ++i)
			gamma[i] = std::round(255 * std::pow(double(255 - i)/255,
			                                     1/gamma_val));
		return gamma;
	}();
	return {gamma[col.red()], gamma[col.green()], gamma[col.blue()]};
}


} // namespace


void ColorCache::init()
{
	for (int col = 0; col <= Color_ignore; ++col) {
		lcolors_[col] = QColor(lcolor.getX11Name(ColorCode(col)).c_str());
	}

	initialized_ = true;
}


/// get the given color
QColor ColorCache::get(Color const & color) const
{
	return get(color, lyxrc.use_system_colors);
}


QColor ColorCache::get(Color const & color, bool syscolors) const
{
	QColor const col = getPlain(color, syscolors);
	return (lyxrc.invert_colors != invert_debug) ? invert(col) : col;
}


QColor ColorCache::getPlain(Color const & color, bool syscolors) const
{
	if (!initialized_)
		const_cast<ColorCache *>(this)->init();
	if (color <= Color_ignore && color.mergeColor == Color_ignore) {
		QPalette::ColorRole const cr = role(color.baseColor);
		if (syscolors && cr != QPalette::NoRole) {
			static QColor const white = Qt::white;
			QColor const c = pal_.brush(QPalette::Active, cr).color();
			if (cr == QPalette::Base && c == white)
				return lcolors_[color.baseColor];
			else
				return c;
		} else
			return lcolors_[color.baseColor];
	}
	if (color.mergeColor != Color_ignore) {
		// FIXME: This would ideally be done in the Color class, but
		// that means that we'd have to use the Qt code in the core.
		QColor base_color = getPlain(color.baseColor, syscolors).toRgb();
		QColor merge_color = getPlain(color.mergeColor, syscolors).toRgb();
		return QColor(
			(base_color.red() + merge_color.red()) / 2,
			(base_color.green() + merge_color.green()) / 2,
			(base_color.blue() + merge_color.blue()) / 2);
	}
	// used by branches
	return QColor(lcolor.getX11Name(color.baseColor).c_str());
}


bool ColorCache::isSystem(ColorCode const color) const
{
	QPalette::ColorRole const cr = role(color);
	if (cr == QPalette::Base) {
		static QColor const white = Qt::white;
		return pal_.brush(QPalette::Active, cr).color() != white;
	} else
		return cr != QPalette::NoRole;
}


QColor rgb2qcolor(RGBColor const & rgb)
{
	return QColor(rgb.r, rgb.g, rgb.b);
}


} // namespace lyx

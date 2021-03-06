// -*- C++ -*-
/**
 * \file MathSupport.h
 * This file is part of LyX, the document processor.
 * Licence details can be found in the file COPYING.
 *
 * \author Alejandro Aguilar Sierra
 * \author André Pönitz
 *
 * Full author contact details are available in file CREDITS.
 */

#ifndef MATH_SUPPORT_H
#define MATH_SUPPORT_H

#include "MathParser_flags.h"
#include "support/strfwd.h"

#include "Color.h"

namespace lyx {

class MetricsBase;
class MetricsInfo;
class PainterInfo;
class FontInfo;
class Dimension;
class MathData;
class MathAtom;
class InsetMath;
class latexkeys;
class LaTeXFeatures;


int mathed_font_em(FontInfo const &);

int mathed_font_x_height(FontInfo const & font);

int mathed_mu(FontInfo const & font, double mu);

int mathed_thinmuskip(FontInfo const & font);

int mathed_medmuskip(FontInfo const & font);

int mathed_thickmuskip(FontInfo const & font);

int mathed_char_width(FontInfo const &, char_type c);

int mathed_char_kerning(FontInfo const &, char_type c);

// thickness added to decorations because of the zoom value
double mathed_deco_thickness(MetricsBase const & mb);

// add the thickness of the decorations. num_w and num_h are a multiplier
// in case there are several decorations, horizontally or vertically
void mathed_deco_metrics(MetricsBase const & mb, Dimension & dim,
                         int num_w = 1, int num_h = 1);

void mathed_draw_deco(PainterInfo const & pi, double x, double y, double w,
                      double h, docstring const & name);

// Set the dimension for the radical according to rule 11 of Appendix G
// The width is arbitrary.
// dim_nucl is the dimension of the nucleus.
Dimension mathedRule11RadicalDim(MetricsBase & mb, Dimension const & dim_nucl);

// Draw radical around cell of dimension dim_nucl
// Returns the x at which this cell should be drawn
int mathedDrawRadical(PainterInfo & pi, double x, double y,
                      Dimension const & dim_nucl);

void mathed_draw_marker(PainterInfo const & pi, int x, int y, int w, int h,
                        Color col);

void mathed_string_dim(FontInfo const & font,
		       docstring const & s,
		       Dimension & dim);

int mathed_string_width(FontInfo const &, docstring const & s);

void mathedSymbolDim(MetricsBase & mb, Dimension & dim, latexkeys const * sym);

void mathedSymbolDraw(PainterInfo & pi, int x, int y, latexkeys const * sym);

void metricsStrRedBlack(MetricsInfo & mi, Dimension & dim, docstring const & s);

void drawStrRed(PainterInfo & pi, int x, int y, docstring const & s);
void drawStrBlack(PainterInfo & pi, int x, int y, docstring const & s);

void math_font_max_dim(FontInfo const &, int & asc, int & desc);

void augmentFont(FontInfo & f, std::string const & cmd);

bool isFontName(std::string const & name);

bool isMathFont(std::string const & name);

bool isTextFont(std::string const & name);

bool isAlphaSymbol(MathAtom const & at);

// converts single cell to string
docstring asString(MathData const & ar);
// converts single inset to string
docstring asString(InsetMath const &);
docstring asString(MathAtom const &);
// converts string to single cell
void asArray(docstring const &, MathData &, Parse::flags f = Parse::NORMAL);

// simulate axis height (font dimension sigma 22)
int axis_height(MetricsBase & mb);

void validate_math_word(LaTeXFeatures & features, docstring const & word);

} // namespace lyx

#endif

// -*- C++ -*-
/**
 * \file GuiPainter.h
 * This file is part of LyX, the document processor.
 * Licence details can be found in the file COPYING.
 *
 * \author John Levon
 * \author Abdelrazak Younes
 *
 * Full author contact details are available in file CREDITS.
 */

#ifndef GUIPAINTER_H
#define GUIPAINTER_H

#include "Color.h"

#include "frontends/Painter.h"

#include <QPainter>
#include <QPainterPath>
#include <stack>

class QString;

namespace lyx {

class FontInfo;

namespace frontend {

/**
 * GuiPainter - a painter implementation for Qt
 */
class GuiPainter : public QPainter, public Painter {
public:
	GuiPainter(QPaintDevice *, double pixel_ratio);
	virtual ~GuiPainter();

	/// This painter paints
	virtual bool isNull() const { return false; }

	// set the dash offset to make it absolute wrt the QPointF, for
	// line_onoffdash_absolute
	void setAbsoluteOffset(QPointF const & p, double lw);

	/// draw a line from point to point
	void line(int x1, int y1, int x2, int y2, Color, line_style ls = line_solid,
	          int lw = thin_line) override;
	void lineDouble(double x1, double y1, double x2, double y2, Color,
	                double line_width = thin_line,
	                line_style ls = line_solid) override;

	/**
	 * lines -  draw a set of lines
	 * @param xp array of points' x co-ords
	 * @param yp array of points' y co-ords
	 * @param np size of the points array
	 */
	void lines(int const * xp, int const * yp, int np, Color,
	           fill_style = fill_none, line_style = line_solid,
	           int line_width = thin_line) override;
	void linesDouble(double const * xp, double const * yp, int np, Color,
	                 double line_width = thin_line, fill_style = fill_none,
	                 line_style = line_solid) override;
	void linesDouble(std::vector<QPointF> points, Color col, double lw,
	                 fill_style fs, line_style ls);


	/**
	 * path -  draw a path with bezier curves
	 * @param xp array of points' x co-ords
	 * @param yp array of points' y co-ords
	 * @param c1x array of first control points' x co-ords
	 * @param c1y array of first control points' y co-ords
	 * @param c2x array of second control points' x co-ords
	 * @param c2y array of second control points' y co-ords
	 * @param np size of the points array
	 */
	virtual void path(int const * xp, int const * yp,
		int const * c1x, int const * c1y,
		int const * c2x, int const * c2y,
		int np, Color,
		fill_style = fill_none, line_style = line_solid,
		int line_width = thin_line);

	/// draw a rectangle
	void rectangle(int x, int y,
	               int w, int h,
	               Color,
	               line_style ls = line_solid,
	               int lw = thin_line) override;
	void rectangleDouble(double x, double y,
	                     double w, double h,
	                     Color,
	                     double lw = thin_line,
	                     line_style ls = line_solid) override;


	/// draw a filled rectangle
	virtual void fillRectangle(
		int x, int y,
		int w, int h,
		Color);

	/// draw an arc
	virtual void arc(
		int x, int y,
		unsigned int w, unsigned int h,
		int a1, int a2,
		Color);

	/// draw a pixel
	virtual void point(int x, int y, Color);

	/// draw an image from the image cache
	virtual void image(int x, int y, int w, int h,
		lyx::graphics::Image const & image);

	/// draw a string at position x, y (y is the baseline).
	virtual void text(int x, int y, docstring const & str, FontInfo const & f);

	/// draw a char at position x, y (y is the baseline)
	virtual void text(int x, int y, char_type c, FontInfo const & f);

	/** draw a string at position x, y (y is the baseline). The
	 * text direction is enforced by the \c Font.
	 */
	virtual void text(int x, int y, docstring const & str, Font const & f,
                      double wordspacing, double textwidth);

	/** draw a string at position x, y (y is the baseline), but
	 * make sure that the part between \c from and \c to is in
	 * \c other color. The text direction is enforced by the \c Font.
	 */
	virtual void text(int x, int y, docstring const & str, Font const & f,
	                  Color other, size_type from, size_type to,
                      double wordspacing, double textwidth);

	///
	virtual void textDecoration(FontInfo const & f, int x, int y, int width);

	/// draw a string and enclose it inside a button frame
	virtual void buttonText(int x, int baseline, docstring const & s,
	                        FontInfo const & font, Color back, Color frame,
	                        int offset, double lw = thin_line);

	/// start monochrome painting mode, i.e. map every color into [min,max]
	virtual void enterMonochromeMode(Color const & min,
		Color const & max);
	/// leave monochrome painting mode
	virtual void leaveMonochromeMode();

	/**
	 * Draw a string and enclose it inside a rectangle. If
	 * back color is specified, the background is cleared with
	 * the given color. If frame is specified, a thin frame is drawn
	 * around the text with the given color.
	 */
	virtual void rectText(int x, int baseline, docstring const & str,
		FontInfo const & font, Color back, Color frame);

	/// draw a character of a preedit string for cjk support.
	virtual int preeditText(int x, int y,
		char_type c, FontInfo const & f, preedit_style style);

	void wavyHorizontalLine(int x, int y, int width, ColorCode col);

private:
	/// check the font, and if set, draw an underline
	void underline(FontInfo const & f,
	               int x, int y, int width, line_style ls = line_solid);

	/// check the font, and if set, draw an dashed underline
	void dashedUnderline(FontInfo const & f,
		int x, int y, int width);

	/// check the font, and if set, draw an strike-through line
	void strikeoutLine(FontInfo const & f,
		int x, int y, int width);

	/// check the font, and if set, draw cross-through lines
	void crossoutLines(FontInfo const & f,
		int x, int y, int width);

	/// check the font, and if set, draw double underline
	void doubleUnderline(FontInfo const & f,
		int x, int y, int width);

	/// set pen parameters
	void setQPainterPen(QColor const & col, line_style ls = line_solid,
	                    double lw = thin_line);

	// Direction for painting text
	enum Direction { LtR, RtL, Auto };

	// Helper for text() method
	void do_drawText(int x, int y, QString str, Direction dir, FontInfo const & f, QFont ff);

	// Real text() method
	void text(int x, int y, docstring const & s,
              FontInfo const & f, Direction const dir,
              double const wordspacing, double tw);

	QColor current_color_;
	Painter::line_style current_ls_;
	double current_lw_;
	///
	bool const use_pixmap_cache_;
	///
	std::stack<QColor> monochrome_min_;
	///
	std::stack<QColor> monochrome_max_;
	/// convert into Qt color, possibly applying the monochrome mode
	QColor computeColor(Color col);
	/// possibly apply monochrome mode
	QColor filterColor(QColor const & col);
	///
	QString generateStringSignature(QString const & str, FontInfo const & f,
	                                double wordspacing);
};

} // namespace frontend
} // namespace lyx

#endif // GUIPAINTER_H

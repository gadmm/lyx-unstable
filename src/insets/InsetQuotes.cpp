/**
 * \file InsetQuotes.cpp
 * This file is part of LyX, the document processor.
 * Licence details can be found in the file COPYING.
 *
 * \author Jean-Marc Lasgouttes
 *
 * Full author contact details are available in file CREDITS.
 */

#include <config.h>

#include "InsetQuotes.h"

#include "Buffer.h"
#include "BufferParams.h"
#include "BufferView.h"
#include "Dimension.h"
#include "Font.h"
#include "Language.h"
#include "LaTeXFeatures.h"
#include "Lexer.h"
#include "LyXRC.h"
#include "MetricsInfo.h"
#include "OutputParams.h"
#include "output_xhtml.h"
#include "Paragraph.h"
#include "ParIterator.h"
#include "texstream.h"

#include "frontends/FontMetrics.h"
#include "frontends/Painter.h"

#include "support/debug.h"
#include "support/docstring.h"
#include "support/docstream.h"
#include "support/lstrings.h"

using namespace std;
using namespace lyx::support;

namespace lyx {

namespace {

/* codes used to read/write quotes to LyX files
 * e    ``english''
 * s    ''swedish''
 * g    ,,german``
 * p    ,,polish''
 * f    <<french>>
 * a    >>danish<<
 */

char const * const language_char = "esgpfa";
char const * const side_char = "lr" ;
char const * const times_char = "sd";

// List of known quote chars
char const * const quote_char = ",'`<>";

// Unicode characters needed by each quote type
char_type const display_quote_char[2][5] = {
	{ 0x201a, 0x2019, 0x2018, 0x2039, 0x203a},
	{ 0x201e, 0x201d, 0x201c, 0x00ab, 0x00bb}
};

// Index of chars used for the quote. Index is [side, language]
int quote_index[2][6] = {
	{ 2, 1, 0, 0, 3, 4 },    // "'',,<>"
	{ 1, 1, 2, 1, 4, 3 }     // "`'`'><"
};

// Corresponding LaTeX code, for double and single quotes.
char const * const latex_quote_t1[2][5] = {
	{ "\\quotesinglbase",  "'", "`",
    "\\guilsinglleft", "\\guilsinglright" },
  { ",,", "''", "``", "<<", ">>" }
};

char const * const latex_quote_ot1[2][5] = {
	{ "\\quotesinglbase",  "'", "`",
    "\\guilsinglleft", "\\guilsinglright" },
  { "\\quotedblbase", "''", "``",
    "\\guillemotleft", "\\guillemotright" }
};

char const * const latex_quote_noligatures[2][5] = {
	{ "\\quotesinglbase",  "\\textquoteleft", "\\textquoteright",
    "\\guilsinglleft", "\\guilsinglright" },
  { "\\quotedblbase", "\\textquotedblleft", "\\textquotedblright",
    "\\guillemotleft", "\\guillemotright" }
};

char const * const latex_quote_babel[2][5] = {
	{ "\\glq",  "'", "`", "\\flq", "\\frq" },
  { "\\glqq", "''", "``", "\\flqq", "\\frqq" }
};

char const * const html_quote[2][5] = {
	{ "&sbquo;",  "&rsquo;", "&lsquo;",
	  "&lsaquo;", "&rsaquo;" },
  { "&bdquo;", "&rdquo;", "&ldquo;", "&laquo;", "&raquo;" }
};

} // namespace anon


InsetQuotes::InsetQuotes(Buffer * buf, string const & str) : Inset(buf)
{
	parseString(str);
}

InsetQuotes::InsetQuotes(Buffer * buf, char_type c, QuoteTimes t)
	: Inset(buf), times_(t), pass_thru_(false)
{
	if (buf) {
		language_ = buf->params().quotes_language;
		fontenc_ = (buf->params().fontenc == "global")
			? lyxrc.fontenc : buf->params().fontenc;
	} else {
		language_ = EnglishQuotes;
		fontenc_ = lyxrc.fontenc;
	}

	setSide(c);
}


docstring InsetQuotes::layoutName() const
{
	return from_ascii("Quotes");
}


void InsetQuotes::setSide(char_type c)
{
	// Decide whether left or right
	switch (c) {
	case ' ':
	case '(':
	case '[':
		side_ = LeftQuote;   // left quote
		break;
	default:
		side_ = RightQuote;  // right quote
	}
}


void InsetQuotes::parseString(string const & s)
{
	string str = s;
	if (str.length() != 3) {
		lyxerr << "ERROR (InsetQuotes::InsetQuotes):"
			" bad string length." << endl;
		str = "eld";
	}

	int i;

	for (i = 0; i < 6; ++i) {
		if (str[0] == language_char[i]) {
			language_ = QuoteLanguage(i);
			break;
		}
	}
	if (i >= 6) {
		lyxerr << "ERROR (InsetQuotes::InsetQuotes):"
			" bad language specification." << endl;
		language_ = EnglishQuotes;
	}

	for (i = 0; i < 2; ++i) {
		if (str[1] == side_char[i]) {
			side_ = QuoteSide(i);
			break;
		}
	}
	if (i >= 2) {
		lyxerr << "ERROR (InsetQuotes::InsetQuotes):"
			" bad side specification." << endl;
		side_ = LeftQuote;
	}

	for (i = 0; i < 2; ++i) {
		if (str[2] == times_char[i]) {
			times_ = QuoteTimes(i);
			break;
		}
	}
	if (i >= 2) {
		lyxerr << "ERROR (InsetQuotes::InsetQuotes):"
			" bad times specification." << endl;
		times_ = DoubleQuotes;
	}
}


docstring InsetQuotes::displayString() const
{
	// In PassThru, we use straight quotes
	if (pass_thru_)
		return (times_ == DoubleQuotes) ? from_ascii("\"") : from_ascii("'");

	int const index = quote_index[side_][language_];
	docstring retdisp = docstring(1, display_quote_char[times_][index]);

	// in French, thin spaces are added inside double guillemets
	// FIXME: this should be done by a separate quote type.
	if (prefixIs(context_lang_, "fr")
	    && times_ == DoubleQuotes && language_ == FrenchQuotes) {
		// THIN SPACE (U+2009)
		char_type const thin_space = 0x2009;
		if (side_ == LeftQuote)
			retdisp += thin_space;
		else
			retdisp = thin_space + retdisp;
	}

	return retdisp;
}


void InsetQuotes::metrics(MetricsInfo & mi, Dimension & dim) const
{
	FontInfo & font = mi.base.font;
	frontend::FontMetrics const & fm = theFontMetrics(font);
	dim.asc = fm.maxAscent();
	dim.des = fm.maxDescent();
	dim.wid = fm.width(displayString());
}


void InsetQuotes::draw(PainterInfo & pi, int x, int y) const
{
	FontInfo font = pi.base.font;
	font.setPaintColor(pi.textColor(font.realColor()));
	pi.pain.text(x, y, displayString(), font);
}


void InsetQuotes::write(ostream & os) const
{
	string text;
	text += language_char[language_];
	text += side_char[side_];
	text += times_char[times_];
	os << "Quotes " << text;
}


void InsetQuotes::read(Lexer & lex)
{
	lex.setContext("InsetQuotes::read");
	lex.next();
	parseString(lex.getString());
	lex >> "\\end_inset";
}


void InsetQuotes::latex(otexstream & os, OutputParams const & runparams) const
{
	const int quoteind = quote_index[side_][language_];
	string qstr;

	// In pass-thru context, we output plain quotes
	if (runparams.pass_thru)
		qstr = (times_ == DoubleQuotes) ? "\"" : "'";
	else if (language_ == FrenchQuotes && times_ == DoubleQuotes
	    && prefixIs(runparams.local_font->language()->code(), "fr")
	    && !runparams.use_polyglossia) {
		// Specific guillemets of French babel
		// including correct French spacing
		if (side_ == LeftQuote)
			qstr = "\\og";
		else
			// the preceding space is important here
			// FIXME: really? (doesn't make a difference
			//        with recent babel-french)
			qstr = " \\fg";
	} else if (fontenc_ == "T1"
		   && !runparams.local_font->language()->internalFontEncoding()
		   && !runparams.use_polyglossia) {
		// Quotation marks for T1 font encoding
		// (using ligatures)
		qstr = latex_quote_t1[times_][quoteind];
	} else if (runparams.local_font->language()->internalFontEncoding()) {
		// Quotation marks for internal font encodings
		// (ligatures not featured)
		qstr = latex_quote_noligatures[times_][quoteind];
#ifdef DO_USE_DEFAULT_LANGUAGE
	} else if (doclang == "default") {
#else
	} else if (!runparams.use_babel || runparams.isFullUnicode()) {
#endif
		// Standard quotation mark macros
		// These are also used by polyglossia
		// and babel without fontenc (XeTeX/LuaTeX)
		qstr = latex_quote_ot1[times_][quoteind];
	} else {
		// Babel shorthand quotation marks (for T1/OT1)
		qstr = latex_quote_babel[times_][quoteind];
	}

	if (!runparams.pass_thru) {
		// Always guard against unfortunate ligatures (!` ?` `` '' ,, << >>)
		char_type const lastchar = os.lastChar();
		if (prefixIs(qstr, "`")) {
			if (lastchar == '!' || lastchar == '?')
				qstr.insert(0, "{}");
		}
		if (qstr[0] == lastchar)
			qstr.insert(0, "{}");
	}

	os << from_ascii(qstr);

	// FIXME Check if the space in " \\fg" is really needed (see above)
	if (prefixIs(qstr, "\\") || prefixIs(qstr, " \\"))
		// properly terminate the command depending on the context
		os << termcmd;
}


int InsetQuotes::plaintext(odocstringstream & os, 
        OutputParams const &, size_t) const
{
	docstring const str = displayString();
	os << str;
	return str.size();
}


docstring InsetQuotes::getQuoteEntity() const {
	const int quoteind = quote_index[side_][language_];
	docstring res = from_ascii(html_quote[times_][quoteind]);
	// in French, thin spaces are added inside double guillemets
	// FIXME: this should be done by a separate quote type.
	if (prefixIs(context_lang_, "fr")
	    && times_ == DoubleQuotes && language_ == FrenchQuotes) {
		// THIN SPACE (U+2009)
		docstring const thin_space = from_ascii("&#x2009;");
		if (side_ == LeftQuote)
			res += thin_space;
		else
			res = thin_space + res;
	}
	return res;
}


int InsetQuotes::docbook(odocstream & os, OutputParams const &) const
{
	os << getQuoteEntity();
	return 0;
}


docstring InsetQuotes::xhtml(XHTMLStream & xs, OutputParams const &) const
{
	xs << XHTMLStream::ESCAPE_NONE << getQuoteEntity();
	return docstring();
}


void InsetQuotes::toString(odocstream & os) const
{
	os << displayString();
}


void InsetQuotes::forOutliner(docstring & os, size_t const, bool const) const
{
	os += displayString();
}


void InsetQuotes::updateBuffer(ParIterator const & it, UpdateType /* utype*/)
{
	BufferParams const & bp = buffer().masterBuffer()->params();
	pass_thru_ = it.paragraph().isPassThru();
	context_lang_ = it.paragraph().getFontSettings(bp, it.pos()).language()->code();
}


void InsetQuotes::validate(LaTeXFeatures & features) const
{
	char type = quote_char[quote_index[side_][language_]];

#ifdef DO_USE_DEFAULT_LANGUAGE
	if (features.bufferParams().language->lang() == "default"
#else
	if (!features.useBabel()
#endif
	    && !features.usePolyglossia() && fontenc_ != "T1") {
		if (times_ == SingleQuotes)
			switch (type) {
			case ',': features.require("quotesinglbase"); break;
			case '<': features.require("guilsinglleft");  break;
			case '>': features.require("guilsinglright"); break;
			default: break;
			}
		else
			switch (type) {
			case ',': features.require("quotedblbase");   break;
			case '<': features.require("guillemotleft");  break;
			case '>': features.require("guillemotright"); break;
			default: break;
			}
	}
}

} // namespace lyx

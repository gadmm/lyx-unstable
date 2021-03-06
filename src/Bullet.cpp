/**
 * \file Bullet.cpp
 * This file is part of LyX, the document processor.
 * Licence details can be found in the file COPYING.
 *
 * \author Lars Gullik Bjønnes
 * \author Allan Rae
 *
 * Full author contact details are available in file CREDITS.
 */

/* Completes the implementation of the Bullet class
 * It defines the various LaTeX commands etc. required to
 * generate the bullets in the bullet-panel's.
 */

#include <config.h>

#include "Bullet.h"

#include "support/lassert.h"

using namespace std;

namespace lyx {


/** The four LaTeX itemize environment default bullets
 */
extern
Bullet const ITEMIZE_DEFAULTS[4] = { Bullet(0, 8),//"\\(\\bullet\\)"
				     Bullet(0, 0),//"\\normalfont\\bfseries{--}"
				     Bullet(0, 6),//"\\(\\ast\\)"
				     Bullet(0, 10) };//"\\(\\cdot\\)"

// will need these later if still using full text as below
// \usepackage{latexsym,pifont,amssymb}
// and wasysym when that panel is created


Bullet::Bullet(int f, int c, int s)
	: font(f), character(c), size(s), user_text(0)
{
	if (f < MIN || f >= FONTMAX)
		font = MIN;
	if (c < MIN || c >= CHARMAX)
		character = MIN;
	if (s < MIN || s >= SIZEMAX)
		size = MIN;
	generateText();
	testInvariant();
}



Bullet::Bullet(docstring const & t)
	: font(MIN), character(MIN), size(MIN), user_text(1), text(t)
{
	testInvariant();
}


void Bullet::setCharacter(int c)
{
	if (c < MIN || c >= CHARMAX)
		character = MIN;
	else
		character = c;
	user_text = 0;
	testInvariant();
}


void Bullet::setFont(int f)
{
	if (f < MIN || f >= FONTMAX)
		font = MIN;
	else
		font = f;
	user_text = 0;
	testInvariant();
}


void Bullet::setSize(int s)
{
	if (s < MIN || s >= SIZEMAX)
		size = MIN;
	else
		size = s;
	user_text = 0;
	testInvariant();
}


void Bullet::setText(docstring const & t)
{
	font = character = size = MIN;
	user_text = 1;
	text = t;
	testInvariant();
}


int Bullet::getCharacter() const
{
	return character;
}


int Bullet::getFont() const
{
	return font;
}


int Bullet::getSize() const
{
	return size;
}


Bullet & Bullet::operator=(Bullet const & b)
{
	b.testInvariant();
	font = b.font;
	character = b.character;
	size = b.size;
	user_text = b.user_text;
	text = b.text;
	this->testInvariant();
	return *this;
}


docstring const & Bullet::getText() const
{
	if (user_text == 0)
		generateText();
	return text;
}


bool operator==(const Bullet & b1, const Bullet & b2)
{
	bool result = false;

	if (b1.user_text && b2.user_text) {
		/* both have valid text */
		if (b1.text == b2.text)
			result = true;
	} else if (b1.character == b2.character && b1.font == b2.font &&
			 b1.size == b2.size) {
		result = true;
	}
	return result;
}


/*--------------------Private Member Functions-------------------*/


void Bullet::generateText() const
{
	// Assumption:
	// user hasn't defined their own text and/or I haven't generated
	// the text for the current font/character settings yet
	// thus the calling member function should say:
	//    if (user_text == 0) {
	//       generateText();
	//    }
	// Since a function call is more expensive than a conditional
	// this is more efficient. Besides this function is internal to
	// the class so it's only the class author that has access --
	// external users thus can't make mistakes.

	if ((font >= 0) && (character >= 0)) {
		text = bulletEntry(font, character);
		if (size >= 0)
			text = bulletSize(size) + text;
		user_text = -1;
		// text is now defined and doesn't need to be recalculated
		// unless font/character or text is modified
	}
}


docstring const Bullet::bulletSize(int s)
{
	// use a parameter rather than hard code `size' in here
	// in case some future function may want to retrieve
	// an arbitrary entry.
	// See additional comments in bulletEntry() below.

	static char const * BulletSize[SIZEMAX] = {
		"\\tiny",  "\\scriptsize", "\\footnotesize", "\\small", "\\normalsize",
		"\\large", "\\Large",      "\\LARGE",        "\\huge",  "\\Huge"
	};

	return from_ascii(BulletSize[s]);
}


docstring const Bullet::bulletEntry(int f, int c)
{
	// Despite how this may at first appear the static local variables
	// are only initialized once..
	// This is a work-around to avoid the "Static Initialization Problem"
	// and should work for all compilers. See "C++ FAQs" by Cline and Lomow,
	// Addison-Wesley, 1994, FAQ-180 pp169-171 for an explanation.
	// Doing things this way also makes it possible to generate `text' at
	// the time of construction.  It also encapsulates the conversion
	// of font, character and size entries to text.

	// The single 2-dim array had to be changed to multiple 1-dim arrays
	// to get around a compiler bug in an earler version of gcc (< 2.7.2.1)
	// static string const BulletPanels[FONTMAX][CHARMAX] = {
	static char const * BulletPanel0[CHARMAX] = {
		/* standard */
		"\\normalfont\\bfseries{--}", "\\(\\vdash\\)",
		"\\(\\dashv\\)", "\\(\\flat\\)", "\\(\\natural\\)",
		"\\(\\sharp\\)", "\\(\\ast\\)", "\\(\\star\\)",
		"\\(\\bullet\\)", "\\(\\circ\\)", "\\(\\cdot\\)",
		"\\(\\dagger\\)", "\\(\\bigtriangleup\\)",
		"\\(\\bigtriangledown\\)", "\\(\\triangleleft\\)",
		"\\(\\triangleright\\)", "\\(\\lhd\\)", "\\(\\rhd\\)",
		"\\(\\oplus\\)", "\\(\\ominus\\)", "\\(\\otimes\\)",
		"\\(\\oslash\\)", "\\(\\odot\\)", "\\(\\spadesuit\\)",
		"\\(\\diamond\\)", "\\(\\Diamond\\)", "\\(\\Box\\)",
		"\\(\\diamondsuit\\)", "\\(\\heartsuit\\)",
		"\\(\\clubsuit\\)", "\\(\\rightarrow\\)", "\\(\\leadsto\\)",
		"\\(\\rightharpoonup\\)", "\\(\\rightharpoondown\\)",
		"\\(\\Rightarrow\\)", "\\(\\succ\\)"
	};
	static char const * BulletPanel1[CHARMAX] = {
		/* amssymb */
		"\\(\\Rrightarrow\\)", "\\(\\rightarrowtail\\)",
		"\\(\\twoheadrightarrow\\)", "\\(\\rightsquigarrow\\)",
		"\\(\\looparrowright\\)", "\\(\\multimap\\)",
		"\\(\\boxtimes\\)", "\\(\\boxplus\\)", "\\(\\boxminus\\)",
		"\\(\\boxdot\\)", "\\(\\divideontimes\\)", "\\(\\Vvdash\\)",
		"\\(\\lessdot\\)", "\\(\\gtrdot\\)", "\\(\\maltese\\)",
		"\\(\\bigstar\\)", "\\(\\checkmark\\)", "\\(\\Vdash\\)",
		"\\(\\backsim\\)", "\\(\\thicksim\\)",
		"\\(\\centerdot\\)", "\\(\\circleddash\\)",
		"\\(\\circledast\\)", "\\(\\circledcirc\\)",
		"\\(\\vartriangleleft\\)", "\\(\\vartriangleright\\)",
		"\\(\\vartriangle\\)", "\\(\\triangledown\\)",
		"\\(\\lozenge\\)", "\\(\\square\\)", "\\(\\blacktriangleleft\\)",
		"\\(\\blacktriangleright\\)", "\\(\\blacktriangle\\)",
		"\\(\\blacktriangledown\\)", "\\(\\blacklozenge\\)",
		"\\(\\blacksquare\\)"
	};
	static char const * BulletPanel2[CHARMAX] = {
		/* psnfss1 */
		"\\ding{108}", "\\ding{109}",
		"\\ding{119}", "\\Pisymbol{psy}{197}",
		"\\Pisymbol{psy}{196}", "\\Pisymbol{psy}{183}",
		"\\ding{71}", "\\ding{70}",
		"\\ding{118}", "\\ding{117}",
		"\\Pisymbol{psy}{224}", "\\Pisymbol{psy}{215}",
		"\\ding{111}", "\\ding{112}",
		"\\ding{113}", "\\ding{114}",
		"\\Pisymbol{psy}{68}", "\\Pisymbol{psy}{209}",
		"\\ding{120}", "\\ding{121}",
		"\\ding{122}", "\\ding{110}",
		"\\ding{115}", "\\ding{116}",
		"\\Pisymbol{psy}{42}", "\\ding{67}",
		"\\ding{66}", "\\ding{82}",
		"\\ding{81}", "\\ding{228}",
		"\\ding{162}", "\\ding{163}",
		"\\ding{166}", "\\ding{167}",
		"\\ding{226}", "\\ding{227}"
	};
	static char const * BulletPanel3[CHARMAX] = {
		/* psnfss2 */
		"\\ding{37}", "\\ding{38}",
		"\\ding{34}", "\\ding{36}",
		"\\ding{39}", "\\ding{40}",
		"\\ding{41}", "\\ding{42}",
		"\\ding{43}", "\\ding{44}",
		"\\ding{45}", "\\ding{47}",
		"\\ding{53}", "\\ding{54}",
		"\\ding{59}", "\\ding{57}",
		"\\ding{62}", "\\ding{61}",
		"\\ding{55}", "\\ding{56}",
		"\\ding{58}", "\\ding{60}",
		"\\ding{63}", "\\ding{64}",
		"\\ding{51}", "\\ding{52}",
		"\\Pisymbol{psy}{170}", "\\Pisymbol{psy}{167}",
		"\\Pisymbol{psy}{168}", "\\Pisymbol{psy}{169}",
		"\\ding{164}", "\\ding{165}",
		"\\ding{171}", "\\ding{168}",
		"\\ding{169}", "\\ding{170}"
	};
	static char const * BulletPanel4[CHARMAX] = {
		/* psnfss3 */
		"\\ding{65}", "\\ding{76}",
		"\\ding{75}", "\\ding{72}",
		"\\ding{80}", "\\ding{74}",
		"\\ding{78}", "\\ding{77}",
		"\\ding{79}", "\\ding{85}",
		"\\ding{90}", "\\ding{98}",
		"\\ding{83}", "\\ding{84}",
		"\\ding{86}", "\\ding{87}",
		"\\ding{88}", "\\ding{89}",
		"\\ding{92}", "\\ding{91}",
		"\\ding{93}", "\\ding{105}",
		"\\ding{94}", "\\ding{99}",
		"\\ding{103}", "\\ding{104}",
		"\\ding{106}", "\\ding{107}",
		"\\ding{68}", "\\ding{69}",
		"\\ding{100}", "\\ding{101}",
		"\\ding{102}", "\\ding{96}",
		"\\ding{95}", "\\ding{97}"
	};
	static char const * BulletPanel5[CHARMAX] = {
		/* psnfss4 */
		"\\ding{223}", "\\ding{224}",
		"\\ding{225}", "\\ding{232}",
		"\\ding{229}", "\\ding{230}",
		"\\ding{238}", "\\ding{237}",
		"\\ding{236}", "\\ding{235}",
		"\\ding{234}", "\\ding{233}",
		"\\ding{239}", "\\ding{241}",
		"\\ding{250}", "\\ding{251}",
		"\\ding{49}", "\\ding{50}",
		"\\ding{217}", "\\ding{245}",
		"\\ding{243}", "\\ding{248}",
		"\\ding{252}", "\\ding{253}",
		"\\ding{219}", "\\ding{213}",
		"\\ding{221}", "\\ding{222}",
		"\\ding{220}", "\\ding{212}",
		"\\Pisymbol{psy}{174}", "\\Pisymbol{psy}{222}",
		"\\ding{254}", "\\ding{242}",
		"\\ding{231}", "\\Pisymbol{psy}{45}"
	};  /* string const BulletPanels[][] */

	static char const ** BulletPanels[FONTMAX] = {
		BulletPanel0, BulletPanel1,
		BulletPanel2, BulletPanel3,
		BulletPanel4, BulletPanel5
	};

	return from_ascii(BulletPanels[f][c]);
}

void Bullet::testInvariant() const
{
#ifdef ENABLE_ASSERTIONS
	LATTEST(font >= MIN);
	LATTEST(font < FONTMAX);
	LATTEST(character >= MIN);
	LATTEST(character < CHARMAX);
	LATTEST(size >= MIN);
	LATTEST(size < SIZEMAX);
	LATTEST(user_text >= -1);
	LATTEST(user_text <= 1);
	// now some relational/operational tests
	if (user_text == 1) {
		LATTEST(font == -1 && (character == -1 && size == -1));
		//        LATTEST(!text.empty()); // this isn't necessarily an error
	}
	//      else if (user_text == -1) {
	//        LATTEST(!text.empty()); // this also isn't necessarily an error
	//      }
	//      else {
	//        // user_text == 0
	//        LATTEST(text.empty()); // not usually true
	//      }
#endif
}


} // namespace lyx

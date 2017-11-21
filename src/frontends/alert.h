// -*- C++ -*-
/**
 * \file alert.h
 * This file is part of LyX, the document processor.
 * Licence details can be found in the file COPYING.
 *
 * \author John Levon
 *
 * Full author contact details are available in file CREDITS.
 */

#ifndef LYX_ALERT_H
#define LYX_ALERT_H

#include "support/strfwd.h"


namespace lyx {

class Buffer;

namespace frontend {
namespace Alert {

/**
 * Prompt for a question. Returns 0-2 for the chosen button.
 * Set default_button and cancel_button to reasonable values. b1-b3
 * should have accelerators marked with an '&'. title should be
 * a short summary. Strings should be gettextised.
 * Please think about the poor user.
 *
 * Remember to use boost::format. If you make any of these buttons
 * "Yes" or "No", I will personally come around to your house and
 * slap you with fish, and not in an enjoyable way either.
 */
int prompt(docstring const & title, docstring const & question,
	   int default_button, int cancel_button,
	   docstring const & b1, docstring const & b2,
	   docstring const & b3 = empty_docstring(),
	   docstring const & b4 = empty_docstring());

/**
 * Display a warning to the user. Title should be a short (general) summary.
 * Only use this if the user cannot perform some remedial action.
 * \p askshowagain will display a check box where the user can turn off the
 * warning for future cases. Ponder carefully if this is feasible.
 *
 * The console output takes care of converting any Qt html to plain text.
 */
void warning(docstring const & title, docstring const & message,
	     bool const & askshowagain = false);

/**
 * Display a warning to the user. Title should be a short (general) summary.
 * Only use this if the user cannot perform some remedial action.
 * On some systems it is possible to show a backtrace.
 *
 * The console output takes care of converting any Qt html to plain text.
 */
void error(docstring const & title, docstring const & message, bool backtrace = false);

/**
 * Informational message. Use very very sparingly. That is, you must
 * apply to me, in triplicate, under the sea, breathing in petrol
 * and reciting the Nicene Creed, whilst running uphill and also
 * eating.
 */
void information(docstring const & title, docstring const & message);

/**
 * Asks for a text. Returns true for OK. Obtained message is in response
 * (even empty string). dflt stands for default message in the dialog.
 */
bool askForText(docstring & response, docstring const & msg,
	docstring const & dflt = empty_docstring());

/**
 * Prompts for opening a URL. If it is a local file, use the Formats class to
 * prompt to open it using the application configured in LyX. Otherwise we use
 * Qt's facilities to understand the URL and open the file; the user is prompted
 * by being shown the URL in ASCII to prevent spoofing.
 */
bool openUrl(docstring const & url, Buffer const & buf);

} // namespace Alert
} // namespace frontend
} // namespace lyx

#endif // LYX_ALERT_H

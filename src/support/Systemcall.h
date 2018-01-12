// -*- C++ -*-
/**
 * \file Systemcall.h
 * This file is part of LyX, the document processor.
 * Licence details can be found in the file COPYING.
 *
 * \author Asger Alstrup
 *
 * Interface cleaned up by
 * \author Angus Leeming
 *
 * Full author contact details are available in file CREDITS.
 */

#ifndef SYSTEMCALL_H
#define SYSTEMCALL_H

#include <string>

namespace lyx {
namespace support {

/**
 * An instance of Class Systemcall represents a single child process.
 *
 * Class Systemcall uses system() to launch the child process.
 * The user can choose to wait or not wait for the process to complete, but no
 * callback is invoked upon completion of the child.
 *
 * The child process is not killed when the Systemcall instance goes out of
 * scope.
 */
class Systemcall {
public:
	/// whether to wait for completion
	enum Starttype {
		Wait,     //< wait for completion before returning from startscript()
		WaitLoop, //< wait, but check occasionally for cancellation
		DontWait  //< don't wait for completion
	};

	// enum values chosen hopefully not to conflict with ordinary return values
	enum ReturnValue {
		OK = 0,
		NOSTART = 1001,
		ERROR = 2001,
		TIMEOUT = 4001,
		KILLED = 9001
	};


	// Kill the process which is running at time being
	static void killscript();

	/** Start child process.
	 *  The string "what" contains a commandline with arguments separated
	 *  by spaces and encoded in the filesystem encoding. "$$s" will be
	 *  replaced accordingly by commandPrep(). The string "path" contains
	 *  the path to be prepended to the TEXINPUTS environment variable
	 *  encoded in utf-8. Similarly for the string "lpath" that, if not
	 *  empty, specifies an additional directory to be added to TEXINPUTS
	 *  but after "path". Unset "process_events" in case UI should be
	 *  blocked while  processing the external command.
	 */
	int startscript(Starttype how, std::string const & what,
			std::string const & path = empty_string(),
			std::string const & lpath = empty_string(),
			bool process_events = false);
};

} // namespace support
} // namespace lyx

#endif // SYSTEMCALL_H

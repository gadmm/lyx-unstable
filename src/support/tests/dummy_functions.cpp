#include <config.h>

#include "support/Messages.h"

using namespace std;

namespace lyx {
	// Dummy verbose support
	bool verbose = false;

	// Dummy LyXRC support
	class LyXRC { string icon_set; } lyxrc;

	// Keep the linker happy on Windows
	void lyx_exit(int) {}

	// Dummy language support
	Messages const & getGuiMessages()
	{
		static Messages lyx_messages;

		return lyx_messages;
	}
	Messages const & getMessages(string const &)
	{
		static Messages lyx_messages;

		return lyx_messages;
	}

} // namespace lyx

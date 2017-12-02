// -*- C++ -*-
/**
 * \file src/OutputEnums.h
 * This file is part of LyX, the document processor.
 * Licence details can be found in the file COPYING.
 *
 * \author Richard Heck
 * \author Guillaume Munch
 *
 * Full author contact details are available in file CREDITS.
 */

#ifndef OUTPUTENUMS_H
#define OUTPUTENUMS_H

#include <set>

namespace lyx {

enum UpdateType {
	InternalUpdate,
	OutputUpdate
};

enum struct AuxFiles {
	Bib,
	All
};

using AuxFilesFlags = std::set<AuxFiles>;

}

#endif // OUTPUTENUMS_H

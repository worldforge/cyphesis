// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifndef RULESETS_ARITHMETIC_SCRIPT_H
#define RULESETS_ARITHMETIC_SCRIPT_H

#include <string>

/// \brief Base class for script objects which handle statistics for entities.
///
/// This base class allows scripts to override attribute requests
class ArithmeticScript {
  public:
    ArithmeticScript();
    virtual ~ArithmeticScript();

    virtual int attribute(const std::string &, float &) = 0;
};

#endif // RULESETS_ARITHMETIC_SCRIPT_H

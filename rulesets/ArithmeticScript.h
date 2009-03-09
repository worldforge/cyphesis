// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2006 Alistair Riddoch
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

// $Id$

#ifndef RULESETS_ARITHMETIC_SCRIPT_H
#define RULESETS_ARITHMETIC_SCRIPT_H

#include <string>

/// \brief Base class for script objects which handle statistics for entities.
///
/// This base class allows scripts to override attribute requests
class ArithmeticScript {
  public:
    virtual ~ArithmeticScript();

    /// \brief Retrieve a named value from the arithmetic model
    ///
    /// @param name of the value to be retrieved
    /// @param val value returned by the model
    virtual int attribute(const std::string & name, float & val) = 0;

    /// \brief Retrieve a named value from the arithmetic model
    ///
    /// @param name of the value to be retrieved
    /// @param val value returned by the model
    virtual void set(const std::string & name, const float & val) = 0;
};

#endif // RULESETS_ARITHMETIC_SCRIPT_H

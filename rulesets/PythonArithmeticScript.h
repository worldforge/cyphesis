// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2005 Alistair Riddoch
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

#ifndef RULESETS_PYTHON_ARITHMETIC_SCRIPT_H
#define RULESETS_PYTHON_ARITHMETIC_SCRIPT_H

#include "ArithmeticScript.h"

/// \brief Base class for script objects which handle statistics for entities.
///
/// This base class allows scripts to override attribute requests
class PythonArithmeticScript : public ArithmeticScript {
  protected:
    /// \brief Python instance object implementing the script
    struct _object * m_script;
  public:
    PythonArithmeticScript(struct _object * script);
    virtual ~PythonArithmeticScript();

    /// \brief Accessor for python instance object implementing the script
    struct _object * script() const { return m_script; }

    virtual int attribute(const std::string & name, float & val);
    virtual void set(const std::string & name, const float & val);
};

#endif // RULESETS_PYTHON_ARITHMETIC_SCRIPT_H

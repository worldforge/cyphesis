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

#ifndef RULESETS_PYTHON_ARITHMETIC_FACTORY_H
#define RULESETS_PYTHON_ARITHMETIC_FACTORY_H

#include "ArithmeticFactory.h"

#include "rulesets/PythonClass.h"

#include <string>

/// \brief Factory class for for creating python arithmetic scripts
class PythonArithmeticFactory : public ArithmeticKit, private PythonClass {
  public:
    PythonArithmeticFactory(const std::string & package,
                            const std::string & name);
    virtual ~PythonArithmeticFactory();

    int setup();

    virtual ArithmeticScript * newScript(LocatedEntity * owner);
};

#endif // RULESETS_PYTHON_ARITHMETIC_FACTORY_H

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

#ifndef RULESETS_ARITHMETIC_FACTORY_H
#define RULESETS_ARITHMETIC_FACTORY_H

#include <string>

class ArithmeticScript;
class Entity;

/// \brief Base class for for factories for creating arithmetic scripts
class ArithmeticKit {
  public:
    virtual ~ArithmeticKit();

    /// \brief Create a new arithmetic model object
    ///
    /// @param chr The Character object for which the model should be created
    virtual ArithmeticScript * newScript(Entity * owner) = 0;
};

/// \brief Factory class for for creating python arithmetic scripts
class PythonArithmeticFactory : public ArithmeticKit {
  public:
    /// /brief Python module object containing the script type
    struct _object * m_module;
    /// \brief Python clas object for the script type
    struct _object * m_class;

    /// \brief Name of the package containing the script
    std::string m_package;
    /// \brief Name of the type within the package for the script
    std::string m_type;

    int getClass();
    int addScript();
  public:
    PythonArithmeticFactory(const std::string & package,
                            const std::string & name);
    virtual ~PythonArithmeticFactory();

    virtual ArithmeticScript * newScript(Entity * owner);
};

#endif // RULESETS_ARITHMETIC_FACTORY_H

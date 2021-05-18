// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2008 Alistair Riddoch
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


#ifndef COMMON_VARIABLE_H
#define COMMON_VARIABLE_H

#include <iosfwd>

/// \brief Abstract class for dynamic variable monitors
///
/// Variables which should be represented in the monitors output are
/// referenced by subclasses this interface
class VariableBase {
  public:
    virtual ~VariableBase() = default;
    virtual void send(std::ostream &) = 0;
    virtual bool isNumeric() const = 0;
};

/// \brief Concrete class template for dynamic variable monitors
///
/// Objects of this type expose the variable they reference to the
/// monitoring subsystem.
template<typename T>
class Variable : public VariableBase {
  protected:
    const T & m_variable;
  public:
    explicit Variable(const T & variable);

    ~Variable() override;
    void send(std::ostream &) override;
    bool isNumeric() const override;
};

#endif // COMMON_VARIABLE_H

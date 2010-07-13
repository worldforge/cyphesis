// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2010 Alistair Riddoch
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

#ifndef SERVER_ARITHMETIC_BUILDER_H
#define SERVER_ARITHMETIC_BUILDER_H

#include <string>

class ArithmeticScript;
class Entity;

/// \brief Builder to handle the creation of arithmetic objects
class ArithmeticBuilder {
  protected:
    static ArithmeticBuilder * m_instance;

    ArithmeticBuilder();
  public:
    static ArithmeticBuilder * instance();
    static void del();

    ArithmeticScript * newArithmetic(const std::string &, Entity *);
};

#endif // SERVER_ARITHMETIC_BUILDER_H

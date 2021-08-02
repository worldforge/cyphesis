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


#include "Variable.h"

#include <iostream>

template <typename T>
Variable<T>::Variable(const T & variable) : m_variable(variable)
{
}

template <typename T>
Variable<T>::~Variable() = default;

template <typename T>
void Variable<T>::send(std::ostream & o)
{
    o << m_variable;
}


template <>
bool Variable<int>::isNumeric() const
{
    return true;
}

template <>
bool Variable<long>::isNumeric() const
{
    return true;
}

template <>
bool Variable<float>::isNumeric() const
{
    return true;
}

template <>
bool Variable<std::string>::isNumeric() const
{
    return false;
}

template <>
bool Variable<const char *>::isNumeric() const
{
    return false;
}

template class Variable<int>;
template class Variable<long>;
template class Variable<float>;
template class Variable<std::string>;
template class Variable<const char *>;


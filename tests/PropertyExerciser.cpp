// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2007 Alistair Riddoch
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

// $Id: PropertyExerciser.cpp,v 1.1 2007-01-11 22:35:06 alriddoch Exp $

#include "PropertyExerciser.h"

#include "common/Property.h"

#include <limits>

using Atlas::Message::Element;

PropertyExerciser::PropertyExerciser()
{
    integer_values.push_back(0);
    integer_values.push_back(-1);
    integer_values.push_back(23);
    integer_values.push_back(42);
    integer_values.push_back(INT_MAX);
    integer_values.push_back(INT_MIN);
    integer_values.push_back(UINT_MAX);
    integer_values.push_back(LONG_MAX);
    integer_values.push_back(LONG_MIN);

    float_values.push_back(0.f);
    float_values.push_back(-0.f);
    float_values.push_back(1.f);
    float_values.push_back(-1.f);
    float_values.push_back(23.f);
    float_values.push_back(42.f);
    float_values.push_back(-23.f);
    float_values.push_back(-42.f);
    float_values.push_back(__FLT_MIN__);
    float_values.push_back(__FLT_MAX__);
    float_values.push_back(__DBL_MIN__);
    float_values.push_back(__DBL_MAX__);

    ptr_values.push_back(0);
    ptr_values.push_back(&integer_ptr_target);
    ptr_values.push_back(&float_ptr_target);
    ptr_values.push_back(&ptr_ptr_target);
    ptr_values.push_back(&string_ptr_target);
    ptr_values.push_back(&map_ptr_target);
    ptr_values.push_back(&list_ptr_target);

    string_values.push_back("");
    string_values.push_back("hello world");
    string_values.push_back("goodbye, cruel world");
    string_values.push_back(std::string(32768, ' '));
    string_values.push_back(std::string(32768, 'a'));
    string_values.push_back(std::string(32768, 'B'));
    string_values.push_back(std::string(32768, '0'));
    string_values.push_back("!£$%^&*()_+}{[]:@~#';<>?/.,l\\|");
    string_values.push_back("pwu3dc5012cw*/-+3+Q£%$\"q%2");

}

int PropertyExerciser::exerciseProperty(PropertyBase & property,
                                        Element::Type element_type)
{
    Element get_target;
    if (property.get(get_target)) {
        assert(get_target.getType() == element_type);
    } else {
        assert(get_target.getType() == Element::TYPE_NONE);
    }
    return 0;
}

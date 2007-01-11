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

// $Id: AllPropertytest.cpp,v 1.1 2007-01-11 22:35:06 alriddoch Exp $

#include "PropertyExerciser.h"

#include "common/DynamicProperty_impl.h"

#include <cassert>

using Atlas::Message::Element;

static const Element::Type TYPE_INT = Element::TYPE_INT;
static const Element::Type TYPE_FLOAT = Element::TYPE_FLOAT;

int main()
{
    PropertyExerciser exerciser;

    {
        int test_integer_val;
        Property<int> test_property(test_integer_val, 0);
        assert(exerciser.exerciseProperty(test_property, TYPE_INT) == 0);
    }

    {
        DynamicProperty<int> test_property;
        assert(exerciser.exerciseProperty(test_property, TYPE_INT) == 0);
    }

    {
        long test_long_val;
        Property<long> test_property(test_long_val, 0);
        assert(exerciser.exerciseProperty(test_property, TYPE_INT) == 0);
    }

    {
        DynamicProperty<long> test_property;
        assert(exerciser.exerciseProperty(test_property, TYPE_INT) == 0);
    }

    {
        float test_float_val;
        Property<float> test_property(test_float_val, 0);
        assert(exerciser.exerciseProperty(test_property, TYPE_FLOAT) == 0);
    }

    {
        DynamicProperty<float> test_property;
        assert(exerciser.exerciseProperty(test_property, TYPE_FLOAT) == 0);
    }

    {
        double test_double_val;
        Property<double> test_property(test_double_val, 0);
        assert(exerciser.exerciseProperty(test_property, TYPE_FLOAT) == 0);
    }

    {
        DynamicProperty<double> test_property;
        assert(exerciser.exerciseProperty(test_property, TYPE_FLOAT) == 0);
    }

    return 0;
}

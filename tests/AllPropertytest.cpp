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

// $Id: AllPropertytest.cpp,v 1.3 2007-01-16 01:07:49 alriddoch Exp $

#include "PropertyExerciser.h"

#include "rulesets/AreaProperty.h"
#include "rulesets/AtlasProperties.h"
#include "rulesets/CalendarProperty.h"
#include "rulesets/EntityProperty.h"
#include "rulesets/LineProperty.h"
#include "rulesets/OutfitProperty.h"
#include "rulesets/StatisticsProperty.h"
#include "rulesets/TerrainProperty.h"

#include "rulesets/Character.h"
#include "rulesets/Statistics.h"

#include "common/DynamicProperty_impl.h"

#include <Mercator/Terrain.h>

#include <cassert>

using Atlas::Message::Element;

static const Element::Type TYPE_INT = Element::TYPE_INT;
static const Element::Type TYPE_FLOAT = Element::TYPE_FLOAT;
static const Element::Type TYPE_STRING = Element::TYPE_STRING;
static const Element::Type TYPE_PTR = Element::TYPE_PTR;
static const Element::Type TYPE_LIST = Element::TYPE_LIST;
static const Element::Type TYPE_MAP = Element::TYPE_MAP;

int main()
{
    PropertyExerciser exerciser;

    {
        int test_integer_val;
        Property<int> test_property(test_integer_val, 0);
        assert(exerciser.exerciseProperty(test_property, TYPE_INT) == 0);
    }

    {
        int test_integer_val = 1;
        ImmutableProperty<int> test_property(test_integer_val, 0);
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
        long test_long_val = 1;
        ImmutableProperty<long> test_property(test_long_val, 0);
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
        float test_float_val = 0.1f;
        ImmutableProperty<float> test_property(test_float_val, 0);
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
        double test_double_val = 0.1;
        ImmutableProperty<double> test_property(test_double_val, 0);
        assert(exerciser.exerciseProperty(test_property, TYPE_FLOAT) == 0);
    }

    {
        DynamicProperty<double> test_property;
        assert(exerciser.exerciseProperty(test_property, TYPE_FLOAT) == 0);
    }

    {
        std::string test_string_val;
        Property<std::string> test_property(test_string_val, 0);
        assert(exerciser.exerciseProperty(test_property, TYPE_STRING) == 0);
    }

    {
        std::string test_string_val("bob");
        ImmutableProperty<std::string> test_property(test_string_val, 0);
        assert(exerciser.exerciseProperty(test_property, TYPE_STRING) == 0);
    }

    {
        DynamicProperty<std::string> test_property;
        assert(exerciser.exerciseProperty(test_property, TYPE_STRING) == 0);
    }

    {
        AreaProperty test_property(0);
        assert(exerciser.exerciseProperty(test_property, TYPE_MAP) == 0);
    }

    {
        std::string test_string_val;
        IdProperty test_property(test_string_val);
        assert(exerciser.exerciseProperty(test_property, TYPE_STRING) == 0);
    }

    {
        std::string test_string_val("1");
        IdProperty test_property(test_string_val);
        assert(exerciser.exerciseProperty(test_property, TYPE_STRING) == 0);
    }

    {
        std::string test_string_val;
        NameProperty test_property(test_string_val, 0);
        assert(exerciser.exerciseProperty(test_property, TYPE_STRING) == 0);
    }

    {
        std::string test_string_val("bob");
        NameProperty test_property(test_string_val, 0);
        assert(exerciser.exerciseProperty(test_property, TYPE_STRING) == 0);
    }

    {
        Dynamic<NameProperty, std::string> test_property;
        assert(exerciser.exerciseProperty(test_property, TYPE_STRING) == 0);
    }

    {
        EntitySet test_entityset_val;
        ContainsProperty test_property(test_entityset_val);
        assert(exerciser.exerciseProperty(test_property, TYPE_LIST) == 0);
    }

    {
        CalendarProperty test_property(0);
        assert(exerciser.exerciseProperty(test_property, TYPE_MAP) == 0);
    }

#if 0
    // FIXME This currently segfaults, as we give it pointers that are not
    // pointers to entities. This highlights the fact that we need to 
    // protect property code from getting given pointers from outside.
    // Atlas-C++ must not allow pointers to come in from the network.
    {
        EntityRef test_entityref_data;
        EntityProperty test_property(test_entityref_data);
        assert(exerciser.exerciseProperty(test_property, TYPE_STRING) == 0);
    }
#endif

    // FIXME Yay this throws!!! I found a bug with my foolish tests
    {
        CoordList test_coordlist_data;
        LineProperty test_property(test_coordlist_data, 0);
        assert(exerciser.exerciseProperty(test_property, TYPE_LIST) == 0);
    }

#if 0
    // FIXME THis segfaults, probably the same reason as why EntityRef
    // does
    {
        OutfitProperty test_property;
        assert(exerciser.exerciseProperty(test_property, TYPE_MAP) == 0);
    }
#endif

    {
        Character test_char("1", 1);
        StatisticsProperty test_property(test_char.statistics(), 0);
        assert(exerciser.exerciseProperty(test_property, TYPE_MAP) == 0);
    }

    {
        Mercator::Terrain test_terrain_data;
        PointSet test_pointset_data1, test_pointset_data2;
        TerrainProperty test_property(test_terrain_data,
                                      test_pointset_data1,
                                      test_pointset_data2, 0);
        assert(exerciser.exerciseProperty(test_property, TYPE_MAP) == 0);
    }

    return 0;
}

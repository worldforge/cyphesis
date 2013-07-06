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


#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include "PropertyExerciser.h"

#include "rulesets/AreaProperty.h"
#include "rulesets/AtlasProperties.h"
#include "rulesets/CalendarProperty.h"
#include "rulesets/EntityProperty.h"
#include "rulesets/LineProperty.h"
#include "rulesets/OutfitProperty.h"
#include "rulesets/StatisticsProperty.h"
#include "rulesets/TerrainProperty.h"

#include "rulesets/ArithmeticFactory.h"

#include "common/BaseWorld.h"
#include "common/log.h"
#include "common/Property_impl.h"
#include "common/TypeNode.h"

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
        Property<int> test_property(0);
        assert(exerciser.exerciseProperty(test_property, TYPE_INT) == 0);
    }

    {
        Property<long> test_property(0);
        assert(exerciser.exerciseProperty(test_property, TYPE_INT) == 0);
    }

    {
        Property<float> test_property(0);
        assert(exerciser.exerciseProperty(test_property, TYPE_FLOAT) == 0);
    }

    {
        Property<double> test_property(0);
        assert(exerciser.exerciseProperty(test_property, TYPE_FLOAT) == 0);
    }

    {
        Property<std::string> test_property(0);
        assert(exerciser.exerciseProperty(test_property, TYPE_STRING) == 0);
    }

    {
        AreaProperty test_property;
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
        NameProperty test_property(0);
        assert(exerciser.exerciseProperty(test_property, TYPE_STRING) == 0);
    }

    {
        NameProperty test_property(0);
        assert(exerciser.exerciseProperty(test_property, TYPE_STRING) == 0);
    }

    {
        LocatedEntitySet test_entityset_val;
        ContainsProperty test_property(test_entityset_val);
        assert(exerciser.exerciseProperty(test_property, TYPE_LIST) == 0);
    }

    {
        CalendarProperty test_property;
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
        LineProperty test_property;
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
        StatisticsProperty test_property;
        assert(exerciser.exerciseProperty(test_property, TYPE_MAP) == 0);
    }

    {
        TerrainProperty test_property;
        assert(exerciser.exerciseProperty(test_property, TYPE_MAP) == 0);
    }

    return 0;
}

// stubs

BaseWorld * BaseWorld::m_instance = 0;

BaseWorld::BaseWorld(LocatedEntity & gw) : m_gameWorld(gw)
{
    m_instance = this;
}

BaseWorld::~BaseWorld()
{
    m_instance = 0;
}

LocatedEntity * BaseWorld::getEntity(const std::string & id) const
{
    return 0;
}

LocatedEntity * BaseWorld::getEntity(long id) const
{
    return 0;
}

TypeNode::TypeNode(const std::string & name) : m_name(name), m_parent(0)
{
}

TypeNode::TypeNode(const std::string & name,
                   const Atlas::Objects::Root & d) : m_name(name),
                                                     m_description(d),
                                                     m_parent(0)
{
}

TypeNode::~TypeNode()
{
}

bool TypeNode::isTypeOf(const std::string & base_type) const
{
    const TypeNode * node = this;
    do {
        if (node->name() == base_type) {
            return true;
        }
        node = node->parent();
    } while (node != 0);
    return false;
}

bool TypeNode::isTypeOf(const TypeNode * base_type) const
{
    const TypeNode * node = this;
    do {
        if (node == base_type) {
            return true;
        }
        node = node->parent();
    } while (node != 0);
    return false;
}

void log(LogLevel lvl, const std::string & msg)
{
}

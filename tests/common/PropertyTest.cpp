// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2004 Alistair Riddoch
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

#include "common/Property.h"
#include "../TestEntity.h"

#include <Atlas/Objects/SmartPtr.h>
#include <Atlas/Objects/Anonymous.h>

#include <cassert>

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Objects::Entity::Anonymous;

class MinimalProperty : public PropertyBase
{
    public:
        MinimalProperty()
        {}

        virtual int get(Atlas::Message::Element& val) const
        { return 0; }

        virtual void set(const Atlas::Message::Element& val)
        {}

        virtual MinimalProperty* copy() const
        { return new MinimalProperty; }
};

static void exerciseProperty(PropertyBase* pb, LocatedEntity& entity)
{
    pb->install(entity, "test_prop");
    pb->apply(entity);
    MapType map;
    pb->add("test_name", map);
    Anonymous ent;
    pb->add("test_name", ent);
}

int main()
{
    TestEntity entity{};
    // Assertions to verify the flags have the desired properties.
    assert((prop_flag_persistence_clean | prop_flag_persistence_mask) == prop_flag_persistence_mask);
    assert((prop_flag_persistence_ephem | prop_flag_persistence_mask) == prop_flag_persistence_mask);
    assert((prop_flag_persistence_ephem | prop_flag_persistence_clean) == prop_flag_persistence_mask);

    assert((prop_flag_persistence_clean & prop_flag_persistence_mask) == prop_flag_persistence_clean);
    assert((prop_flag_persistence_ephem & prop_flag_persistence_mask) == prop_flag_persistence_ephem);
    assert((prop_flag_persistence_ephem & prop_flag_persistence_clean) == 0);

    assert((prop_flag_visibility_private | prop_flag_visibility_non_public) == prop_flag_visibility_non_public);
    assert((prop_flag_visibility_protected | prop_flag_visibility_non_public) == prop_flag_visibility_non_public);
    assert((prop_flag_visibility_protected | prop_flag_visibility_private) == prop_flag_visibility_non_public);

    assert((prop_flag_visibility_private & prop_flag_visibility_non_public) == prop_flag_visibility_private);
    assert((prop_flag_visibility_protected & prop_flag_visibility_non_public) == prop_flag_visibility_protected);
    assert((prop_flag_visibility_protected & prop_flag_visibility_private) == 0);

    assert((prop_flag_visibility_non_public & prop_flag_persistence_mask) == 0);

    assert(!PropertyUtil::isValidName("abcdefghijklmnopqrstuvwxyzabcdefg")); //33 characters
    assert(PropertyUtil::isValidName("abcdefghijklmnopqrstuvwxyzabcdef")); //32 characters
    assert(!PropertyUtil::isValidName(""));
    assert(PropertyUtil::isValidName("1"));
    assert(PropertyUtil::isValidName("a1"));
    assert(PropertyUtil::isValidName("$"));
    assert(!PropertyUtil::isValidName("Ö"));
    assert(PropertyUtil::isValidName("a"));
    assert(PropertyUtil::isValidName("A"));
    assert(PropertyUtil::isValidName("_a"));
    assert(PropertyUtil::isValidName("__a"));
    assert(PropertyUtil::isValidName("___a"));
    assert(PropertyUtil::isValidName("$eid"));
    assert(PropertyUtil::isValidName("foo-"));
    assert(PropertyUtil::isValidName("-"));
    assert(!PropertyUtil::isValidName("@"));
    assert(!PropertyUtil::isValidName(":"));
    assert(!PropertyUtil::isValidName(";"));
    assert(!PropertyUtil::isValidName("!"));
    assert(!PropertyUtil::isValidName("foo!append"));


    assert(PropertyUtil::flagsForPropertyName("foo") == 0);
    assert(PropertyUtil::flagsForPropertyName("_foo") == prop_flag_visibility_protected);
    assert(PropertyUtil::flagsForPropertyName("__foo") == prop_flag_visibility_private);
    assert(PropertyUtil::flagsForPropertyName("foo_") == 0);
    assert(PropertyUtil::flagsForPropertyName("f_oo") == 0);

    auto parsed = PropertyUtil::parsePropertyModification("foo");
    assert(parsed.first == ModifierType::Default);
    assert(parsed.second == "foo");

    parsed = PropertyUtil::parsePropertyModification("foo!");
    assert(parsed.first == ModifierType::Default);
    assert(parsed.second == "foo!");

    parsed = PropertyUtil::parsePropertyModification("foo!fdsfds");
    assert(parsed.first == ModifierType::Default);
    assert(parsed.second == "foo!fdsfds");

    parsed = PropertyUtil::parsePropertyModification("foo!default");
    assert(parsed.first == ModifierType::Default);
    assert(parsed.second == "foo");

    parsed = PropertyUtil::parsePropertyModification("foo!append");
    assert(parsed.first == ModifierType::Append);
    assert(parsed.second == "foo");

    parsed = PropertyUtil::parsePropertyModification("foo!prepend");
    assert(parsed.first == ModifierType::Prepend);
    assert(parsed.second == "foo");

    parsed = PropertyUtil::parsePropertyModification("foo!subtract");
    assert(parsed.first == ModifierType::Subtract);
    assert(parsed.second == "foo");

    parsed = PropertyUtil::parsePropertyModification("foo!add-fraction");
    assert(parsed.first == ModifierType::AddFraction);
    assert(parsed.second == "foo");


    Element val;

    {
        PropertyBase* pb = new MinimalProperty;
        exerciseProperty(pb, entity);
        delete pb;
    }

    {
        long i = 23;
        PropertyBase* pb = new SoftProperty;
        assert(pb->flags().m_flags == 0);
        pb->set(i);
        pb->get(val);
        assert(val == i);
        exerciseProperty(pb, entity);
        delete pb;
    }

    {
        long i = 23;
        PropertyBase* pb = new SoftProperty(i);
        assert(pb->flags().m_flags == 0);
        pb->get(val);
        assert(val == i);
        exerciseProperty(pb, entity);
        delete pb;
    }

    {
        long i = 23;
        PropertyBase* pb = new Property<int>(0);
        assert(pb->flags().m_flags == 0);
        pb->set(i);
        pb->get(val);
        assert(val == i);
        exerciseProperty(pb, entity);
        delete pb;
    }

    {
        long i = 23;
        PropertyBase* pb = new Property<long>(0);
        assert(pb->flags().m_flags == 0);
        pb->set(i);
        pb->get(val);
        assert(val == i);
        exerciseProperty(pb, entity);
        delete pb;
    }

    {
        float f = 17.2f;
        PropertyBase* pb = new Property<float>(1);
        assert(pb->flags().m_flags == 1);
        pb->set(f);
        pb->get(val);
        assert(val == f);
        exerciseProperty(pb, entity);
        delete pb;
    }

    {
        double d = 65.4;
        PropertyBase* pb = new Property<double>(2);
        assert(pb->flags().m_flags == 2);
        pb->set(d);
        pb->get(val);
        assert(val == d);
        exerciseProperty(pb, entity);
        delete pb;
    }

    {
        std::string s = "Test String";
        PropertyBase* pb = new Property<std::string>(3);
        assert(pb->flags().m_flags == 3);
        pb->set(s);
        pb->get(val);
        assert(val == s);
        exerciseProperty(pb, entity);
        delete pb;
    }

    {
        long i = 23;
        PropertyBase* pb = new Property<long>(4);
        assert(pb->flags().m_flags == 4);
        pb->set(i);
        pb->get(val);
        assert(val == i);
        exerciseProperty(pb, entity);
        delete pb;
    }

    {
        MapType m;
        m.emplace("foo", "bar");
        PropertyBase* pb = new Property<MapType>(5);
        assert(pb->flags().m_flags == 5);
        pb->set(m);
        pb->get(val);
        assert(val == m);
        exerciseProperty(pb, entity);
        delete pb;
    }

    {
        long i = 1;
        PropertyBase* pb = new BoolProperty();
        assert(pb->flags().m_flags == 0);
        pb->set(i);
        pb->get(val);
        assert(val == i);
        pb->set(200);
        pb->get(val);
        assert(val == 1L);
        exerciseProperty(pb, entity);
        delete pb;
    }
}

#include "../stubs/common/stublog.h"
#include "../stubs/rules/stubLocatedEntity.h"
#include "../stubs/common/stubRouter.h"
#include "../stubs/rules/stubLocation.h"

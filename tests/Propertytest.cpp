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

// $Id$

#include "common/Property.h"

#include <Atlas/Objects/SmartPtr.h>
#include <Atlas/Objects/Anonymous.h>

#include <cassert>

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Objects::Entity::Anonymous;

class MinimalProperty : public PropertyBase {
  public:
    MinimalProperty() : PropertyBase(0) { }
    virtual bool get(Atlas::Message::Element & val) const { return true; }
    virtual void set(const Atlas::Message::Element & val) { }

};

static void exerciseProperty(PropertyBase * pb)
{
    pb->install(0);
    pb->apply(0);
    MapType map;
    pb->add("test_name", map);
    Anonymous ent;
    pb->add("test_name", ent);
}

int main()
{
    // Assertions to verify the flags have the desired properties.
    assert((per_clean | per_mask) == per_mask);
    assert((per_ephem | per_mask) == per_mask);
    assert((per_ephem | per_clean) == per_mask);

    assert((per_clean & per_mask) == per_clean);
    assert((per_ephem & per_mask) == per_ephem);
    assert((per_ephem & per_clean) == 0);

    assert((vis_hidden | vis_mask) == vis_mask);
    assert((vis_internal | vis_mask) == vis_mask);
    assert((vis_internal | vis_hidden) == vis_mask);

    assert((vis_hidden & vis_mask) == vis_hidden);
    assert((vis_internal & vis_mask) == vis_internal);
    assert((vis_internal & vis_hidden) == 0);

    assert((vis_mask & per_mask) == 0);

    Element val;

    {
    PropertyBase * pb = new MinimalProperty;
    exerciseProperty(pb);
    delete pb;
    }

    {
    long i = 23;
    PropertyBase * pb = new SoftProperty;
    assert(pb->flags() == 0);
    pb->set(i);
    pb->get(val);
    assert(val == i);
    exerciseProperty(pb);
    delete pb;
    }

    {
    long i = 23;
    PropertyBase * pb = new SoftProperty(i);
    assert(pb->flags() == 0);
    pb->get(val);
    assert(val == i);
    exerciseProperty(pb);
    delete pb;
    }

    {
    long i = 23;
    PropertyBase * pb = new Property<int>(0);
    assert(pb->flags() == 0);
    pb->set(i);
    pb->get(val);
    assert(val == i);
    exerciseProperty(pb);
    delete pb;
    }

    {
    long i = 23;
    PropertyBase * pb = new Property<long>(0);
    assert(pb->flags() == 0);
    pb->set(i);
    pb->get(val);
    assert(val == i);
    exerciseProperty(pb);
    delete pb;
    }

    {
    float f = 17.2f;
    PropertyBase * pb = new Property<float>(1);
    assert(pb->flags() == 1);
    pb->set(f);
    pb->get(val);
    assert(val == f);
    exerciseProperty(pb);
    delete pb;
    }

    {
    double d = 65.4;
    PropertyBase * pb = new Property<double>(2);
    assert(pb->flags() == 2);
    pb->set(d);
    pb->get(val);
    assert(val == d);
    exerciseProperty(pb);
    delete pb;
    }

    {
    std::string s = "Test String";
    PropertyBase * pb = new Property<std::string>(3);
    assert(pb->flags() == 3);
    pb->set(s);
    pb->get(val);
    assert(val == s);
    exerciseProperty(pb);
    delete pb;
    }

    {
    long i = 23;
    PropertyBase * pb = new Property<long>(4);
    assert(pb->flags() == 4);
    pb->set(i);
    pb->get(val);
    assert(val == i);
    exerciseProperty(pb);
    delete pb;
    }
}

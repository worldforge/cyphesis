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

// $Id: EntityPropertytest.cpp,v 1.2 2006-12-31 15:56:35 alriddoch Exp $

#include "rulesets/EntityProperty.h"

#include "rulesets/Entity.h"

#include <Atlas/Objects/Anonymous.h>

#include <cassert>

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Objects::Entity::Anonymous;

int main()
{

    {
        // Test constructor
        PropertyBase * pb = new EntityProperty(*new EntityRef);
        delete pb;
    }

    {
        // Check constructor has set flags correctly to zero
        PropertyBase * pb = new EntityProperty(* new EntityRef);
        assert(pb->flags() == 0);
        delete pb;
    }

    {
        // Check getting the value fails when property is unassigned
        Element val;

        PropertyBase * pb = new EntityProperty(* new EntityRef);
        assert(pb->get(val) == false);
        delete pb;
    }

    {
        // Check that setting the value to a pointer works
        Entity ent("1", 1);

        PropertyBase * pb = new EntityProperty(* new EntityRef);
        pb->set(Atlas::Message::Element(&ent));
        delete pb;
    }

    {
        // Check that setting the value to a pointer works can get retrieved
        Entity ent("1", 1);
        Element val;

        PropertyBase * pb = new EntityProperty(* new EntityRef);
        pb->set(Atlas::Message::Element(&ent));
        assert(pb->get(val) == true);
        assert(val.isString());
        assert(val.String() == ent.getId());
        delete pb;
    }

    {
        // Check that adding the uninitialised value to a message works.
        MapType map;
        static const std::string key = "foo";

        PropertyBase * pb = new EntityProperty(* new EntityRef);

        MapType::const_iterator I = map.find(key);
        assert(I == map.end());

        pb->add(key, map);

        I = map.find(key);
        assert(I != map.end());
        assert(I->second.isString());
        assert(I->second.String().empty());
        delete pb;
    }

    {
        // Check that adding the uninitialised value to an argument works.
        Anonymous arg;
        static const std::string key = "foo";
        Element val;

        PropertyBase * pb = new EntityProperty(* new EntityRef);

        assert(!arg->hasAttr(key));
        assert(arg->copyAttr(key, val) != 0);

        pb->add(key, arg);

        assert(arg->hasAttr(key));
        assert(arg->copyAttr(key, val) == 0);
        assert(val.isString());
        assert(val.String().empty());
        delete pb;
    }

    {
        // Check that adding the uninitialised value to an argument as a hard
        // attribute works
        Anonymous arg;
        static const std::string key = "id";
        Element val;

        PropertyBase * pb = new EntityProperty(* new EntityRef);

        assert(!arg->hasAttr(key));
        // Hard coded attribute ID has not been set, so returns false, but
        // copying it gives us the default
        assert(arg->copyAttr(key, val) == 0);
        assert(val.isString());

        pb->add(key, arg);

        assert(arg->hasAttr(key));
        assert(arg->copyAttr(key, val) == 0);
        assert(val.isString());
        delete pb;
    }

    {
        // Check that adding the value to a message works.
        Entity ent("1", 1);
        MapType map;
        static const std::string key = "foo";

        PropertyBase * pb = new EntityProperty(* new EntityRef);
        pb->set(Atlas::Message::Element(&ent));

        MapType::const_iterator I = map.find(key);
        assert(I == map.end());

        pb->add(key, map);

        I = map.find(key);
        assert(I != map.end());
        assert(I->second.isString());
        assert(I->second.String() == ent.getId());
        delete pb;
    }

    {
        // Check that adding the value to an argument works.
        Entity ent("1", 1);
        Anonymous arg;
        static const std::string key = "foo";
        Element val;

        PropertyBase * pb = new EntityProperty(* new EntityRef);
        pb->set(Atlas::Message::Element(&ent));

        assert(!arg->hasAttr(key));
        assert(arg->copyAttr(key, val) != 0);

        pb->add(key, arg);

        assert(arg->hasAttr(key));
        assert(arg->copyAttr(key, val) == 0);
        assert(val.isString());
        assert(val.String() == ent.getId());
        delete pb;
    }

    {
        // Check that adding the value to an argument as a hard attribute works
        Entity ent("1", 1);
        Anonymous arg;
        static const std::string key = "id";
        Element val;

        PropertyBase * pb = new EntityProperty(* new EntityRef);
        pb->set(Atlas::Message::Element(&ent));

        assert(!arg->hasAttr(key));
        // Hard coded attribute ID has not been set, so returns false, but
        // copying it gives us the default
        assert(arg->copyAttr(key, val) == 0);
        assert(val.isString());
        assert(val.String() != ent.getId());

        pb->add(key, arg);

        assert(arg->hasAttr(key));
        assert(arg->copyAttr(key, val) == 0);
        assert(val.isString());
        assert(val.String() == ent.getId());
        delete pb;
    }

}

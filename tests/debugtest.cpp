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


#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include "common/debug.h"

#include <Atlas/Message/Element.h>

#include <iostream>

#include <cassert>

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Message::ListType;

int main()
{
    {
        static const bool debug_flag = true;
        bool flag_set = false;
        debug(flag_set = true;);
        assert(flag_set == true);
    }

    {
        static const bool debug_flag = false;
        bool flag_set = false;
        debug(flag_set = true;);
        assert(flag_set == false);
    }

    {
        // Test debug dumping an empty map
        MapType empty;

        debug_dump(empty, std::cout);
    }

    {
        // Test debug dumping some data
        MapType one;
        assert((one["number"] = 1).isInt());

        debug_dump(one, std::cout);
    }

    {
        // Test debug dumping some data
        MapType one;
        assert((one["foo"] = 1.1).isFloat());

        debug_dump(one, std::cout);
    }

    {
        // Test debug dumping some data
        MapType one;
        assert((one["foo"] = (void*)0).isPtr());

        debug_dump(one, std::cout);
    }

    {
        // Test debug dumping some data
        MapType one;
        assert((one["foo"] = "string").isString());

        debug_dump(one, std::cout);
    }

    {
        // Test debug dumping some data
        MapType one;
        assert((one["foo"] = ListType()).isList());

        debug_dump(one, std::cout);
    }

    {
        // Test debug dumping some data
        MapType one;
        assert((one["foo"] = MapType()).isMap());

        debug_dump(one, std::cout);
    }

    {
        // Test debug dumping some data
        MapType one;

        MapType member;
        assert((member["foo"] = 1).isInt());
        assert((member["bar"] = 1.1).isFloat());
        assert((member["baz"] = (void*)0).isPtr());
        assert((member["quz"] = "string").isString());
        assert((member["mim"] = ListType()).isList());
        assert((member["woz"] = MapType()).isMap());
        assert(member.size() == 6);

        assert((one["foo"] = member).isMap());

        debug_dump(one, std::cout);
    }

    {
        // Test debug dumping some data
        MapType one;

        ListType member(6);
        assert((member[0] = 1).isInt());
        assert((member[1] = 1.1).isFloat());
        assert((member[2] = (void*)0).isPtr());
        assert((member[3] = "string").isString());
        assert((member[4] = ListType()).isList());
        assert((member[5] = MapType()).isMap());
        assert(member.size() == 6);

        assert((one["foo"] = member).isList());

        debug_dump(one, std::cout);
    }

    {
        // Test debug dumping some data
        ListType member(6);
        assert((member[0] = 1).isInt());
        assert((member[1] = 1.1).isFloat());
        assert((member[2] = (void*)0).isPtr());
        assert((member[3] = "string").isString());
        assert((member[4] = ListType()).isList());
        assert((member[5] = MapType()).isMap());
        assert(member.size() == 6);

        debug_dump(member, std::cout);
    }

    // FIXME Check the result - should be moslty deterministic
    {
        // Test debug dumping an empty map
        MapType empty;

        std::string out = debug_tostring(empty);
    }

    {
        // Test debug dumping some data
        MapType one;
        assert((one["number"] = 1).isInt());

        std::string out = debug_tostring(one);
    }

    {
        // Test debug dumping some data
        MapType one;
        assert((one["foo"] = 1.1).isFloat());

        std::string out = debug_tostring(one);
    }

    {
        // Test debug dumping some data
        MapType one;
        assert((one["foo"] = (void*)0).isPtr());

        std::string out = debug_tostring(one);
    }

    {
        // Test debug dumping some data
        MapType one;
        assert((one["foo"] = "string").isString());

        std::string out = debug_tostring(one);
    }

    {
        // Test debug dumping some data
        MapType one;
        assert((one["foo"] = ListType()).isList());

        std::string out = debug_tostring(one);
    }

    {
        // Test debug dumping some data
        MapType one;
        assert((one["foo"] = MapType()).isMap());

        std::string out = debug_tostring(one);
    }

    {
        // Test debug dumping some data
        MapType one;

        MapType member;
        assert((member["foo"] = 1).isInt());
        assert((member["bar"] = 1.1).isFloat());
        assert((member["baz"] = (void*)0).isPtr());
        assert((member["quz"] = "string").isString());
        assert((member["mim"] = ListType()).isList());
        assert((member["woz"] = MapType()).isMap());
        assert(member.size() == 6);

        assert((one["foo"] = member).isMap());

        std::string out = debug_tostring(one);
    }

    {
        // Test debug dumping some data
        MapType one;

        ListType member(6);
        assert((member[0] = 1).isInt());
        assert((member[1] = 1.1).isFloat());
        assert((member[2] = (void*)0).isPtr());
        assert((member[3] = "string").isString());
        assert((member[4] = ListType()).isList());
        assert((member[5] = MapType()).isMap());
        assert(member.size() == 6);

        assert((one["foo"] = member).isList());

        std::string out = debug_tostring(one);
    }

    {
        // Test debug dumping some data
        ListType member(6);
        assert((member[0] = 1).isInt());
        assert((member[1] = 1.1).isFloat());
        assert((member[2] = (void*)0).isPtr());
        assert((member[3] = "string").isString());
        assert((member[4] = ListType()).isList());
        assert((member[5] = MapType()).isMap());
        assert(member.size() == 6);

        std::string out = debug_tostring(member);
    }

    return 0;
}

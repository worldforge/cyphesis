// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2003 Alistair Riddoch
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

#include "../IGEntityExerciser.h"
#include "../allOperations.h"

#include "rules/Line.h"

#include <cassert>

using Atlas::Message::MapType;
using Atlas::Message::ListType;

int main()
{
    Line e(1);

    IGEntityExerciser<Line> ee(e);

    // Throw an op of every type at the entity
    ee.runOperations();

    // Subscribe the entity to every class of op
    std::set<std::string> opNames;
    ee.addAllOperations(opNames);

    // Throw an op of every type at the entity again now it is subscribed
    ee.runOperations();

    std::set<std::string> attrNames;
    attrNames.insert("id");
    attrNames.insert("bbox");
    attrNames.insert("contains");
    attrNames.insert("start_intersections");
    attrNames.insert("end_intersections");
    attrNames.insert("coords");

    // Make sure we have all the default attributes
    assert(ee.checkAttributes(attrNames));

    attrNames.insert("test_int");
    attrNames.insert("test_float");
    attrNames.insert("test_list_string");
    attrNames.insert("test_list_int");
    attrNames.insert("test_list_float");
    attrNames.insert("test_map_string");
    attrNames.insert("test_map_int");
    attrNames.insert("test_map_float");

    // Make sure we don't have the test attributes yet
    assert(!ee.checkAttributes(attrNames));

    // Add the test attributes
    e.setAttr("test_int", 1);
    e.setAttr("test_float", 1.f);
    e.setAttr("test_list_string", "test_value");
    e.setAttr("test_list_int", ListType(1, 1));
    e.setAttr("test_list_float", ListType(1, 1.f));
    e.setAttr("test_map_string", ListType(1, "test_value"));
    MapType test_map;
    test_map["test_key"] = 1;
    e.setAttr("test_map_int", test_map);
    test_map["test_key"] = 1.f;
    e.setAttr("test_map_float", test_map);
    test_map["test_key"] = "test_value";
    e.setAttr("test_map_string", test_map);
    
    // Make sure we have the test attributes now
    assert(ee.checkAttributes(attrNames));

    MapType entityAsAtlas;

    // Dump a representation of the entity into an Atlas Message
    e.addToMessage(entityAsAtlas);

    // Make sure we got at least some of it
    assert(entityAsAtlas.size() > 14);

    // Read the contents of the Atlas Message back in
    e.merge(entityAsAtlas);

    return 0;
}

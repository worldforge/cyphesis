// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#include "IGEntityExerciser.h"
#include "allOperations.h"

#include "rulesets/Food.h"

#include <cassert>

using Atlas::Message::MapType;
using Atlas::Message::ListType;

int main()
{
    Food e("testId");

    IGEntityExerciser<Food> ee(e);

    // Throw an op of every type at the entity
    ee.runOperations();

    // Subscribe the entity to every class of op
    std::set<std::string> opNames;
    ee.addAllOperations(opNames);

    // Throw an op of every type at the entity again now it is subscribed
    ee.runOperations();

    std::set<std::string> attrNames;
    attrNames.insert("status");
    attrNames.insert("id");
    attrNames.insert("name");
    attrNames.insert("mass");
    attrNames.insert("bbox");
    attrNames.insert("contains");

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
    e.set("test_int", 1);
    e.set("test_float", 1.f);
    e.set("test_list_string", "test_value");
    e.set("test_list_int", ListType(1, 1));
    e.set("test_list_float", ListType(1, 1.f));
    e.set("test_map_string", ListType(1, "test_value"));
    MapType test_map;
    test_map["test_key"] = 1;
    e.set("test_map_int", test_map);
    test_map["test_key"] = 1.f;
    e.set("test_map_float", test_map);
    test_map["test_key"] = "test_value";
    e.set("test_map_string", test_map);
    
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

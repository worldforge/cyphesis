// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#include "IGEntityExerciser.h"
#include "allOperations.h"

#include "rulesets/World.h"

#include <cassert>

class TestWorld : public BaseWorld {
  public:
    TestWorld(const std::string & id, Entity & gw) : BaseWorld(id, gw) { }

    virtual bool idle() { return false; }
    virtual Entity * addObject(Entity * obj, bool setup = true) { return 0; }
    virtual Entity * addNewObject(const std::string &,
                                  const Atlas::Message::Element::MapType &) {
        return 0;
    }
    virtual void setSerialnoOp(RootOperation &) { }
    virtual OpVector message(RootOperation & op, const Entity * obj) { return OpVector(); }
    virtual Entity * findByName(const std::string & name) { return 0; }
    virtual Entity * findByType(const std::string & type) { return 0; }
};

int main()
{
    World e("testId");
    TestWorld tw("testId", e);
    e.m_world = &tw;

    IGEntityExerciser<World> ee(e);

    // Throw an op of every type at the entity
    ee.runOperations();

    // Subscribe the entity to every class of op
    std::set<std::string> opNames;
    ee.addAllOperations(opNames);
    ee.subscribeOperations(opNames);

    // Throw an op of every type at the entity again now it is subscribed
    ee.runOperations();

    std::set<std::string> attrNames;
    attrNames.insert("status");
    attrNames.insert("id");
    attrNames.insert("name");
    attrNames.insert("mass");
    attrNames.insert("bbox");
    attrNames.insert("contains");
    attrNames.insert("terrain");

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
    e.set("test_list_int", Element::ListType(1, 1));
    e.set("test_list_float", Element::ListType(1, 1.f));
    e.set("test_map_string", Element::ListType(1, "test_value"));
    Element::MapType test_map;
    test_map["test_key"] = 1;
    e.set("test_map_int", test_map);
    test_map["test_key"] = 1.f;
    e.set("test_map_float", test_map);
    test_map["test_key"] = "test_value";
    e.set("test_map_string", test_map);
    
    // Make sure we have the test attributes now
    assert(ee.checkAttributes(attrNames));

    Element::MapType entityAsAtlas;

    // Dump a representation of the entity into an Atlas Message
    e.addToObject(entityAsAtlas);

    // Make sure we got at least some of it
    assert(entityAsAtlas.size() > 14);

    // Read the contents of the Atlas Message back in
    e.merge(entityAsAtlas);

    return 0;
}

// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#include "IGEntityExerciser.h"

#include "rulesets/Entity.h"

#include "common/Chop.h"
#include "common/Cut.h"
#include "common/Eat.h"
#include "common/Burn.h"
#include "common/Generic.h"
#include "common/Nourish.h"
#include "common/Setup.h"
#include "common/Tick.h"

#include <Atlas/Objects/Operation/Appearance.h>
#include <Atlas/Objects/Operation/Combine.h>
#include <Atlas/Objects/Operation/Delete.h>
#include <Atlas/Objects/Operation/Disappearance.h>
#include <Atlas/Objects/Operation/Divide.h>
#include <Atlas/Objects/Operation/Error.h>
#include <Atlas/Objects/Operation/Feel.h>
#include <Atlas/Objects/Operation/Imaginary.h>
#include <Atlas/Objects/Operation/Listen.h>
#include <Atlas/Objects/Operation/Logout.h>
#include <Atlas/Objects/Operation/Look.h>
#include <Atlas/Objects/Operation/Move.h>
#include <Atlas/Objects/Operation/Sound.h>
#include <Atlas/Objects/Operation/Smell.h>
#include <Atlas/Objects/Operation/Sniff.h>
#include <Atlas/Objects/Operation/Talk.h>
#include <Atlas/Objects/Operation/Touch.h>

#include <cassert>

int main()
{
    Entity e("testId");

    IGEntityExerciser<Entity> ee(e);

    ee.runOperations();

    std::set<std::string> opNames;
    ee.addAllOperations(opNames);
    ee.subscribeOperations(opNames);

    ee.runOperations();

    std::set<std::string> attrNames;
    attrNames.insert("status");
    attrNames.insert("id");
    attrNames.insert("name");
    attrNames.insert("mass");
    attrNames.insert("bbox");
    attrNames.insert("contains");

    assert(ee.checkAttributes(attrNames));

    attrNames.insert("test_int");
    attrNames.insert("test_float");
    attrNames.insert("test_list_string");
    attrNames.insert("test_list_int");
    attrNames.insert("test_list_float");
    attrNames.insert("test_map_string");
    attrNames.insert("test_map_int");
    attrNames.insert("test_map_float");

    assert(!ee.checkAttributes(attrNames));

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
    
    assert(ee.checkAttributes(attrNames));

    return 0;
}

// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2008 Alistair Riddoch
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

#include "TestWorld.h"

#include "common/Property.h"

#include "rulesets/World.h"
#include "rulesets/Python_API.h"

#include "server/EntityBuilder.h"

int main(int argc, char ** argv)
{
    loadConfig(argc, argv);
    database_flag = false;

    init_python_api();

    World e("0", 0);
    TestWorld test_world(e);
    EntityBuilder::init(test_world);

    Entity * thing = new Thing("1", 1);

    // Check no flags are set
    assert(thing->getFlags() == 0);
    // Check test attribute is not there
    assert(!thing->hasAttr("test_attr1"));
    // Check test property not there
    assert(thing->getProperty("test_attr1") == 0);
    assert(thing->modProperty("test_attr1") == 0);
    // Set the flag that this entity is clean
    thing->setFlags(entity_clean);
    // Check the flags are no longer clear
    assert(thing->getFlags() != 0);
    // Check the entity_clean flag is set
    assert(thing->getFlags() & entity_clean);
    // Check entity_clean is the only flag set
    assert((thing->getFlags() & ~entity_clean) == 0);
    // Set the flag that this entity is queued
    thing->setFlags(entity_queued);
    // Check the entity_queued flag is set
    assert(thing->getFlags() & entity_queued);
    // Check the entity_clean flag is still set
    assert(thing->getFlags() & entity_clean);
    
    thing->setAttr("test_attr1", 1);

    // Check entity_clean is no longer set
    assert((thing->getFlags() & entity_clean) == 0);
    // Check the attribute is there
    assert(thing->hasAttr("test_attr1"));
    // Check it is there as a property
    assert(thing->getProperty("test_attr1") != 0);
    assert(thing->modProperty("test_attr1") != 0);
    // Check it is there as an integer property
    assert(thing->modPropertyType<int>("test_attr1") != 0);
    // Check it is not available as any other type.
    assert(thing->modPropertyType<double>("test_attr1") == 0);
    assert(thing->modPropertyType<std::string>("test_attr1") == 0);

    Property<int> * int_prop = thing->modPropertyType<int>("test_attr1");
    assert(int_prop != 0);
    // assert(int_prop->flags() & flag_unsent);
    // TODO(alriddoch) make sure flags behave fully correctly, and signals
    // are emitted as required.

    // TODO(alriddoch) add coverage for calling requirePropertyClass() when
    // a property of the wrong type already exists.
}

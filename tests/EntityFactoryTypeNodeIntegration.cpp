// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2011 Alistair Riddoch
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

#include "server/EntityFactory.h"

#include "TestPropertyManager.h"

#include "rulesets/Thing.h"

#include "common/TypeNode.h"

#include <cassert>

int main()
{
    TestPropertyManager tpm;

    EntityFactoryBase * ek;
    ek = new EntityFactory<Thing>;

    ek->m_type = new TypeNode("thing");

    assert(ek->m_type->defaults().empty());

    ek->m_attributes["test1"] = Atlas::Message::StringType("foo");
    ek->m_classAttributes["test1"] = Atlas::Message::StringType("foo");

    assert(ek->m_type->defaults().empty());

    ek->updateProperties();

    assert(ek->m_type->defaults().size() == 1);

    EntityFactoryBase * subclass_ek = new EntityFactory<Thing>;

    subclass_ek->m_type = new TypeNode("subclass");
    subclass_ek->m_type->setParent(ek->m_type);
    ek->m_children.insert(subclass_ek);

    assert(subclass_ek->m_type->defaults().empty());

    subclass_ek->m_attributes["test2"] = Atlas::Message::StringType("bar");
    subclass_ek->m_classAttributes["test2"] = Atlas::Message::StringType("bar");

    assert(subclass_ek->m_type->defaults().empty());

    subclass_ek->updateProperties();

    assert(subclass_ek->m_type->defaults().size() == 1);

    ek->updateProperties();

    assert(ek->m_type->defaults().size() == 1);
    assert(subclass_ek->m_type->defaults().size() == 2);

    subclass_ek->m_attributes["test1"] = Atlas::Message::StringType("bar");
    subclass_ek->m_classAttributes["test1"] = Atlas::Message::StringType("bar");

    ek->updateProperties();

    assert(ek->m_type->defaults().size() == 1);
    assert(subclass_ek->m_type->defaults().size() == 2);

    return 0;
}

// stubs

#include "rulesets/PythonScriptFactory.h"

#include "rulesets/Creator.h"
#include "rulesets/Plant.h"
#include "rulesets/Stackable.h"

Stackable::Stackable(const std::string & id, long intId) :
           Thing(id, intId), m_num(1)
{
    // m_properties["num"] = new Property<int>(m_num, 0);
}

Stackable::~Stackable()
{
}

void Stackable::CombineOperation(const Operation & op, OpVector & res)
{
}

void Stackable::DivideOperation(const Operation & op, OpVector & res)
{
}

Plant::Plant(const std::string & id, long intId) :
       Thing(id, intId)
{
}

Plant::~Plant()
{
}

void Plant::NourishOperation(const Operation & op, OpVector & res)
{
}

void Plant::TickOperation(const Operation & op, OpVector & res)
{
}

void Plant::TouchOperation(const Operation & op, OpVector & res)
{
}

#include "stubs/rulesets/stubCreator.h"
#include "stubs/rulesets/stubCharacter.h"
#include "stubs/rulesets/stubEntity.h"
#include "stubs/rulesets/stubLocatedEntity.h"
#include "stubs/rulesets/stubThing.h"
#include "stubs/common/stubRouter.h"
#include "stubs/modules/stubLocation.h"

void log(LogLevel lvl, const std::string & msg)
{
}

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

#include "../TestPropertyManager.h"

#include "rules/simulation/Thing.h"

#include "common/TypeNode.h"

#include <cassert>

int main()
{
    TestPropertyManager tpm;
    std::map<const TypeNode*, TypeNode::PropertiesUpdate> changes;

    TypeNode typeNode("thing");
    EntityFactory<Thing> ek;

    ek.m_type = &typeNode;

    assert(ek.m_type->defaults().empty());

    ek.m_attributes["test1"] = Atlas::Message::StringType("foo");
    ek.m_classAttributes["test1"] = {Atlas::Message::StringType("foo")};

    assert(ek.m_type->defaults().empty());

    ek.updateProperties(changes, tpm);

    assert(ek.m_type->defaults().size() == 1);

    TypeNode subType("subclass");
    EntityFactory<Thing> subclass_ek;
    subclass_ek.m_type = &subType;
    subclass_ek.m_type->setParent(ek.m_type);
    ek.m_children.insert(&subclass_ek);

    assert(subclass_ek.m_type->defaults().empty());

    subclass_ek.m_attributes["test2"] = Atlas::Message::StringType("bar");
    subclass_ek.m_classAttributes["test2"] = {Atlas::Message::StringType("bar")};

    assert(subclass_ek.m_type->defaults().empty());

    subclass_ek.updateProperties(changes, tpm);

    assert(subclass_ek.m_type->defaults().size() == 1);

    ek.updateProperties(changes, tpm);

    assert(ek.m_type->defaults().size() == 1);
    assert(subclass_ek.m_type->defaults().size() == 2);

    subclass_ek.m_attributes["test1"] = Atlas::Message::StringType("bar");
    subclass_ek.m_classAttributes["test1"] = {Atlas::Message::StringType("bar")};

    ek.updateProperties(changes, tpm);

    assert(ek.m_type->defaults().size() == 1);
    assert(subclass_ek.m_type->defaults().size() == 2);

    return 0;
}

// stubs

#include "rules/python/PythonScriptFactory.h"

#include "../stubs/rules/simulation/stubEntity.h"
#include "../stubs/rules/stubLocatedEntity.h"
#include "../stubs/rules/simulation/stubThing.h"
#include "../stubs/common/stubRouter.h"
#include "../stubs/rules/stubLocation.h"
#include "../stubs/common/stublog.h"

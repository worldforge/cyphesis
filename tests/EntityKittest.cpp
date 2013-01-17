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

// $Id$

#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include "common/EntityKit.h"

#include "common/ScriptKit.h"

#include <Atlas/Message/Element.h>

#include <cassert>

using Atlas::Message::MapType;

class TestEntityKit : public EntityKit
{
  public:
    virtual ~TestEntityKit() { }

    virtual LocatedEntity * newEntity(const std::string & id, long intId) { return 0; }

    virtual EntityKit * duplicateFactory() { return 0; }
};

class TestScriptKit : public ScriptKit<LocatedEntity>
{
  public:
    std::string m_package;
    virtual const std::string & package() const { return m_package; }
    virtual int addScript(LocatedEntity * entity) const { return 0; }
    virtual int refreshClass() { return 0; }
};

int main()
{
    {
        EntityKit * ek = new TestEntityKit;

        delete ek;
    }

    {
        EntityKit * ek = new TestEntityKit;

        ek->m_scriptFactory = new TestScriptKit;

        delete ek;
    }

    {
        EntityKit * ek = new TestEntityKit;

        ek->addProperties();

        delete ek;
    }

    {
        EntityKit * ek = new TestEntityKit;

        ek->updateProperties();

        delete ek;
    }

    {
        EntityKit * ek = new TestEntityKit;

        EntityKit * ekc = new TestEntityKit;
        ekc->m_classAttributes.insert(std::make_pair("foo", "value"));

        ek->m_children.insert(ekc);

        ek->updateProperties();

        assert(ekc->m_attributes.find("foo") != ekc->m_attributes.end());

        delete ek;
    }

    return 0;
}

// stubs

#include "common/TypeNode.h"

void TypeNode::addProperties(const MapType & attributes)
{
}

void TypeNode::updateProperties(const MapType & attributes)
{
}

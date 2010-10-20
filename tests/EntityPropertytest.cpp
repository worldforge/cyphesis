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

// $Id$

#include "rulesets/EntityProperty.h"

#include "rulesets/Entity.h"

#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/RootOperation.h>

#include <cassert>

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Objects::Entity::Anonymous;

int main()
{

    {
        // Test constructor
        PropertyBase * pb = new EntityProperty();
        delete pb;
    }

    {
        // Check constructor has set flags correctly to zero
        PropertyBase * pb = new EntityProperty;
        assert(pb->flags() == 0);
        delete pb;
    }

    {
        // Check getting the value fails when property is unassigned
        Element val;

        PropertyBase * pb = new EntityProperty;
        assert(pb->get(val) == false);
        delete pb;
    }

    {
        // Check that setting the value to a pointer works
        Entity ent("1", 1);

        PropertyBase * pb = new EntityProperty;
        pb->set(Atlas::Message::Element(&ent));
        delete pb;
    }

    {
        // Check that setting the value to a pointer works can get retrieved
        Entity ent("1", 1);
        Element val;

        PropertyBase * pb = new EntityProperty;
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

        PropertyBase * pb = new EntityProperty;

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

        PropertyBase * pb = new EntityProperty;

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

        PropertyBase * pb = new EntityProperty;

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

        PropertyBase * pb = new EntityProperty;
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

        PropertyBase * pb = new EntityProperty;
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

        PropertyBase * pb = new EntityProperty;
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

// stubs

Entity::Entity(const std::string & id, long intId) :
        LocatedEntity(id, intId), m_motion(0), m_flags(0)
{
}

Entity::~Entity()
{
}

void Entity::destroy()
{
    destroyed.emit();
}

void Entity::ActuateOperation(const Operation &, OpVector &)
{
}

void Entity::AppearanceOperation(const Operation &, OpVector &)
{
}

void Entity::AttackOperation(const Operation &, OpVector &)
{
}

void Entity::CombineOperation(const Operation &, OpVector &)
{
}

void Entity::CreateOperation(const Operation &, OpVector &)
{
}

void Entity::DeleteOperation(const Operation &, OpVector &)
{
}

void Entity::DisappearanceOperation(const Operation &, OpVector &)
{
}

void Entity::DivideOperation(const Operation &, OpVector &)
{
}

void Entity::EatOperation(const Operation &, OpVector &)
{
}

void Entity::ImaginaryOperation(const Operation &, OpVector &)
{
}

void Entity::LookOperation(const Operation &, OpVector &)
{
}

void Entity::MoveOperation(const Operation &, OpVector &)
{
}

void Entity::NourishOperation(const Operation &, OpVector &)
{
}

void Entity::SetOperation(const Operation &, OpVector &)
{
}

void Entity::SightOperation(const Operation &, OpVector &)
{
}

void Entity::SoundOperation(const Operation &, OpVector &)
{
}

void Entity::TalkOperation(const Operation &, OpVector &)
{
}

void Entity::TickOperation(const Operation &, OpVector &)
{
}

void Entity::TouchOperation(const Operation &, OpVector &)
{
}

void Entity::UpdateOperation(const Operation &, OpVector &)
{
}

void Entity::UseOperation(const Operation &, OpVector &)
{
}

void Entity::WieldOperation(const Operation &, OpVector &)
{
}

void Entity::externalOperation(const Operation & op)
{
}

void Entity::operation(const Operation & op, OpVector & res)
{
}

void Entity::addToMessage(Atlas::Message::MapType & omap) const
{
}

void Entity::addToEntity(const Atlas::Objects::Entity::RootEntity & ent) const
{
}

void Entity::setAttr(const std::string & name,
                     const Atlas::Message::Element & attr)
{
}

const PropertyBase * Entity::getProperty(const std::string & name) const
{
    return 0;
}

void Entity::onContainered()
{
}

void Entity::onUpdated()
{
}

LocatedEntity::LocatedEntity(const std::string & id, long intId) :
               Router(id, intId),
               m_refCount(0), m_seq(0),
               m_script(0), m_type(0), m_contains(0)
{
}

LocatedEntity::~LocatedEntity()
{
}

bool LocatedEntity::hasAttr(const std::string & name) const
{
    return false;
}

bool LocatedEntity::getAttr(const std::string & name, Atlas::Message::Element & attr) const
{
    return false;
}

bool LocatedEntity::getAttrType(const std::string & name,
                                Atlas::Message::Element & attr,
                                int type) const
{
    return false;
}

void LocatedEntity::setAttr(const std::string & name, const Atlas::Message::Element & attr)
{
    return;
}

const PropertyBase * LocatedEntity::getProperty(const std::string & name) const
{
    return 0;
}

void LocatedEntity::onContainered()
{
}

void LocatedEntity::onUpdated()
{
}

Router::Router(const std::string & id, long intId) : m_id(id),
                                                             m_intId(intId)
{
}

Router::~Router()
{
}

void Router::addToMessage(Atlas::Message::MapType & omap) const
{
}

void Router::addToEntity(const Atlas::Objects::Entity::RootEntity & ent) const
{
}

Location::Location() : m_loc(0)
{
}

BaseWorld * BaseWorld::m_instance = 0;

BaseWorld::BaseWorld(Entity & gw) : m_gameWorld(gw)
{
    m_instance = this;
}

BaseWorld::~BaseWorld()
{
    m_instance = 0;
}

Entity * BaseWorld::getEntity(const std::string & id) const
{
    return 0;
}

Entity * BaseWorld::getEntity(long id) const
{
    return 0;
}

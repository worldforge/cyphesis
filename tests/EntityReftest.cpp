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

#include "modules/EntityRef.h"

#include "rulesets/Entity.h"

#include <sigc++/functors/ptr_fun.h>

#include <cassert>

static bool emitted;

static void signal_emitted()
{
    emitted = true;
}

void checkSignal()
{
    {
        // Check the assignment operator causes the signal to fire
        emitted = false;

        Entity e("1", 1);
        EntityRef ref;

        assert(emitted == false);

        ref.Changed.connect(sigc::ptr_fun(&signal_emitted));

        assert(emitted == false);

        ref = EntityRef(&e);

        assert(ref.get() == &e);
        assert(emitted == true);
    }

    {
        // Check the assignment operator does not cause the signal to fire
        // the the pointer is unchanged
        emitted = false;

        Entity e("1", 1);
        EntityRef ref(&e);

        assert(emitted == false);

        ref.Changed.connect(sigc::ptr_fun(&signal_emitted));

        assert(emitted == false);

        ref = EntityRef(&e);

        assert(ref.get() == &e);
        assert(emitted == false);
    }

    {
        // Check that destroying the Entity makes the reference null.
        emitted = false;

        Entity e("1", 1);
        Entity * container = new Entity("2", 2);

        // Set the location of the entity being tested, as destroy requires it.
        e.m_location.m_loc = container;
        // Make sure the container has a contains structure, as destroy
        // requires it.
        container->m_contains = new LocatedEntitySet;
        // Increment the refcount on the container, else the tested Entity's
        // destructor will delete it.
        container->incRef();

        EntityRef ref(&e);

        assert(emitted == false);

        ref.Changed.connect(sigc::ptr_fun(&signal_emitted));

        assert(ref.get() == &e);
        assert(emitted == false);

        e.destroy();

        assert(ref.get() == 0);
        assert(emitted == true);
    }
}

int main()
{
    {
        // Check the default constructor
        EntityRef ref;
    }

    {
        // Check the default constructor initialises to NULL via get
        EntityRef ref;

        assert(ref.get() == 0);
    }

    {
        // Check the default constructor initialises to NULL via dereference
        EntityRef ref;

        assert(&(*ref) == 0);
    }

    {
        // Check the default constructor initialises to NULL via ->
        EntityRef ref;

        assert(ref.operator->() == 0);
    }

    {
        // Check the default constructor initialises to NULL via ==
        EntityRef ref;

        assert(ref == 0);
    }

    {
        // Check the initialising constructor via get
        Entity * e = new Entity("1", 1);
        EntityRef ref(e);

        assert(ref.get() == e);
    }

    {
        // Check the initialising constructor via dereference
        Entity * e = new Entity("1", 1);
        EntityRef ref(e);

        assert(&(*ref) == e);
    }

    {
        // Check the initialising constructor via ->
        Entity * e = new Entity("1", 1);
        EntityRef ref(e);

        assert(ref.operator->() == e);
    }

    {
        // Check the initialising constructor via ==
        Entity * e = new Entity("1", 1);
        EntityRef ref(e);

        assert(ref == e);
    }

    {
        // Check the copy constructor
        Entity * e = new Entity("1", 1);
        EntityRef ref(e);
        EntityRef ref2(ref);

        assert(ref2.get() == e);
    }

    {
        // Check the comparison operator
        Entity * e = new Entity("1", 1);
        EntityRef ref(e);
        EntityRef ref2(e);

        assert(ref == ref2);
    }

    {
        // Check the comparison operator
        Entity * e = new Entity("1", 1);
        Entity * e2 = new Entity("2", 2);
        EntityRef ref(e);
        EntityRef ref2(e2);

        assert(!(ref == ref2));
    }

#if 0
    // These tests should be included should we add operator!=
    {
        // Check the comparison operator
        Entity e("1", 1);
        EntityRef ref(&e);
        EntityRef ref2(&e);

        assert(!(ref != ref2));
    }

    {
        // Check the comparison operator
        Entity e("1", 1);
        Entity e2("2", 2);
        EntityRef ref(&e);
        EntityRef ref2(&e2);

        assert(ref != ref2);
    }
#endif

    {
        // Check the less than operator
        Entity * e = new Entity("1", 1);
        EntityRef ref(e);
        EntityRef ref2(e);

        assert(!(ref < ref2) && !(ref2 < ref));
    }

    {
        // Check the less than operator
        Entity * e = new Entity("1", 1);
        Entity * e2 = new Entity("2", 2);
        EntityRef ref(e);
        EntityRef ref2(e2);

        assert(ref < ref2 || ref2 < ref);
    }

    {
        // Check the assignment operator
        Entity * e = new Entity("1", 1);
        EntityRef ref;

        ref = EntityRef(e);

        assert(ref.get() == e);
    }

    {
        // Check that destroying the Entity makes the reference null.
        Entity e("1", 1);
        Entity * container = new Entity("2", 2);

        // Set the location of the entity being tested, as destroy requires it.
        e.m_location.m_loc = container;
        // Make sure the container has a contains structure, as destroy
        // requires it.
        container->m_contains = new LocatedEntitySet;
        // Increment the refcount on the container, else the tested Entity's
        // destructor will delete it.
        container->incRef();

        EntityRef ref(&e);

        assert(ref.get() == &e);
        e.destroy();
        assert(ref.get() == 0);
    }

    checkSignal();
}

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

Location::Location()
{
}

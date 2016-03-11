// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2010 Alistair Riddoch
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


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "rulesets/PhysicalDomain.h"

#include "rulesets/TerrainProperty.h"

#include <bullet/btBulletDynamicsCommon.h>.h"

#include "stubs/rulesets/stubTerrainProperty.h"
#include "stubs/common/stubTypeNode.h"

#include <cassert>

class TestBulletDomain : public PhysicalDomain
{
  public:
    btDynamicsWorld * test_getDynamicsWorld() const
    {
        return m_dynamicsWorld;
    }
};


int main()
{
#ifdef HAVE_BULLET
    {
        BulletDomain * bd = new BulletDomain;
        delete bd;
    }

    {
        BulletDomain * bd = new BulletDomain;
        delete bd;
    }

    {
        TestBulletDomain * bd = new TestBulletDomain;
        assert(bd->test_getCollisionWorld() != 0);

        btCollisionObject * obj = new btCollisionObject;

        btMatrix3x3 basis;
        basis.setIdentity();
        obj->getWorldTransform().setBasis(basis);

        btBoxShape* box = new btBoxShape(btVector3(1,1,1));
        obj->setCollisionShape(box);

        bd->test_getCollisionWorld()->addCollisionObject(obj);

        delete bd;
    }
#endif // HAVE_BULLET

    return 0;
}

// stubs

PropertyBase::PropertyBase(unsigned int flags) : m_flags(flags)
{
}

PropertyBase::~PropertyBase()
{
}

void PropertyBase::install(LocatedEntity *, const std::string & name)
{
}

void PropertyBase::remove(LocatedEntity *, const std::string & name)
{
}

void PropertyBase::apply(LocatedEntity *)
{
}

void PropertyBase::add(const std::string & s,
                       Atlas::Message::MapType & ent) const
{
    get(ent[s]);
}

void PropertyBase::add(const std::string & s,
                       const Atlas::Objects::Entity::RootEntity & ent) const
{
}

HandlerResult PropertyBase::operation(LocatedEntity *,
                                      const Operation &,
                                      OpVector &)
{
    return OPERATION_IGNORED;
}

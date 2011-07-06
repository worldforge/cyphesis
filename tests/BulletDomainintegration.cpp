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

// $Id$

#include "rulesets/BulletDomain.h"

#include "rulesets/TerrainProperty.h"

#include "btBulletCollisionCommon.h"

#include <cassert>

class TestBulletDomain : public BulletDomain
{
  public:
    btCollisionWorld * test_getCollisionWorld() const
    {
        return m_collisionWorld;
    }
};

int main()
{
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

    return 0;
}

// stubs

TerrainProperty::TerrainProperty() :
    m_data(*(Mercator::Terrain*)0),
    m_tileShader(*(Mercator::TileShader*)0)
{
}

TerrainProperty::~TerrainProperty()
{
}

bool TerrainProperty::get(Atlas::Message::Element & ent) const
{
    return true;
}

void TerrainProperty::set(const Atlas::Message::Element & ent)
{
}

bool TerrainProperty::getHeightAndNormal(float x,
                                         float y,
                                         float & height,
                                         Vector3D & normal) const
{
    return true;
}

PropertyBase::PropertyBase(unsigned int flags) : m_flags(flags)
{
}

PropertyBase::~PropertyBase()
{
}

void PropertyBase::install(Entity *)
{
}

void PropertyBase::apply(Entity *)
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

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


#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include "../TestWorld.h"
#include "../TestPropertyManager.h"

#include "common/Property.h"

#include "rules/simulation/World.h"


int main(int argc, char ** argv)
{
    TestPropertyManager tpm;
    // database_flag = false;

    Ref<Entity>  thing(new Thing(1));

    // Check no flags are set
    assert(thing->flags().m_flags == 0);
    // Check test attribute is not there
    assert(!thing->hasAttr("test_attr1"));
    // Check test property not there
    assert(thing->getProperty("test_attr1") == 0);
    assert(thing->modProperty("test_attr1") == 0);
    // Set the flag that this entity is clean
    thing->addFlags(entity_clean);
    // Check the flags are no longer clear
    assert(thing->flags().m_flags != 0);
    // Check the entity_clean flag is set
    assert(thing->flags().m_flags & entity_clean);
    // Check entity_clean is the only flag set
    assert((thing->flags().m_flags & ~entity_clean) == 0);
    // Set the flag that this entity is queued
    thing->addFlags(entity_queued);
    // Check the entity_queued flag is set
    assert(thing->flags().m_flags & entity_queued);
    // Check the entity_clean flag is still set
    assert(thing->flags().m_flags & entity_clean);

    thing->setAttrValue("test_attr1", 1);

    // Check entity_clean is no longer set
    assert((thing->flags().m_flags & entity_clean) == 0);
    // Check the attribute is there
    assert(thing->hasAttr("test_attr1"));
    // Check it is there as a property
    assert(thing->getProperty("test_attr1") != 0);
    assert(thing->modProperty("test_attr1") != 0);
    Atlas::Message::Element val;
    assert(thing->getProperty("test_attr1")->get(val) == 0);
    assert(val.isInt());
    // Check it is not available as any other type as it was installed as a soft property.
    assert(!thing->modPropertyType<int>("test_attr1"));
    assert(!thing->modPropertyType<double>("test_attr1"));
    assert(!thing->modPropertyType<std::string>("test_attr1"));

}

// stubs



#include "rules/AtlasProperties.h"
#include "rules/Domain.h"
#include "rules/simulation/DomainProperty.h"

#include "common/const.h"
#include "common/log.h"

using Atlas::Objects::Entity::RootEntity;

#include "../stubs/common/stubcustom.h"
#include "../stubs/rules/stubAtlasProperties.h"
#include "../stubs/rules/simulation/stubDomainProperty.h"
#include "../stubs/common/stubMonitors.h"
#include "../stubs/common/stubVariable.h"
#include "../stubs/rules/simulation/stubPropelProperty.h"
#include "../stubs/rules/simulation/stubEntityProperty.h"
#include "../stubs/rules/simulation/stubModeDataProperty.h"



#include "../stubs/common/stubRouter.h"
#include "../stubs/common/stubLink.h"
#include "../stubs/modules/stubWeakEntityRef.h"
#include "../stubs/rules/stubLocation.h"
#include "../stubs/rules/stubAtlasProperties.h"
#include "../stubs/rules/stubPhysicalProperties.h"
#include "../stubs/rules/simulation/stubBaseWorld.h"
#include "../stubs/common/stubPropertyManager.h"
#include "../stubs/common/stubTypeNode.h"

void addToEntity(const Point3D & p, std::vector<double> & vd)
{
}

WFMath::CoordType squareDistance(const Point3D & u, const Point3D & v)
{
    return 1.f;
}

template <typename FloatT>
int fromStdVector(Point3D & p, const std::vector<FloatT> & vf)
{
    if (vf.size() != 3) {
        return -1;
    }
    p[0] = vf[0];
    p[1] = vf[1];
    p[2] = vf[2];
    p.setValid();
    return 0;
}

template <>
int fromStdVector<double>(Point3D & p, const std::vector<double> & vf)
{
    return 0;
}

template <>
int fromStdVector<double>(Vector3D & v, const std::vector<double> & vf)
{
    return 0;
}

#include "../stubs/rules/stubModifier.h"
#include "../stubs/common/stublog.h"

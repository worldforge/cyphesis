// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2003 Alistair Riddoch
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

#include "IGEntityExerciser.h"
#include "allOperations.h"

#include "rulesets/Plant.h"

#include "rulesets/AtlasProperties.h"
#include "rulesets/AreaProperty.h"
#include "rulesets/BBoxProperty.h"
#include "rulesets/Domain.h"
#include "rulesets/Script.h"
#include "rulesets/StatusProperty.h"

#include "common/id.h"
#include "common/log.h"
#include "common/Property_impl.h"
#include "common/TypeNode.h"

#include "stubs/common/stubCustom.h"
#include "stubs/common/stubRouter.h"
#include "stubs/common/stubTypeNode.h"
#include "stubs/rulesets/stubLocation.h"
#include "stubs/rulesets/stubThing.h"
#include "stubs/rulesets/stubEntity.h"

#define STUB_LocatedEntity_getAttr
int LocatedEntity::getAttr(const std::string & name,
                           Atlas::Message::Element & attr) const
{
    return -1;
}

#define STUB_LocatedEntity_getAttrType
int LocatedEntity::getAttrType(const std::string & name,
                               Atlas::Message::Element & attr,
                               int type) const
{
    return -1;
}

#include "stubs/rulesets/stubLocatedEntity.h"
#include "stubs/rulesets/stubIdProperty.h"
#include "stubs/common/stubProperty.h"
#include "stubs/common/stubPropertyManager.h"
#include "stubs/rulesets/stubContainsProperty.h"
#include "stubs/rulesets/stubStatusProperty.h"
#include "stubs/rulesets/stubBBoxProperty.h"
#include "stubs/rulesets/stubAreaProperty.h"
#include "stubs/rulesets/stubTerrainEffectorProperty.h"
#include "stubs/rulesets/stubDomain.h"
#include "stubs/rulesets/stubVector3Property.h"
#include "stubs/rulesets/stubBiomassProperty.h"



#include <cstdlib>

#include <cassert>

using Atlas::Message::Element;
using Atlas::Message::ListType;
using Atlas::Message::MapType;
using Atlas::Objects::Entity::RootEntity;

int main()
{
    Ref<Plant> e(new Plant("1", 1));
    TypeNode type("plant");
    e->setType(&type);

    IGEntityExerciser ee(e);

    // Throw an op of every type at the entity
    ee.runOperations();

    // Subscribe the entity to every class of op
    std::set<std::string> opNames;
    ee.addAllOperations(opNames);

    // Throw an op of every type at the entity again now it is subscribed
    ee.runOperations();

    return 0;
}

// stubs

#include "stubs/rulesets/stubScaleProperty.h"

void addToEntity(const Point3D & p, std::vector<double> & vd)
{
    vd.resize(3);
    vd[0] = p[0];
    vd[1] = p[1];
    vd[2] = p[2];
}


#ifndef STUB_BaseWorld_getEntity
#define STUB_BaseWorld_getEntity
Ref<LocatedEntity> BaseWorld::getEntity(const std::string & id) const
{
    return getEntity(integerId(id));
}

Ref<LocatedEntity> BaseWorld::getEntity(long id) const
{
    auto I = m_eobjects.find(id);
    if (I != m_eobjects.end()) {
        assert(I->second);
        return I->second;
    } else {
        return nullptr;
    }
}
#endif //STUB_BaseWorld_getEntity

#include "stubs/rulesets/stubBaseWorld.h"


#include "stubs/rulesets/stubScript.h"


void log(LogLevel lvl, const std::string & msg)
{
}

long integerId(const std::string & id)
{
    long intId = strtol(id.c_str(), 0, 10);
    if (intId == 0 && id != "0") {
        intId = -1L;
    }

    return intId;
}

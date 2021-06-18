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

#include "../IGEntityExerciser.h"
#include "../allOperations.h"
#include "../TestWorld.h"

#include "rules/simulation/World.h"

#include "rules/AtlasProperties.h"
#include "rules/BBoxProperty.h"
#include "rules/simulation/CalendarProperty.h"
#include "rules/Domain.h"
#include "rules/Script.h"
#include "rules/simulation/StatusProperty.h"
#include "rules/simulation/TerrainProperty.h"

#include "common/id.h"
#include "common/Inheritance.h"
#include "common/log.h"
#include "common/Property_impl.h"
#include "common/TypeNode.h"

#include "../stubs/common/stubcustom.h"
#include "../stubs/common/stubRouter.h"
#include "../stubs/common/stubTypeNode.h"
#include "../stubs/common/stubPropertyManager.h"
#include "../stubs/rules/stubLocation.h"
#include "../stubs/rules/simulation/stubThing.h"
#include "../stubs/rules/simulation/stubEntity.h"
#include "../stubs/rules/stubLocatedEntity.h"
#include "../stubs/rules/stubAtlasProperties.h"
#include "../stubs/common/stubProperty.h"
#include "../stubs/rules/simulation/stubStatusProperty.h"
#include "../stubs/rules/stubBBoxProperty.h"
#include "../stubs/rules/simulation/stubTerrainProperty.h"
#include "../stubs/rules/simulation/stubCalendarProperty.h"
#include "../stubs/rules/stubDomain.h"


#include <cstdlib>

#include <cassert>

using Atlas::Message::Element;
using Atlas::Message::ListType;
using Atlas::Message::MapType;
using Atlas::Objects::Entity::RootEntity;

int main()
{
    Ref<World> e(new World());
    TypeNode type("world");
    e->setType(&type);

    IGEntityExerciser ee(e);

    assert(e->m_parent == nullptr);

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

#include "../stubs/rules/simulation/stubBaseWorld.h"
#include "../stubs/rules/simulation/stubWorldTimeProperty.h"
#include "../stubs/rules/simulation/stubVoidDomain.h"
#include "../stubs/common/stubconst.h"
#include "../stubs/common/stublog.h"
#include "../stubs/common/stubid.h"
#include "../stubs/rules/stubPhysicalProperties.h"



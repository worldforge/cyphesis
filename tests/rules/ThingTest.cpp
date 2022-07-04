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

#include "rules/simulation/Thing.h"

#include "rules/Domain.h"

#include "common/const.h"
#include "common/id.h"
#include "common/log.h"
#include "common/Property_impl.h"
#include "common/TypeNode.h"


#include "../stubs/common/stubcustom.h"
#include "../stubs/common/stubRouter.h"
#include "../stubs/common/stubTypeNode.h"
#include "../stubs/common/stubPropertyManager.h"
#include "../stubs/rules/stubLocation.h"
#include "../stubs/common/stubProperty.h"
#include "../stubs/rules/stubDomain.h"
#include "../stubs/rules/simulation/stubPropelProperty.h"
#include "../stubs/rules/simulation/stubModeDataProperty.h"
#include "../TestDomain.h"


#include <cstdlib>

#include <cassert>

using Atlas::Message::Element;
using Atlas::Message::ListType;
using Atlas::Message::MapType;
using Atlas::Objects::Entity::RootEntity;

int main()
{
    Ref<Thing> e(new Thing(1));
    TypeNode type("thing");
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

//stubs



#include "../stubs/rules/simulation/stubEntity.h"
#include "../stubs/rules/simulation/stubEntityProperty.h"


#define STUB_LocatedEntity_isVisibleForOtherEntity
bool LocatedEntity::isVisibleForOtherEntity(const LocatedEntity& watcher) const
{
    return true;
}

#define STUB_LocatedEntity_makeContainer
void LocatedEntity::makeContainer()
{
    if (m_contains == 0) {
        m_contains.reset(new LocatedEntitySet);
    }
}

#define STUB_LocatedEntity_changeContainer
void LocatedEntity::changeContainer(const Ref<LocatedEntity>& new_loc)
{
    assert(m_parent != nullptr);
    assert(m_parent->m_contains != nullptr);
    m_parent->m_contains->erase(this);
    if (m_parent->m_contains->empty()) {
        m_parent->onUpdated();
    }
    new_loc->makeContainer();
    bool was_empty = new_loc->m_contains->empty();
    new_loc->m_contains->insert(this);
    if (was_empty) {
        new_loc->onUpdated();
    }
    assert(m_parent->checkRef() > 0);
    auto oldLoc = m_parent;
    m_parent = new_loc.get();
    assert(m_parent->checkRef() > 0);

    onContainered(oldLoc);
}

#define STUB_LocatedEntity_broadcast
void LocatedEntity::broadcast(const Atlas::Objects::Operation::RootOperation& op, OpVector& res, Visibility visibility) const
{
    auto copy = op.copy();
    copy->setTo(getId());
    res.push_back(copy);
}

#include "../stubs/rules/stubLocatedEntity.h"


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
#include "../stubs/rules/simulation/stubModeDataProperty.h"
#include "../stubs/modules/stubWeakEntityRef.h"
#include "../stubs/common/stublog.h"
#include "../stubs/common/stubid.h"

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

template <typename FloatT>
int fromStdVector(Vector3D & v, const std::vector<FloatT> & vf)
{
    if (vf.size() != 3) {
        return -1;
    }
    v[0] = vf[0];
    v[1] = vf[1];
    v[2] = vf[2];
    v.setValid();
    return 0;
}

template int fromStdVector<double>(Point3D & p, const std::vector<double> & vf);
template int fromStdVector<double>(Vector3D & v, const std::vector<double> & vf);


WFMath::CoordType squareDistance(const Point3D & u, const Point3D & v)
{
    return 1.0;
}
#include "../stubs/rules/stubPhysicalProperties.h"


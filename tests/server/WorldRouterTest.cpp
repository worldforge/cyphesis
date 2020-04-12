// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2009 Alistair Riddoch
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

#include "../TestBase.h"

#include "server/WorldRouter.h"

#include "server/EntityBuilder.h"
#include "rules/simulation/SpawnEntity.h"

#include "rules/Domain.h"
#include "rules/simulation/World.h"

#include "common/const.h"
#include "common/globals.h"
#include "common/id.h"
#include "common/Inheritance.h"
#include "common/log.h"
#include "common/Monitors.h"
#include "common/SystemTime.h"
#include "common/operations/Tick.h"
#include "common/Variable.h"

#include <Atlas/Objects/Anonymous.h>

#include <cstdio>
#include <cstdlib>

#include <cassert>

using Atlas::Message::MapType;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Operation::Tick;

static bool stub_deny_newid = false;
Atlas::Objects::Factories factories;

class WorldRoutertest : public Cyphesis::TestBase
{
    WorldRouter * test_world;
        EntityBuilder* m_eb;
  public:
    WorldRoutertest();

    void setup();
    void teardown();

    void test_constructor();
    void test_addNewEntity_unknown();
    void test_addNewEntity_thing();
    void test_addNewEntity_idfail();
    void test_addEntity();
    void test_addEntity_tick();
    void test_addEntity_tick_get();
    void test_spawnNewEntity_unknown();
    void test_spawnNewEntity_thing();
    void test_createSpawnPoint();
    void test_delEntity();
    void test_delEntity_world();

        Inheritance* m_inheritance;
        Ref<LocatedEntity> m_rootEntity;
};

WorldRoutertest::WorldRoutertest()
{
    ADD_TEST(WorldRoutertest::test_addNewEntity_thing);
    ADD_TEST(WorldRoutertest::test_constructor);
    ADD_TEST(WorldRoutertest::test_addNewEntity_unknown);
    ADD_TEST(WorldRoutertest::test_addNewEntity_idfail);
    ADD_TEST(WorldRoutertest::test_addEntity);
    ADD_TEST(WorldRoutertest::test_addEntity_tick);
    ADD_TEST(WorldRoutertest::test_addEntity_tick_get);
    ADD_TEST(WorldRoutertest::test_spawnNewEntity_unknown);
    ADD_TEST(WorldRoutertest::test_spawnNewEntity_thing);
    ADD_TEST(WorldRoutertest::test_createSpawnPoint);
    ADD_TEST(WorldRoutertest::test_delEntity);
    ADD_TEST(WorldRoutertest::test_delEntity_world);
}

void WorldRoutertest::setup()
{
    m_rootEntity = new Entity("", 0);
    m_inheritance = new Inheritance(factories);
    m_eb = new EntityBuilder();
    test_world = new WorldRouter(m_rootEntity, *m_eb);
}

void WorldRoutertest::teardown()
{
    m_rootEntity = nullptr;
    delete m_eb;
    delete test_world;
    delete m_inheritance;
}

void WorldRoutertest::test_constructor()
{
}

void WorldRoutertest::test_addNewEntity_unknown()
{

    auto ent1 = test_world->addNewEntity("__no_such_type__",
                                                    Anonymous());
    assert(ent1.get() == 0);
}

void WorldRoutertest::test_addNewEntity_thing()
{
    auto ent1 = test_world->addNewEntity("thing", Anonymous());
    assert(ent1.get() != 0);
}


void WorldRoutertest::test_addNewEntity_idfail()
{
    stub_deny_newid = true;

    auto ent1 = test_world->addNewEntity("thing", Anonymous());
    assert(ent1.get() == 0);

    stub_deny_newid = false;
}

void WorldRoutertest::test_addEntity()
{
    std::string id;
    long int_id = newId(id);

    auto ent2 = new Entity(id, int_id);
    assert(ent2 != 0);
    ent2->m_location.m_parent = m_rootEntity;
    ent2->m_location.m_pos = Point3D(0,0,0);
    test_world->addEntity(ent2);
}

void WorldRoutertest::test_addEntity_tick()
{
    std::string id;
    long int_id = newId(id);

    auto ent2 = new Entity(id, int_id);
    assert(ent2 != 0);
    ent2->m_location.m_parent = m_rootEntity;
    ent2->m_location.m_pos = Point3D(0,0,0);
    test_world->addEntity(ent2);

    Tick tick;
    tick->setFutureSeconds(0);
    tick->setTo(ent2->getId());
    test_world->message(tick, *ent2);
}


void WorldRoutertest::test_addEntity_tick_get()
{
    std::string id;
    long int_id = newId(id);

    auto  ent2 = new Entity(id, int_id);
    assert(ent2 != 0);
    ent2->m_location.m_parent = m_rootEntity;
    ent2->m_location.m_pos = Point3D(0,0,0);
    test_world->addEntity(ent2);

    Tick tick;
    tick->setFutureSeconds(0);
    tick->setTo(ent2->getId());
    test_world->message(tick, *ent2);

}

void WorldRoutertest::test_spawnNewEntity_unknown()
{
    auto ent3 = test_world->spawnNewEntity("__no_spawn__",
                                                      "thing",
                                                      Anonymous());
    assert(!ent3);
}

void WorldRoutertest::test_spawnNewEntity_thing()
{
    auto ent3 = test_world->spawnNewEntity("bob",
                                                      "thing",
                                                      Anonymous());
    assert(!ent3);
}

void WorldRoutertest::test_createSpawnPoint()
{
    std::string id;
    long int_id = newId(id);

    auto  ent2 = new Entity(id, int_id);
    assert(ent2 != 0);
    ent2->m_location.m_parent = m_rootEntity;
    ent2->m_location.m_pos = Point3D(0,0,0);
    test_world->addEntity(ent2);

    {
        int ret;

        Atlas::Message::MapType spawn_data;
        ret = test_world->createSpawnPoint(spawn_data, ent2);
        assert(ret == -1);

        spawn_data["name"] = 1;
        ret = test_world->createSpawnPoint(spawn_data, ent2);
        assert(ret == -1);

        spawn_data["name"] = "bob";
        ret = test_world->createSpawnPoint(spawn_data, ent2);
        assert(ret == 0);

        ret = test_world->createSpawnPoint(spawn_data, ent2);
        assert(ret == 0);
    }

    {
        Atlas::Message::ListType spawn_repr;
        test_world->getSpawnList(spawn_repr);
        assert(!spawn_repr.empty());
        assert(spawn_repr.size() == 1u);
    }

    auto ent3 = test_world->spawnNewEntity("bob",
                                                      "permitted_non_existant",
                                                      Anonymous());
    assert(!ent3);

    ent3 = test_world->spawnNewEntity("bob",
                                      "thing",
                                      Anonymous());
    assert(ent3);
}

void WorldRoutertest::test_delEntity()
{
    std::string id;
    long int_id = newId(id);

    auto  ent2 = new Entity(id, int_id);
    assert(ent2 != 0);
    ent2->m_location.m_parent = m_rootEntity;
    ent2->m_location.m_pos = Point3D(0,0,0);
    test_world->addEntity(ent2);

    test_world->delEntity(ent2);
    test_world->delEntity(m_rootEntity.get());
}

void WorldRoutertest::test_delEntity_world()
{
    test_world->delEntity(m_rootEntity.get());
}

int main()
{
    WorldRoutertest t;

    return t.run();
}

// Stubs

int timeoffset = 0;

namespace consts {
const char * rootWorldId = "0";
const long rootWorldIntId = 0L;
}

namespace Atlas { namespace Objects { namespace Operation {
int TICK_NO = -1;
}}}

#include "../stubs/rules/simulation/stubWorld.h"
#include "../stubs/rules/simulation/stubThing.h"
#include "../stubs/rules/simulation/stubEntity.h"
#include "../stubs/rules/stubDomain.h"
#include "../stubs/common/stubOperationsDispatcher.h"
#include "../stubs/common/stubTypeNode.h"

template class OperationsDispatcher<LocatedEntity>;
template struct OpQueEntry<LocatedEntity>;

#define STUB_LocatedEntity_LocatedEntity_DTOR
// Deletions and reference count decrements are required to ensure map
// memory management works correctly.
LocatedEntity::~LocatedEntity()
{
    if (m_location.m_parent) {
        m_location.m_parent = nullptr;
    }
}

#define STUB_LocatedEntity_makeContainer
void LocatedEntity::makeContainer()
{
    if (!m_contains) {
        m_contains.reset(new LocatedEntitySet);
    }
}


#include "../stubs/rules/stubLocatedEntity.h"

#include "../stubs/common/stubRouter.h"
#include "../stubs/common/stubLink.h"
#include "../stubs/server/stubServerRouting.h"
#include "../stubs/server/stubLobby.h"
#include "../stubs/common/stubShaker.h"

long integerId(const std::string & id)
{
    long intId = strtol(id.c_str(), 0, 10);
    if (intId == 0 && id != "0") {
        intId = -1L;
    }

    return intId;
}

#include "../stubs/common/stublog.h"

static inline float sqr(float x)
{
    return x * x;
}

#define STUB_squareDistance
WFMath::CoordType squareDistance(const Point3D & u, const Point3D & v)
{
    return (sqr(u.x() - v.x()) + sqr(u.y() - v.y()) + sqr(u.z() - v.z()));
}

static bool distanceFromAncestor(const Location & self,
                                 const Location & other, Point3D & c)
{
    if (&self == &other) {
        return true;
    }

    if (other.m_parent == nullptr) {
        return false;
    }

    if (other.orientation().isValid()) {
        c = c.toParentCoords(other.m_pos, other.orientation());
    } else {
        static const Quaternion identity(1, 0, 0, 0);
        c = c.toParentCoords(other.m_pos, identity);
    }

    return distanceFromAncestor(self, other.m_parent->m_location, c);
}

static bool distanceToAncestor(const Location & self,
                               const Location & other, Point3D & c)
{
    c.setToOrigin();
    if (distanceFromAncestor(self, other, c)) {
        return true;
    } else if ((self.m_parent) &&
               distanceToAncestor(self.m_parent->m_location, other, c)) {
        if (self.orientation().isValid()) {
            c = c.toLocalCoords(self.m_pos, self.orientation());
        } else {
            static const Quaternion identity(1, 0, 0, 0);
            c = c.toLocalCoords(self.m_pos, identity);
        }
        return true;
    }
    log(ERROR, "Broken entity hierarchy doing distance calculation");
    if (self.m_parent) {
        std::cerr << "Self(" << self.m_parent->getId() << "," << self.m_parent->describeEntity() << ")"
                  << std::endl << std::flush;
    }
    if (other.m_parent) {
        std::cerr << "Other(" << other.m_parent->getId() << "," << other.m_parent->describeEntity() << ")"
                  << std::endl << std::flush;
    }
     
    return false;
}

#define STUB_squareDistance
boost::optional<WFMath::CoordType> squareDistance(const Location & self, const Location & other)
{
    Point3D dist;
    distanceToAncestor(self, other, dist);
    return sqrMag(dist);
}

#include "../stubs/rules/stubLocation.h"

static long idGenerator = 2;

long newId(std::string & id)
{
    if (stub_deny_newid) {
        return -1;
    }
    static char buf[32];
    long new_id = ++idGenerator;
    sprintf(buf, "%ld", new_id);
    id = buf;
    assert(!id.empty());
    return new_id;
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



#ifndef STUB_Inheritance_getType
#define STUB_Inheritance_getType
const TypeNode* Inheritance::getType(const std::string & parent) const
{
    auto I = atlasObjects.find(parent);
    if (I == atlasObjects.end()) {
        return 0;
    }
    return I->second.get();
}
#endif //STUB_Inheritance_getType

#include "../stubs/common/stubInheritance.h"


#include "../stubs/rules/simulation/stubTask.h"
#include "../stubs/common/stubVariable.h"
#include "../stubs/common/stubMonitors.h"
#include "../stubs/common/stubProperty.h"
#include "../stubs/common/stubPropertyManager.h"
#include "rules/simulation/CorePropertyManager.h"
#include "common/Property_impl.h"

#define STUB_EntityBuilder_newEntity
Ref<LocatedEntity> EntityBuilder::newEntity(const std::string & id, long intId,
                                         const std::string & type,
                                         const RootEntity & attributes,
                                         const BaseWorld & world) const
{
    if (type == "thing") {
        auto e = new Entity(id, intId);
        e->m_location.m_parent = &world.getDefaultLocation();
        e->m_location.m_pos = Point3D(0,0,0);
        return e;
    }
    return 0;
}
#include "../stubs/server/stubEntityBuilder.h"
#include "../stubs/modules/stubWeakEntityRef.h"


SpawnEntity::SpawnEntity(LocatedEntity *)
{
}

int SpawnEntity::setup(const MapType &)
{
    return 0;
}

int SpawnEntity::spawnEntity(const std::string & type,
                             const RootEntity & dsc) const
{
    if (type == "thing" || type == "permitted_non_existant") {
        return 0;
    }
    return -1;
}

int SpawnEntity::addToMessage(MapType & msg) const
{
    return -1;
}

int SpawnEntity::placeInSpawn(Location&) const
{
    return 0;
}

// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2013 Alistair Riddoch
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

#include "../../TestBase.h"

#include "rules/ai/BaseMind.h"

#include <Atlas/Objects/Anonymous.h>

#include <client/ClientPropertyManager.h>
#include <rules/SolidProperty.h>
#include "rules/SimpleTypeStore.h"


#include <memory>

using Atlas::Objects::Entity::Anonymous;

class BaseMindMapEntityintegration : public Cyphesis::TestBase
{
  protected:
    ClientPropertyManager* m_propertyManager;
    std::unique_ptr<TypeStore> m_typeStore;
    Ref<BaseMind> m_mind;
    TypeNode* m_type;
  public:
    BaseMindMapEntityintegration();

    void setup();
    void teardown();

    void test_MemMapdel_top();
    void test_MemMapdel_mid();
    void test_MemMapdel_edge();
    void test_MemMapreadEntity_noloc();
    void test_MemMapreadEntity_changeloc();
    void test_MemMap_updateAdd_location_properties_have_effect();
    void test_MemMapcheck();
};

BaseMindMapEntityintegration::BaseMindMapEntityintegration()
{
    ADD_TEST(BaseMindMapEntityintegration::test_MemMapdel_top);
    ADD_TEST(BaseMindMapEntityintegration::test_MemMapdel_mid);
    ADD_TEST(BaseMindMapEntityintegration::test_MemMapdel_edge);
    ADD_TEST(BaseMindMapEntityintegration::test_MemMapreadEntity_noloc);
    ADD_TEST(BaseMindMapEntityintegration::test_MemMapreadEntity_changeloc);
    ADD_TEST(BaseMindMapEntityintegration::test_MemMap_updateAdd_location_properties_have_effect);
    ADD_TEST(BaseMindMapEntityintegration::test_MemMapcheck);
}

void BaseMindMapEntityintegration::setup()
{
    m_propertyManager = new ClientPropertyManager();
    m_typeStore = std::make_unique<SimpleTypeStore>(*m_propertyManager);
    m_mind = new BaseMind(1, "2", *m_typeStore);
    m_type = new TypeNode("type");
}

void BaseMindMapEntityintegration::teardown()
{
    m_mind = nullptr;
    delete m_type;
    m_type = nullptr;
    m_typeStore.reset();
    delete m_propertyManager;
    m_propertyManager = nullptr;
}

void BaseMindMapEntityintegration::test_MemMapdel_top()
{
    //HACK: Disabled for now as we've disabled the deletion of entities from the mind. See MemMap::del(...)
    return;
    Ref<MemEntity> tlve = new MemEntity(0);
    tlve->m_contains.reset(new LocatedEntitySet);
    m_mind->m_map.m_entities[0] = tlve;

    Ref<MemEntity> e2 = new MemEntity(2);
    e2->m_contains.reset(new LocatedEntitySet);
    e2->m_parent = tlve.get();
    tlve->m_contains->insert(e2);
    m_mind->m_map.m_entities[2] = e2;

    Ref<MemEntity> e3 = new MemEntity(3);
    e3->m_contains.reset(new LocatedEntitySet);
    e3->m_parent = e2.get();
    e2->m_contains->insert(e3);
    m_mind->m_map.m_entities[3] = e3;

    ASSERT_EQUAL(m_mind->m_map.m_entities.size(), 4u);

    // Remove tlve from the map
    m_mind->m_map.del(tlve->getId());

    ASSERT_EQUAL(m_mind->m_map.m_entities.size(), 3u);
    ASSERT_FALSE(e2->m_parent);
    ASSERT_TRUE(e2->m_contains->find(e3) != e2->m_contains->end());
}

void BaseMindMapEntityintegration::test_MemMapdel_mid()
{
    //HACK: Disabled for now as we've disabled the deletion of entities from the mind. See MemMap::del(...)
    return;
    Ref<MemEntity> tlve = new MemEntity(0);
    tlve->m_contains.reset(new LocatedEntitySet);
    m_mind->m_map.m_entities[0] = tlve;

    Ref<MemEntity> e2 = new MemEntity(2);
    e2->m_contains.reset(new LocatedEntitySet);
    e2->m_parent = tlve.get();
    tlve->m_contains->insert(e2);
    m_mind->m_map.m_entities[2] = e2;

    Ref<MemEntity> e3 = new MemEntity(3);
    e3->m_contains.reset(new LocatedEntitySet);
    e3->m_parent = e2.get();
    e2->m_contains->insert(e3);
    m_mind->m_map.m_entities[3] = e3;

    ASSERT_EQUAL(m_mind->m_map.m_entities.size(), 4u);

    // Set a reference, so we can check e2 once it is removed
    e2->incRef();
    // Remove e2 from the map
    m_mind->m_map.del(e2->getId());

    ASSERT_EQUAL(m_mind->m_map.m_entities.size(), 3u);
    ASSERT_TRUE(tlve->m_contains->find(e3) != tlve->m_contains->end());
    ASSERT_TRUE(tlve->m_contains->find(e2) == tlve->m_contains->end());

    ASSERT_FALSE(e2->m_parent);
    ASSERT_EQUAL(e2->checkRef(), 0);
}

void BaseMindMapEntityintegration::test_MemMapdel_edge()
{
    //HACK: Disabled for now as we've disabled the deletion of entities from the mind. See MemMap::del(...)
    return;
    Ref<MemEntity> tlve = new MemEntity(0);
    tlve->m_contains.reset(new LocatedEntitySet);
    m_mind->m_map.m_entities[0] = tlve;

    Ref<MemEntity> e2 = new MemEntity(2);
    e2->m_contains.reset(new LocatedEntitySet);
    e2->m_parent = tlve.get();
    tlve->m_contains->insert(e2);
    m_mind->m_map.m_entities[2] = e2;

    Ref<MemEntity> e3 = new MemEntity(3);
    e3->m_contains.reset(new LocatedEntitySet);
    e3->m_parent = e2.get();
    e2->m_contains->insert(e3);
    m_mind->m_map.m_entities[3] = e3;

    ASSERT_EQUAL(m_mind->m_map.m_entities.size(), 4u);

    // Set a reference, so we can check e3 once it is removed
    e3->incRef();
    // Remove e3 from the map
    m_mind->m_map.del(e3->getId());

    ASSERT_EQUAL(m_mind->m_map.m_entities.size(), 3u);
    ASSERT_TRUE(tlve->m_contains->find(e2) != tlve->m_contains->end());
    ASSERT_TRUE(e2->m_contains->find(e3) == e2->m_contains->end());

    ASSERT_FALSE(e3->m_parent);
    ASSERT_EQUAL(e3->checkRef(), 0);
}

void BaseMindMapEntityintegration::test_MemMapreadEntity_noloc()
{
    Ref<MemEntity> tlve = new MemEntity(0);
    tlve->m_contains.reset(new LocatedEntitySet);
    m_mind->m_map.m_entities[0] = tlve;

    Ref<MemEntity> e2 = new MemEntity(2);
    e2->m_contains.reset(new LocatedEntitySet);
    e2->m_parent = tlve.get();
    tlve->m_contains->insert(e2);
    m_mind->m_map.m_entities[1] = e2;

    Ref<MemEntity> e3 = new MemEntity(3);
    m_mind->m_map.m_entities[2] = e3;

    ASSERT_EQUAL(m_mind->m_map.m_entities.size(), 3u);
    ASSERT_FALSE(e3->m_parent);

    Anonymous data;
    data->setLoc(tlve->getId());

    // Read in entity data the sets the LOC of e3 to tlve
    m_mind->m_map.readEntity(e3, data, 0);

    ASSERT_EQUAL(e3->m_parent, tlve.get())
    ASSERT_TRUE(tlve->m_contains->find(e3) != tlve->m_contains->end());
}

void BaseMindMapEntityintegration::test_MemMapreadEntity_changeloc()
{
    Ref<MemEntity> tlve = new MemEntity(0);
    tlve->m_contains.reset(new LocatedEntitySet);
    m_mind->m_map.m_entities[0] = tlve;

    Ref<MemEntity> e2 = new MemEntity(2);
    e2->m_contains.reset(new LocatedEntitySet);
    e2->m_parent = tlve.get();
    tlve->m_contains->insert(e2);
    m_mind->m_map.m_entities[1] = e2;

    Ref<MemEntity> e3 = new MemEntity(3);
    e3->m_contains.reset(new LocatedEntitySet);
    e3->m_parent = e2.get();
    e2->m_contains->insert(e3);
    m_mind->m_map.m_entities[2] = e3;

    ASSERT_EQUAL(m_mind->m_map.m_entities.size(), 3u);

    Anonymous data;
    data->setLoc(tlve->getId());

    // Read in entity data that changes the LOC of e3 from e2 to TLVE
    m_mind->m_map.readEntity(e3, data, 0);

    ASSERT_EQUAL(e3->m_parent, tlve.get())
    ASSERT_TRUE(e2->m_contains->find(e3) == e2->m_contains->end());
    ASSERT_TRUE(tlve->m_contains->find(e3) != tlve->m_contains->end());
}

void BaseMindMapEntityintegration::test_MemMap_updateAdd_location_properties_have_effect()
{
    Ref<MemEntity> tlve = new MemEntity(0);

    Location location(tlve);
    {
        Atlas::Objects::Entity::Anonymous args;
        location.addToEntity(args);
        args->setAttr("id", "1");
        m_mind->m_map.updateAdd(args, 1.0f);
    }

    auto ent = m_mind->m_map.get("1");
    ASSERT_EQUAL("1", ent->getId());
    ASSERT_TRUE(!ent->getPropertyClassFixed<SolidProperty>());


    BBox bbox2(WFMath::Point<3>(10,20,30), WFMath::Point<3>(40,50,60));
    location.setBBox(bbox2);
    {
        Atlas::Objects::Entity::Anonymous args;
        location.addToEntity(args);
        args->setAttr("id", "1");
        args->setAttr("solid", 0);
        args->setAttr("simple", 0);
        m_mind->m_map.updateAdd(args, 2.0f);
    }

    ent = m_mind->m_map.get("1");
    ASSERT_TRUE(ent->getPropertyClassFixed<SolidProperty>() && !ent->getPropertyClassFixed<SolidProperty>()->isTrue());

}


void BaseMindMapEntityintegration::test_MemMapcheck()
{
    Ref<MemEntity> tlve = new MemEntity(0);
    tlve->setType(m_type);
    tlve->m_contains.reset(new LocatedEntitySet);
    m_mind->m_map.m_entities[0] = tlve;

    Ref<MemEntity> e2 = new MemEntity(2);
    e2->setType(m_type);
    e2->m_contains.reset(new LocatedEntitySet);
    e2->m_parent = tlve.get();
    tlve->m_contains->insert(e2);
    m_mind->m_map.m_entities[1] = e2;

    Ref<MemEntity> e3 = new MemEntity(3);
    e3->setType(m_type);
    e3->m_contains.reset(new LocatedEntitySet);
    e3->m_parent = e2.get();
    e2->m_contains->insert(e3);
    m_mind->m_map.m_entities[2] = e3;

    ASSERT_EQUAL(m_mind->m_map.m_entities.size(), 3u);

    m_mind->m_map.m_checkIterator = m_mind->m_map.m_entities.find(3);
    e3->setVisible(false);
    double time = e3->lastSeen() + 900;

    // We have set up e3 so it is due to be purged from memory.
    m_mind->m_map.check(time);

    //TODO: Enable this check again, as we've disabled removal of entities from MemMap since there are issues
    // Check it has been removed
    //ASSERT_TRUE(e2->m_contains->find(e3) == e2->m_contains->end());
    //ASSERT_TRUE(tlve->m_contains->find(e3) == tlve->m_contains->end());

    //TODO: Enable this check again, as we've disabled removal of entities from MemMap since there are issues
    // Check the reference we have is the only one remaining
    //ASSERT_NULL(e3->m_parent)

    //TODO: Enable this check again; it's disabled since we've disabled ref decrements in MemMap, since the knowledge code doesn't handle entity references correctly.
    //ASSERT_EQUAL(e3->checkRef(), 0);
}

int main()
{
    BaseMindMapEntityintegration t;

    return t.run();
}

// stubs

#include "rules/Script.h"

#include "common/Inheritance.h"
#include "common/log.h"
#include "common/TypeNode.h"

#include "../../stubs/common/stubcustom.h"
#include "../../stubs/rules/simulation/stubDensityProperty.h"
#include "../../stubs/rules/stubScaleProperty.h"


#include "../../stubs/common/stubRouter.h"
#include "../../stubs/common/stubInheritance.h"
#include "../../stubs/rules/stubAtlasProperties.h"
#include "../../stubs/rules/stubScript.h"
#include "../../stubs/common/stubTypeNode.h"
#include "../../stubs/rules/ai/stubTypeResolver.h"
#include "../../stubs/rules/stubSimpleTypeStore.h"
#include "../../stubs/common/stublog.h"
#include "../../stubs/common/stubid.h"


static inline WFMath::CoordType sqr(WFMath::CoordType x)
{
    return x * x;
}

WFMath::CoordType squareDistance(const Point3D & u, const Point3D & v)
{
    return (sqr(u.x() - v.x()) + sqr(u.y() - v.y()) + sqr(u.z() - v.z()));
}

void addToEntity(const Point3D & p, std::vector<double> & vd)
{
}

void addToEntity(const Vector3D & v, std::vector<double> & vd)
{
}

template <>
int fromStdVector<double>(Point3D & p, const std::vector<double> & vf)
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
int fromStdVector<double>(Vector3D & v, const std::vector<double> & vf)
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

#define STUB_PropertyManager_installFactory
void PropertyManager::installFactory(const std::string & name, std::unique_ptr<PropertyKit> factory)
{
    m_propertyFactories.emplace(name, std::move(factory));
}
int PropertyManager::installFactory(const std::string & type_name, const Atlas::Objects::Root & type_desc, std::unique_ptr<PropertyKit> factory)
{
    m_propertyFactories.emplace(type_name, std::move(factory));
    return 0;
}

#include "../../stubs/common/stubPropertyManager.h"
#include "../../stubs/rules/stubModifier.h"


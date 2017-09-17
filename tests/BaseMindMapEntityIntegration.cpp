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

#include "TestBase.h"

#include "rulesets/BaseMind.h"

#include "common/Unseen.h"

#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/SmartPtr.h>

#include <cassert>

using Atlas::Objects::Entity::Anonymous;

class BaseMindMapEntityintegration : public Cyphesis::TestBase
{
  protected:
    BaseMind * m_mind;
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
    m_mind = new BaseMind("1", 1);
}

void BaseMindMapEntityintegration::teardown()
{
    delete m_mind;
}

void BaseMindMapEntityintegration::test_MemMapdel_top()
{
    //HACK: Disabled for now as we've disabled the deletion of entities from the mind. See MemMap::del(...)
    return;
    MemEntity * tlve = new MemEntity("0", 0);
    tlve->m_contains = new LocatedEntitySet;
    m_mind->m_map.m_entities[0] = tlve;

    MemEntity * e2 = new MemEntity("2", 2);
    e2->m_contains = new LocatedEntitySet;
    e2->m_location.m_loc = tlve;
    tlve->m_contains->insert(e2);
    m_mind->m_map.m_entities[2] = e2;

    MemEntity * e3 = new MemEntity("3", 3);
    e3->m_contains = new LocatedEntitySet;
    e3->m_location.m_loc = e2;
    e2->m_contains->insert(e3);
    m_mind->m_map.m_entities[3] = e3;

    ASSERT_EQUAL(m_mind->m_map.m_entities.size(), 4u);

    // Remove tlve from the map
    m_mind->m_map.del(tlve->getId());

    ASSERT_EQUAL(m_mind->m_map.m_entities.size(), 3u);
    ASSERT_NULL(e2->m_location.m_loc);
    ASSERT_TRUE(e2->m_contains->find(e3) != e2->m_contains->end());
}

void BaseMindMapEntityintegration::test_MemMapdel_mid()
{
    //HACK: Disabled for now as we've disabled the deletion of entities from the mind. See MemMap::del(...)
    return;
    MemEntity * tlve = new MemEntity("0", 0);
    tlve->m_contains = new LocatedEntitySet;
    m_mind->m_map.m_entities[0] = tlve;

    MemEntity * e2 = new MemEntity("2", 2);
    e2->m_contains = new LocatedEntitySet;
    e2->m_location.m_loc = tlve;
    tlve->m_contains->insert(e2);
    m_mind->m_map.m_entities[2] = e2;

    MemEntity * e3 = new MemEntity("3", 3);
    e3->m_contains = new LocatedEntitySet;
    e3->m_location.m_loc = e2;
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

    ASSERT_NULL(e2->m_location.m_loc);
    ASSERT_EQUAL(e2->checkRef(), 0);
    e2->decRef();
}

void BaseMindMapEntityintegration::test_MemMapdel_edge()
{
    //HACK: Disabled for now as we've disabled the deletion of entities from the mind. See MemMap::del(...)
    return;
    MemEntity * tlve = new MemEntity("0", 0);
    tlve->m_contains = new LocatedEntitySet;
    m_mind->m_map.m_entities[0] = tlve;

    MemEntity * e2 = new MemEntity("2", 2);
    e2->m_contains = new LocatedEntitySet;
    e2->m_location.m_loc = tlve;
    tlve->m_contains->insert(e2);
    m_mind->m_map.m_entities[2] = e2;

    MemEntity * e3 = new MemEntity("3", 3);
    e3->m_contains = new LocatedEntitySet;
    e3->m_location.m_loc = e2;
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

    ASSERT_NULL(e3->m_location.m_loc);
    ASSERT_EQUAL(e3->checkRef(), 0);
    e3->decRef();
}

void BaseMindMapEntityintegration::test_MemMapreadEntity_noloc()
{
    MemEntity * tlve = new MemEntity("0", 0);
    tlve->m_contains = new LocatedEntitySet;
    m_mind->m_map.m_entities[0] = tlve;

    MemEntity * e2 = new MemEntity("2", 2);
    e2->m_contains = new LocatedEntitySet;
    e2->m_location.m_loc = tlve;
    tlve->m_contains->insert(e2);
    m_mind->m_map.m_entities[2] = e2;

    MemEntity * e3 = new MemEntity("3", 3);
    m_mind->m_map.m_entities[3] = e3;

    ASSERT_EQUAL(m_mind->m_map.m_entities.size(), 4u);
    ASSERT_NULL(e3->m_location.m_loc);

    Anonymous data;
    data->setLoc(tlve->getId());

    // Read in entity data the sets the LOC of e3 to tlve
    m_mind->m_map.readEntity(e3, data, 0);

    ASSERT_EQUAL(e3->m_location.m_loc, tlve)
    ASSERT_TRUE(tlve->m_contains->find(e3) != tlve->m_contains->end());
}

void BaseMindMapEntityintegration::test_MemMapreadEntity_changeloc()
{
    MemEntity * tlve = new MemEntity("0", 0);
    tlve->m_contains = new LocatedEntitySet;
    m_mind->m_map.m_entities[0] = tlve;

    MemEntity * e2 = new MemEntity("2", 2);
    e2->m_contains = new LocatedEntitySet;
    e2->m_location.m_loc = tlve;
    tlve->m_contains->insert(e2);
    m_mind->m_map.m_entities[2] = e2;

    MemEntity * e3 = new MemEntity("3", 3);
    e3->m_contains = new LocatedEntitySet;
    e3->m_location.m_loc = e2;
    e2->m_contains->insert(e3);
    m_mind->m_map.m_entities[3] = e3;

    ASSERT_EQUAL(m_mind->m_map.m_entities.size(), 4u);

    Anonymous data;
    data->setLoc(tlve->getId());

    // Read in entity data that changes the LOC of e3 from e2 to TLVE
    m_mind->m_map.readEntity(e3, data, 0);

    ASSERT_EQUAL(e3->m_location.m_loc, tlve)
    ASSERT_TRUE(e2->m_contains->find(e3) == e2->m_contains->end());
    ASSERT_TRUE(tlve->m_contains->find(e3) != tlve->m_contains->end());
}

void BaseMindMapEntityintegration::test_MemMap_updateAdd_location_properties_have_effect()
{

    MemEntity * tlve = new MemEntity("0", 0);

    Location location(tlve);
    BBox bbox(WFMath::Point<3>(1,2,3), WFMath::Point<3>(4,5,6));
    location.setBBox(bbox);


    {
        Atlas::Objects::Entity::Anonymous args;
        location.addToEntity(args);
        args->setAttr("id", "0");
        m_mind->m_map.updateAdd(args, 1.0f);
    }

    MemEntity* ent = m_mind->m_map.get("0");
    ASSERT_EQUAL("0", ent->getId());
    ASSERT_EQUAL(bbox, ent->m_location.m_bBox);
    ASSERT_TRUE(ent->m_location.isSolid());
    ASSERT_TRUE(ent->m_location.isSimple());


    BBox bbox2(WFMath::Point<3>(10,20,30), WFMath::Point<3>(40,50,60));
    location.setBBox(bbox2);
    {
        Atlas::Objects::Entity::Anonymous args;
        location.addToEntity(args);
        args->setAttr("id", "0");
        args->setAttr("solid", 0);
        args->setAttr("simple", 0);
        m_mind->m_map.updateAdd(args, 2.0f);
    }

    ent = m_mind->m_map.get("0");
    ASSERT_EQUAL(bbox2, ent->m_location.m_bBox);
    ASSERT_TRUE(!ent->m_location.isSolid());
    ASSERT_TRUE(!ent->m_location.isSimple());

}


void BaseMindMapEntityintegration::test_MemMapcheck()
{
    MemEntity * tlve = new MemEntity("0", 0);
    tlve->setType(MemMap::m_entity_type);
    tlve->m_contains = new LocatedEntitySet;
    m_mind->m_map.m_entities[0] = tlve;

    MemEntity * e2 = new MemEntity("2", 2);
    e2->setType(MemMap::m_entity_type);
    e2->m_contains = new LocatedEntitySet;
    e2->m_location.m_loc = tlve;
    tlve->m_contains->insert(e2);
    m_mind->m_map.m_entities[2] = e2;

    MemEntity * e3 = new MemEntity("3", 3);
    e3->setType(MemMap::m_entity_type);
    e3->m_contains = new LocatedEntitySet;
    e3->m_location.m_loc = e2;
    e2->m_contains->insert(e3);
    m_mind->m_map.m_entities[3] = e3;

    ASSERT_EQUAL(m_mind->m_map.m_entities.size(), 4u);

    m_mind->m_map.m_checkIterator = m_mind->m_map.m_entities.find(3);
    e3->setVisible(false);
    e3->incRef();
    double time = e3->lastSeen() + 900;

    // We have set up e3 so it is due to be purged from memory.
    m_mind->m_map.check(time);

    //TODO: Enable this check again, as we've disabled removal of entities from MemMap since there are issues
    // Check it has been removed
    //ASSERT_TRUE(e2->m_contains->find(e3) == e2->m_contains->end());
    //ASSERT_TRUE(tlve->m_contains->find(e3) == tlve->m_contains->end());

    //TODO: Enable this check again, as we've disabled removal of entities from MemMap since there are issues
    // Check the reference we have is the only one remaining
    //ASSERT_NULL(e3->m_location.m_loc)

    //TODO: Enable this check again; it's disabled since we've disabled ref decrements in MemMap, since the knowledge code doesn't handle entity references correctly.
    //ASSERT_EQUAL(e3->checkRef(), 0);
    e3->decRef();
}

int main()
{
    BaseMindMapEntityintegration t;

    return t.run();
}

// stubs

#include "rulesets/Script.h"

#include "common/Inheritance.h"
#include "common/log.h"
#include "common/TypeNode.h"

#include "stubs/common/stubCustom.h"
#include "stubs/rulesets/stubDensityProperty.h"

LocatedEntity::LocatedEntity(const std::string & id, long intId) :
               Router(id, intId),
               m_refCount(0), m_seq(0),
               m_script(0), m_type(0), m_flags(0), m_contains(0)
{
}

// Deletions and reference count decrements are required to ensure map
// memory management works correctly.
LocatedEntity::~LocatedEntity()
{
    if (m_location.m_loc != 0) {
        m_location.m_loc->decRef();
    }
    delete m_contains;
}

bool LocatedEntity::hasAttr(const std::string & name) const
{
    return false;
}

int LocatedEntity::getAttr(const std::string & name,
                           Atlas::Message::Element & attr) const
{
    return -1;
}

int LocatedEntity::getAttrType(const std::string & name,
                               Atlas::Message::Element & attr,
                               int type) const
{
    return -1;
}

PropertyBase * LocatedEntity::setAttr(const std::string & name,
                                      const Atlas::Message::Element & attr)
{
    return 0;
}

const PropertyBase * LocatedEntity::getProperty(const std::string & name) const
{
    return 0;
}

PropertyBase * LocatedEntity::modProperty(const std::string & name)
{
    return 0;
}

PropertyBase * LocatedEntity::setProperty(const std::string & name,
                                          PropertyBase * prop)
{
    return 0;
}

void LocatedEntity::installDelegate(int, const std::string &)
{
}

void LocatedEntity::removeDelegate(int class_no, const std::string & delegate)
{
}

void LocatedEntity::destroy()
{
}

Domain * LocatedEntity::getDomain()
{
    return 0;
}

const Domain * LocatedEntity::getDomain() const
{
    return 0;
}

void LocatedEntity::sendWorld(const Operation & op)
{
}

void LocatedEntity::onContainered(const LocatedEntity*)
{
}

void LocatedEntity::onUpdated()
{
}

void LocatedEntity::makeContainer()
{
    if (m_contains == 0) {
        m_contains = new LocatedEntitySet;
    }
}

void LocatedEntity::merge(const Atlas::Message::MapType & ent)
{
    for (auto I : ent) {
        const std::string & key = I.first;
        setAttr(key, I.second);
    }
}
void LocatedEntity::addChild(LocatedEntity& childEntity)
{
}

void LocatedEntity::removeChild(LocatedEntity& childEntity)
{
}

void LocatedEntity::setType(const TypeNode* t) {
    m_type = t;
}

std::vector<Atlas::Objects::Root> LocatedEntity::getThoughts() const
{
    return std::vector<Atlas::Objects::Root>();
}

Router::Router(const std::string & id, long intId) : m_id(id), m_intId(intId)
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

Inheritance * Inheritance::m_instance = NULL;

Inheritance::Inheritance() : noClass(0)
{
}

const TypeNode * Inheritance::getType(const std::string & parent)
{
    return 0;
}

Inheritance & Inheritance::instance()
{
    if (m_instance == NULL) {
        m_instance = new Inheritance();
    }
    return *m_instance;
}

Script::Script()
{
}

/// \brief Script destructor
Script::~Script()
{
}

bool Script::operation(const std::string & opname,
                       const Atlas::Objects::Operation::RootOperation & op,
                       OpVector & res)
{
   return false;
}

void Script::hook(const std::string & function, LocatedEntity * entity)
{
}

DateTime::DateTime(int t)
{
}

void DateTime::update(int t)
{
}

void WorldTime::initTimeInfo()
{
}

TypeNode::TypeNode(const std::string & name) : m_name(name), m_parent(0)
{
}

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

WFMath::CoordType sqrMag(const Point3D & p) {
    return 0;
}




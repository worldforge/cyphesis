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


#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include "allOperations.h"
#include "TestBase.h"

#include "rulesets/Thing.h"

#include <Atlas/Objects/Anonymous.h>

#include <cassert>

using Atlas::Message::MapType;
using Atlas::Message::ListType;
using Atlas::Objects::smart_dynamic_cast;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Operation::Update;

class testThing : public Thing {
  public:
    testThing(const std::string & id, long intId) :
        Thing(id, intId) { }
    using Thing::updateProperties;
};

static const std::string testName("bob");
static const std::string testNewName("fred");

class ThingupdatePropertiestest : public Cyphesis::TestBase
{
  protected:
    Thing * m_thing;
    Property<std::string> * m_name;

  public:
    ThingupdatePropertiestest();

    void setup();
    void teardown();

    void test_update();
};

ThingupdatePropertiestest::ThingupdatePropertiestest()
{
    ADD_TEST(ThingupdatePropertiestest::test_update);
}

void ThingupdatePropertiestest::setup()
{
    m_name = new Property<std::string>(flag_unsent);
    m_name->data() = testName;

    m_thing = new Thing("1", 1);
    m_thing->setProperty("name", m_name);
}

void ThingupdatePropertiestest::teardown()
{
    delete m_thing;
}

void ThingupdatePropertiestest::test_update()
{
    ASSERT_EQUAL(m_name->flags() & flag_unsent, flag_unsent);

    Update u;
    OpVector res;
   
    m_thing->updateProperties(u, res);

    // The flag marking the property has been cleared
    ASSERT_EQUAL(m_name->flags() & flag_unsent, 0u);

    // The update operation should not have actually changed the name
    // at all
    ASSERT_EQUAL(m_name->data(), testName);

    // The result should be a Sight op
    ASSERT_EQUAL(res.size(), 1u);

    const Operation & result = res.front();

    ASSERT_EQUAL(result->getClassNo(), Atlas::Objects::Operation::SIGHT_NO);

    // The result argument should be a Set op
    ASSERT_EQUAL(result->getArgs().size(), 1u);

    const Operation & result_inner = smart_dynamic_cast<Operation>(result->getArgs().front());

    ASSERT_TRUE(result_inner.isValid());

    ASSERT_EQUAL(result_inner->getClassNo(), Atlas::Objects::Operation::SET_NO);
    ASSERT_EQUAL(result_inner->getArgs().size(), 1u);

    auto set_arg = smart_dynamic_cast<RootEntity>(result_inner->getArgs().front());

    // Make sure the name on the sight set argument has been set to the
    // name of the entity, not the name in the Update op.
    ASSERT_TRUE(!set_arg->isDefaultName());
    ASSERT_EQUAL(set_arg->getName(), testName);
}

int main()
{
    ThingupdatePropertiestest t;

    return t.run();
}

// stubs

#include "rulesets/Domain.h"

#include "common/BaseWorld.h"
#include "common/const.h"
#include "common/log.h"

#include "stubs/common/stubCustom.h"
#include "stubs/rulesets/stubEntity.h"
#include "stubs/rulesets/stubDomain.h"
#include "stubs/common/stubRouter.h"
#include "stubs/common/stubBaseWorld.h"
#include "stubs/modules/stubLocation.h"
#include "stubs/rulesets/stubPropelProperty.h"
#include "stubs/rulesets/stubDomainProperty.h"

LocatedEntity::LocatedEntity(const std::string & id, long intId) :
               Router(id, intId),
               m_refCount(0), m_seq(0),
               m_script(0), m_type(0), m_flags(0), m_contains(0)
{
}

LocatedEntity::~LocatedEntity()
{
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

bool LocatedEntity::isVisibleForOtherEntity(const LocatedEntity* watcher) const
{
    return true;
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

void LocatedEntity::changeContainer(LocatedEntity * new_loc)
{
    assert(m_location.m_loc != 0);
    assert(m_location.m_loc->m_contains != 0);
    m_location.m_loc->m_contains->erase(this);
    if (m_location.m_loc->m_contains->empty()) {
        m_location.m_loc->onUpdated();
    }
    new_loc->makeContainer();
    bool was_empty = new_loc->m_contains->empty();
    new_loc->m_contains->insert(this);
    if (was_empty) {
        new_loc->onUpdated();
    }
    assert(m_location.m_loc->checkRef() > 0);
    LocatedEntity* oldLoc = m_location.m_loc;
    m_location.m_loc = new_loc;
    m_location.m_loc->incRef();
    assert(m_location.m_loc->checkRef() > 0);

    onContainered(oldLoc);
    oldLoc->decRef();
}

void LocatedEntity::merge(const MapType & ent)
{
}

void LocatedEntity::addChild(LocatedEntity& childEntity)
{

}

void LocatedEntity::removeChild(LocatedEntity& childEntity)
{

}

void LocatedEntity::setType(const TypeNode* t)
{

}

std::vector<Atlas::Objects::Root> LocatedEntity::getThoughts() const
{
    return std::vector<Atlas::Objects::Root>();
}

std::string LocatedEntity::describeEntity() const
{
    return "";
}


void LocatedEntity::broadcast(const Atlas::Objects::Operation::RootOperation& op, OpVector& res) const
{
    auto copy = op.copy();
    copy->setTo(getId());
    res.push_back(copy);
}

void LocatedEntity::collectObservers(std::set<const LocatedEntity*>& observers) const
{

}

void LocatedEntity::processAppearDisappear(std::set<const LocatedEntity*> previousObserving, OpVector& res) const
{

}
void addToEntity(const Point3D & p, std::vector<double> & vd)
{
    vd.resize(3);
    vd[0] = p[0];
    vd[1] = p[1];
    vd[2] = p[2];
}


void log(LogLevel lvl, const std::string & msg)
{
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


float squareDistance(const Point3D & u, const Point3D & v)
{
    return 1.f;
}

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

#include "../allOperations.h"
#include "../TestBase.h"

#include "rules/simulation/Thing.h"

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
    testThing(RouterId id) :
        Thing(id) { }
    using Thing::updateProperties;
};

static const std::string testName("bob");
static const std::string testNewName("fred");

struct TestThing : public Thing
{
    TestThing(RouterId id)
            : Thing(id)
    {
    }

    void test_updateProperties(const Operation& op, OpVector& res) {
        updateProperties(op, res);
    }
};

class ThingupdatePropertiestest : public Cyphesis::TestBase
{
  protected:
    Ref<TestThing> m_thing;
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
    m_name = new Property<std::string>(prop_flag_unsent);
    m_name->data() = testName;

    m_thing = new TestThing(1);
    m_thing->setProperty("name", std::unique_ptr<PropertyBase>(m_name));
}

void ThingupdatePropertiestest::teardown()
{
    m_thing = nullptr;
}

void ThingupdatePropertiestest::test_update()
{
    ASSERT_EQUAL(m_name->flags().m_flags & prop_flag_unsent, prop_flag_unsent);

    Update u;
    OpVector res;
   
    m_thing->test_updateProperties(u, res);

    // The flag marking the property has been cleared
    ASSERT_EQUAL(m_name->flags().m_flags & prop_flag_unsent, 0u);

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

#include "rules/Domain.h"

#include "rules/simulation/BaseWorld.h"
#include "common/const.h"
#include "common/log.h"

#include "../stubs/common/stubcustom.h"
#include "../stubs/modules/stubWeakEntityRef.h"
#include "../stubs/rules/simulation/stubEntity.h"
#include "../stubs/rules/stubDomain.h"
#include "../stubs/common/stubRouter.h"
#include "../stubs/rules/simulation/stubBaseWorld.h"
#include "../stubs/rules/stubLocation.h"
#include "../stubs/rules/simulation/stubPropelProperty.h"
#include "../stubs/rules/simulation/stubDomainProperty.h"
#include "../stubs/rules/simulation/stubEntityProperty.h"
#include "../stubs/rules/simulation/stubModeDataProperty.h"
#include "../stubs/rules/stubPhysicalProperties.h"

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


#include "../stubs/common/stublog.h"


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

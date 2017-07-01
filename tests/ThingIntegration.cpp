// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2016 Erik Ogenvik
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

#include "rulesets/Domain.h"
#include "rulesets/Thing.h"
#include "rulesets/VoidDomain.h"
#include "rulesets/PhysicalDomain.h"
#include "rulesets/InventoryDomain.h"
#include "rulesets/EntityProperty.h"

#include "common/const.h"
#include "common/globals.h"
#include "common/id.h"
#include "common/Inheritance.h"
#include "common/log.h"
#include "common/Monitors.h"
#include "common/SystemTime.h"
#include "common/Tick.h"
#include "common/Variable.h"
#include "common/TypeNode.h"

#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/Operation.h>

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Operation::Tick;

class ThingIntegration : public Cyphesis::TestBase
{
    public:
        ThingIntegration();

        void setup();

        void teardown();

        void test_visibility();
};

class ThingExt : public Thing
{
    public:
        Domain* domain;

        explicit ThingExt(const std::string& id, long intId) :
            Thing::Thing(id, intId), domain(nullptr)
        {
            m_type = new TypeNode(id);
            setFlags(entity_perceptive);
        }

        bool test_lookAtEntity(const Operation& op, OpVector& res, LocatedEntity* watcher) const
        {
            return lookAtEntity(op, res, watcher);
        }

        Domain* getDomain() override
        {
            return domain;
        }

        const Domain* getDomain() const override
        {
            return domain;
        }

        void sendWorld(const Operation& op) override
        {

        }


};

ThingIntegration::ThingIntegration()
{
    ADD_TEST(ThingIntegration::test_visibility);
}

void ThingIntegration::setup()
{
}

void ThingIntegration::teardown()
{
}

void ThingIntegration::test_visibility()
{
    WFMath::AxisBox<3> bbox(WFMath::Point<3>(-10, -10, -10), WFMath::Point<3>(10, 10, 10));

    auto verifyBroadcastContains = [&](ThingExt* thing, std::initializer_list<const ThingExt*> expectedThings) {
        OpVector res;
        Atlas::Objects::Operation::Sight s;
        thing->broadcast(s, res);

        for (auto expectedThing : expectedThings) {
            auto I = std::find_if(std::begin(res), std::end(res), [&](auto entry) { return entry->getTo() == expectedThing->getId(); });
            if (I == std::end(res)) {
                addFailure(String::compose("Could not find entity id '%1' in list of broadcasts.", expectedThing->getId()));
                return false;
            } else {
                res.erase(I);
            }
        }

        if (!res.empty()) {
            for (auto op : res) {
                addFailure(String::compose("Found broadcast op to '%1' which was not expected.", op->getTo()));
            }
            return false;
        }
        return true;
    };

    /**
     * First handle the case where there's no domains at all.
     *
     * All entities are places at origo
     * Hierarchy looks like this:
     *
     *              T1              T4
     *         T2       T5
     *         T3
     *
     *
     * With T2 having a physical domain.
     */
    {
        ThingExt* t1 = new ThingExt("1", 1);
        ThingExt* t2 = new ThingExt("2", 2);
        ThingExt* t3 = new ThingExt("3", 3);
        ThingExt* t4 = new ThingExt("4", 4);
        ThingExt* t5 = new ThingExt("5", 5);
        t1->addChild(*t2);
        t1->addChild(*t5);
        t2->addChild(*t3);
        Operation sightOp;
        OpVector res;
        //T1 can see itself
        ASSERT_TRUE(t1->test_lookAtEntity(sightOp, res, t1));
        //T1 can see T2 since it's a child and there's no domain
        ASSERT_TRUE(t2->test_lookAtEntity(sightOp, res, t1));
        //T1 can see T3 since it's a grandchild and there's no domain
        ASSERT_TRUE(t3->test_lookAtEntity(sightOp, res, t1));
        //T1 can't see T4 since it's not in the same graph
        ASSERT_TRUE(!t4->test_lookAtEntity(sightOp, res, t1));

        //T2 can see T1 since it's a parent and there's no domain
        ASSERT_TRUE(t1->test_lookAtEntity(sightOp, res, t2));
        //T3 can see T1 since it's a grand parent and there's no domain
        ASSERT_TRUE(t1->test_lookAtEntity(sightOp, res, t3));
        //T5 can see T3 since they share T1 as parent/grand parent and there's no domain
        ASSERT_TRUE(t3->test_lookAtEntity(sightOp, res, t5));
        //T4 can't see T1 since it's not in the same graph
        ASSERT_TRUE(!t1->test_lookAtEntity(sightOp, res, t4));


        ASSERT_TRUE(verifyBroadcastContains(t1, {t1}));
        ASSERT_TRUE(verifyBroadcastContains(t2, {t1, t2}));
        ASSERT_TRUE(verifyBroadcastContains(t3, {t1, t2, t3}));
        ASSERT_TRUE(verifyBroadcastContains(t4, {t4}));
        ASSERT_TRUE(verifyBroadcastContains(t5, {t1, t5}));
    }

    /**
     * Then handle the case where there's a Void domain
     *
     * Hierarchy looks like this:
     *
     *              T1
     *              T2*
     *              T3
     *
     * With T2 having a void domain.
     */
    {
        ThingExt* t1 = new ThingExt("1", 1);
        ThingExt* t2 = new ThingExt("2", 2);
        ThingExt* t3 = new ThingExt("3", 3);
        t1->addChild(*t2);
        t2->addChild(*t3);

        t2->domain = new VoidDomain(*t2);
        t2->setFlags(entity_domain);

        Operation sightOp;
        OpVector res;
        //T1 can see itself
        ASSERT_TRUE(t1->test_lookAtEntity(sightOp, res, t1));
        //T1 can see T2 since it's a child and there's no domain
        ASSERT_TRUE(t2->test_lookAtEntity(sightOp, res, t1));
        //T1 can't see T3 since T2 has a Void domain
        ASSERT_TRUE(!t3->test_lookAtEntity(sightOp, res, t1));

        //T2 can see T1 since it's a parent and there's no domain
        ASSERT_TRUE(t1->test_lookAtEntity(sightOp, res, t2));
        //T3 can't see T1 since T2 has a domain
        ASSERT_TRUE(!t1->test_lookAtEntity(sightOp, res, t3));

        //T2 can see itself
        ASSERT_TRUE(t2->test_lookAtEntity(sightOp, res, t2));
        //T2 can see T3 even though T2 has a Void domain, since T2 is the parent
        ASSERT_TRUE(t3->test_lookAtEntity(sightOp, res, t2));

        ASSERT_TRUE(verifyBroadcastContains(t1, {t1}));
        ASSERT_TRUE(verifyBroadcastContains(t2, {t1, t2}));
        ASSERT_TRUE(verifyBroadcastContains(t3, {t2, t3}));

    }

    /**
     * Then handle the case where there's a Physical domain.
     *
     * All entities are placed at origo
     * Hierarchy looks like this:
     *
     *              T1
     *              T2*
     *         T3       T5      T7**      T8***
     *         T4       T6**
     *
     * With T2 having a physical domain, and T6 and T7 having invalid positions.
     * T8 is not perceptive.
     */
    {
        ThingExt* t1 = new ThingExt("1", 1);
//        t1->setAttr()
        ThingExt* t2 = new ThingExt("2", 2);
        t2->m_location.m_pos = WFMath::Point<3>::ZERO();
        t2->m_location.setBBox(bbox);
        ThingExt* t3 = new ThingExt("3", 3);
        t3->m_location.m_pos = WFMath::Point<3>::ZERO();
        t3->m_location.setBBox(bbox);
        ThingExt* t4 = new ThingExt("4", 4);
        t4->m_location.m_pos = WFMath::Point<3>::ZERO();
        t4->m_location.setBBox(bbox);
        ThingExt* t5 = new ThingExt("5", 5);
        t5->m_location.m_pos = WFMath::Point<3>::ZERO();
        t5->m_location.setBBox(bbox);
        ThingExt* t6 = new ThingExt("6", 6);
        ThingExt* t7 = new ThingExt("7", 7);
        ThingExt* t8 = new ThingExt("8", 8);
        t8->m_location.m_pos = WFMath::Point<3>::ZERO();
        t8->m_location.setBBox(bbox);
        t8->resetFlags(entity_perceptive);

        t2->domain = new PhysicalDomain(*t2);
        t2->setFlags(entity_domain);

        t1->addChild(*t2);
        t2->addChild(*t3);
        t2->addChild(*t5);
        t2->addChild(*t7);
        t2->addChild(*t8);
        t5->addChild(*t6);
        t3->addChild(*t4);


        Operation sightOp;
        OpVector res;
        //T1 can see itself
        ASSERT_TRUE(t1->test_lookAtEntity(sightOp, res, t1));
        //T1 can see T2 since it's a child and there's no domain
        ASSERT_TRUE(t2->test_lookAtEntity(sightOp, res, t1));
        //T1 can't see T3 since T2 has a Physical domain and it doesn't allow external entities to look into it.
        ASSERT_TRUE(!t3->test_lookAtEntity(sightOp, res, t1));

        //T2 can see T1 since it's a parent and there's no domain
        ASSERT_TRUE(t1->test_lookAtEntity(sightOp, res, t2));
        //T3 can't see T1 since T2 has a domain
        ASSERT_TRUE(!t1->test_lookAtEntity(sightOp, res, t3));

        //T2 can see itself
        ASSERT_TRUE(t2->test_lookAtEntity(sightOp, res, t2));
        //T2 can see T3 since T2 has a Physical domain which allows it.
        ASSERT_TRUE(t3->test_lookAtEntity(sightOp, res, t2));
        //T5 can see T3 since T2 has a Physical domain which allows it.
        ASSERT_TRUE(t3->test_lookAtEntity(sightOp, res, t5));
        //T5 can see T4 since T2 has a Physical domain which allows it to see T3, and thus T4.
        ASSERT_TRUE(t4->test_lookAtEntity(sightOp, res, t5));

        //T2 can see T6 since the parent of T6 is T5, which can be seen and has no domain.
        ASSERT_TRUE(t6->test_lookAtEntity(sightOp, res, t2));
        //T3 can't see T7 since T2 has a Physical domain and T7 has an invalid pos.
        ASSERT_TRUE(!t7->test_lookAtEntity(sightOp, res, t3));
        //T4 can't see T5 since T4 isn't a direct child of T2
        ASSERT_TRUE(!t5->test_lookAtEntity(sightOp, res, t4));

        ASSERT_TRUE(verifyBroadcastContains(t1, {t1}));
        ASSERT_TRUE(verifyBroadcastContains(t2, {t2, t3, t5, t7, t1}));
        //T7 should not be visible since it has an invalid position
        ASSERT_TRUE(verifyBroadcastContains(t3, {t2, t3, t5,}));
        ASSERT_TRUE(verifyBroadcastContains(t4, {t2, t3, t5, t4}));
        ASSERT_TRUE(verifyBroadcastContains(t5, {t2, t3, t5}));
        ASSERT_TRUE(verifyBroadcastContains(t6, {t2, t6, t3, t5}));
        ASSERT_TRUE(verifyBroadcastContains(t7, {t2, t7}));
        ASSERT_TRUE(verifyBroadcastContains(t8, {t2, t3, t5}));

    }

    /**
     * Then handle the case where there's an Inventory domain.
     *
     * All entities are placed at origo
     * Hierarchy looks like this:
     *
     *              T1
     *              T2*    T6
     *         T3**     T5
     *         T4
     *
     * With T2 having an inventory domain.
     * And T3 being wielded
     */
    {
        ThingExt* t1 = new ThingExt("1", 1);
        ThingExt* t2 = new ThingExt("2", 2);
        ThingExt* t3 = new ThingExt("3", 3);
        ThingExt* t4 = new ThingExt("4", 4);
        ThingExt* t5 = new ThingExt("5", 5);
        ThingExt* t6 = new ThingExt("6", 6);

        t1->addChild(*t2);
        t1->addChild(*t6);
        t2->addChild(*t3);
        t2->addChild(*t5);
        t3->addChild(*t4);

        t2->domain = new InventoryDomain(*t2);
        t2->setFlags(entity_domain);
        auto entityProp = new EntityProperty();
        entityProp->data() = EntityRef(t3);
        t2->setProperty("right_hand_wield", entityProp);

        Operation sightOp;
        OpVector res;
        //T1 can see itself
        ASSERT_TRUE(t1->test_lookAtEntity(sightOp, res, t1));
        //T1 can see T2 since it's a child and there's no domain
        ASSERT_TRUE(t2->test_lookAtEntity(sightOp, res, t1));
        //T1 can see T3 since T2 has an Inventory domain and T3 is wielded.
        ASSERT_TRUE(t3->test_lookAtEntity(sightOp, res, t1));
        //T1 can see T4 since T2 has an Inventory domain and T3 is wielded, and T4 is a child.
        ASSERT_TRUE(t4->test_lookAtEntity(sightOp, res, t1));
        //T1 can't see T5 since T2 has an Inventory domain and T5 isn't wielded.
        ASSERT_TRUE(!t5->test_lookAtEntity(sightOp, res, t1));

        //T6 can see T3 since T2 has an Inventory domain and T3 is wielded.
        ASSERT_TRUE(t3->test_lookAtEntity(sightOp, res, t6));
        //T6 can see T4 since T2 has an Inventory domain and T3 is wielded, and T4 is a child.
        ASSERT_TRUE(t4->test_lookAtEntity(sightOp, res, t6));
        //T6 can't see T5 since T2 has an Inventory domain and T5 isn't wielded.
        ASSERT_TRUE(!t5->test_lookAtEntity(sightOp, res, t6));

        ASSERT_TRUE(verifyBroadcastContains(t1, {t1}));
        ASSERT_TRUE(verifyBroadcastContains(t2, {t2, t1}));
        ASSERT_TRUE(verifyBroadcastContains(t3, {t3, t2, t1}));
        ASSERT_TRUE(verifyBroadcastContains(t4, {t4, t3, t2, t1}));
        ASSERT_TRUE(verifyBroadcastContains(t5, {t5, t2}));
        ASSERT_TRUE(verifyBroadcastContains(t6, {t6, t1}));

    }

    /**
     * And then a more complex case, involving multiple domains.
     *
     * All entities are placed at origo
     * Hierarchy looks like this:
     *
     *                T1
     *                T2*
     *          T3*       T5
     *       T4*   T6
     *
     * With T2 having a Physical domain.
     * With T3 having an Inventory domain.
     * And T4 being wielded
     */
    {
        ThingExt* t1 = new ThingExt("1", 1);
        ThingExt* t2 = new ThingExt("2", 2);
        t2->m_location.m_pos = WFMath::Point<3>::ZERO();
        t2->m_location.setBBox(bbox);
        ThingExt* t3 = new ThingExt("3", 3);
        t3->m_location.m_pos = WFMath::Point<3>::ZERO();
        t3->m_location.setBBox(bbox);
        ThingExt* t4 = new ThingExt("4", 4);
        ThingExt* t5 = new ThingExt("5", 5);
        t5->m_location.m_pos = WFMath::Point<3>::ZERO();
        t5->m_location.setBBox(bbox);
        ThingExt* t6 = new ThingExt("6", 6);

        t2->domain = new PhysicalDomain(*t2);
        t2->setFlags(entity_domain);

        t3->domain = new InventoryDomain(*t3);
        t3->setFlags(entity_domain);

        t1->addChild(*t2);
        t2->addChild(*t3);
        t2->addChild(*t5);
        t3->addChild(*t4);
        t3->addChild(*t6);

        auto entityProp = new EntityProperty();
        entityProp->data() = EntityRef(t4);
        t3->setProperty("right_hand_wield", entityProp);

        Operation sightOp;
        OpVector res;
        //T1 can see itself
        ASSERT_TRUE(t1->test_lookAtEntity(sightOp, res, t1));
        //T1 can see T2 since it's a child and there's no domain
        ASSERT_TRUE(t2->test_lookAtEntity(sightOp, res, t1));
        //T1 can't see T3 since T2 has an Physical domain and T1 is a parent.
        ASSERT_TRUE(!t3->test_lookAtEntity(sightOp, res, t1));
        //T1 can't see T4 since T2 has an Physical domain and T1 is a parent.
        ASSERT_TRUE(!t4->test_lookAtEntity(sightOp, res, t1));

        //T5 can see T3 since T2 has an Physical domain .
        ASSERT_TRUE(t3->test_lookAtEntity(sightOp, res, t5));
        //T5 can see T4 since T2 has an Physical domain, T3 has an Inventory Domain and is close, and T4 is wielded.
        ASSERT_TRUE(t4->test_lookAtEntity(sightOp, res, t5));
        //T5 can't see T6 since T2 has an Physical domain, T3 has an Inventory Domain and is close, and T6 isn't wielded.
        ASSERT_TRUE(!t6->test_lookAtEntity(sightOp, res, t5));

        ASSERT_TRUE(verifyBroadcastContains(t1, {t1}));
        ASSERT_TRUE(verifyBroadcastContains(t2, {t1, t2, t3, t5}));
        ASSERT_TRUE(verifyBroadcastContains(t3, {t2, t3, t5}));
        ASSERT_TRUE(verifyBroadcastContains(t4, {t4, t2, t3, t5}));
        ASSERT_TRUE(verifyBroadcastContains(t5, {t2, t3, t5}));
        ASSERT_TRUE(verifyBroadcastContains(t6, {t6, t3}));

    }

    /**
    * Handle "creator" entities which allows more access.
    *
    * All entities are placed at origo
    * Hierarchy looks like this:
    *
    *                T1*
    *       T2*** creator*   T3
    *    T4**  T5
    *
    * With T1 having a Physical domain.
    * With T2 having an Inventory domain.
    * With "creator" being a "creator"
    * And T4 being wielded
    */
    {
        ThingExt* t1 = new ThingExt("1", 1);
        t1->m_location.m_pos = WFMath::Point<3>::ZERO();
        t1->m_location.setBBox(bbox);
        ThingExt* t2 = new ThingExt("2", 2);
        t2->m_location.m_pos = WFMath::Point<3>::ZERO();
        t2->m_location.setBBox(bbox);
        ThingExt* creator = new ThingExt("creator", 10);
        creator->m_location.m_pos = WFMath::Point<3>::ZERO();
        creator->m_location.setBBox(bbox);
        ThingExt* t3 = new ThingExt("3", 3);
        t3->m_location.m_pos = WFMath::Point<3>::ZERO();
        t3->m_location.setBBox(bbox);
        ThingExt* t4 = new ThingExt("4", 4);
        ThingExt* t5 = new ThingExt("5", 5);

        t1->domain = new PhysicalDomain(*t1);
        t1->setFlags(entity_domain);

        t2->domain = new InventoryDomain(*t2);
        t2->setFlags(entity_domain);

        t1->addChild(*t2);
        t1->addChild(*creator);
        t1->addChild(*t3);
        t2->addChild(*t4);
        t2->addChild(*t5);

        auto entityProp = new EntityProperty();
        entityProp->data() = EntityRef(t4);
        t2->setProperty("right_hand_wield", entityProp);

        Operation sightOp;
        OpVector res;
        //T3 can see t4 since it's wielded
        ASSERT_TRUE(t4->test_lookAtEntity(sightOp, res, t3));
        //"creator" can see t4 since it's wielded
        ASSERT_TRUE(t4->test_lookAtEntity(sightOp, res, creator));
        //T3 can't see t5 since it's not wielded
        ASSERT_FALSE(t5->test_lookAtEntity(sightOp, res, t3));
        //"creator" can see t5 since it's a "creator"
        ASSERT_TRUE(t5->test_lookAtEntity(sightOp, res, creator));

        ASSERT_TRUE(verifyBroadcastContains(t4, {t1, t2, t3, t4, creator}));
        ASSERT_TRUE(verifyBroadcastContains(t5, {t2, t5})); //broadcasts won't be sent to creator, since they will be stopped at T2

    }
}

int main()
{
    ThingIntegration t;

    return t.run();
}

// stubs

static inline WFMath::CoordType sqr(WFMath::CoordType x)
{
    return x * x;
}

WFMath::CoordType squareDistance(const Point3D& u, const Point3D& v)
{
    return (sqr(u.x() - v.x()) + sqr(u.y() - v.y()) + sqr(u.z() - v.z()));
}

void addToEntity(const Point3D& p, std::vector<double>& vd)
{
}

void addToEntity(const Vector3D& v, std::vector<double>& vd)
{
}

template<>
int fromStdVector<double>(Point3D& p, const std::vector<double>& vf)
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

template<>
int fromStdVector<double>(Vector3D& v, const std::vector<double>& vf)
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

WFMath::CoordType sqrMag(const Point3D& p)
{
    return 0;
}

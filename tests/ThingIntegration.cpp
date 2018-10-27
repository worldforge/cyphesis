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

#include "common/Inheritance.h"
#include "common/Tick.h"

#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/Operation.h>
#include <rulesets/AttachmentsProperty.h>
#include <rulesets/PlantedOnProperty.h>

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Operation::Tick;

class ThingIntegration : public Cyphesis::TestBase
{
    public:
        ThingIntegration();

        void setup() override;

        void teardown() override;

        void test_visibility();

        void test_reachability();
};

class ThingExt : public Thing
{
    public:
        Domain* domain;

        explicit ThingExt(const std::string& id, long intId) :
            Thing::Thing(id, intId), domain(nullptr)
        {
            m_type = new TypeNode(id);
            addFlags(entity_perceptive);
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
    ADD_TEST(ThingIntegration::test_reachability);
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

    auto verifyBroadcastContains = [&](Ref<ThingExt> thing, std::initializer_list<const Ref<ThingExt>> expectedThings) {
        OpVector res;
        Atlas::Objects::Operation::Sight s;
        thing->broadcast(s, res, Visibility::PUBLIC);

        for (auto expectedThing : expectedThings) {
            auto I = std::find_if(std::begin(res), std::end(res), [&](Operation entry) { return entry->getTo() == expectedThing->getId(); });
            if (I == std::end(res)) {
                addFailure(String::compose("Could not find entity id '%1' in list of broadcasts.", expectedThing->getId()));
                return false;
            } else {
                res.erase(I);
            }
        }

        if (!res.empty()) {
            for (auto& op : res) {
                addFailure(String::compose("Found broadcast op to '%1' which was not expected.", op->getTo()));
            }
            return false;
        }
        return true;
    };

    /**
     * First handle the case where there's no domains at all.
     *
     * All entities are placed at origo
     * Hierarchy looks like this:
     *
     *              T1              T4
     *         T2       T5
     *         T3
     *
     *
     */
    {
        Ref<ThingExt> t1 = new ThingExt("1", 1);
        Ref<ThingExt> t2 = new ThingExt("2", 2);
        Ref<ThingExt> t3 = new ThingExt("3", 3);
        Ref<ThingExt> t4 = new ThingExt("4", 4);
        Ref<ThingExt> t5 = new ThingExt("5", 5);
        t1->addChild(*t2);
        t1->addChild(*t5);
        t2->addChild(*t3);
        Operation sightOp;
        OpVector res;
        //T1 can see itself
        ASSERT_TRUE(t1->test_lookAtEntity(sightOp, res, t1.get()));
        //T1 can see T2 since it's a child and there's no domain
        ASSERT_TRUE(t2->test_lookAtEntity(sightOp, res, t1.get()));
        //T1 can see T3 since it's a grandchild and there's no domain
        ASSERT_TRUE(t3->test_lookAtEntity(sightOp, res, t1.get()));
        //T1 can't see T4 since it's not in the same graph
        ASSERT_FALSE(t4->test_lookAtEntity(sightOp, res, t1.get()));

        //T2 can see T1 since it's a parent and there's no domain
        ASSERT_TRUE(t1->test_lookAtEntity(sightOp, res, t2.get()));
        //T3 can see T1 since it's a grand parent and there's no domain
        ASSERT_TRUE(t1->test_lookAtEntity(sightOp, res, t3.get()));
        //T5 can see T3 since they share T1 as parent/grand parent and there's no domain
        ASSERT_TRUE(t3->test_lookAtEntity(sightOp, res, t5.get()));
        //T4 can't see T1 since it's not in the same graph
        ASSERT_FALSE(t1->test_lookAtEntity(sightOp, res, t4.get()));


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
        Ref<ThingExt> t1 = new ThingExt("1", 1);
        Ref<ThingExt> t2 = new ThingExt("2", 2);
        Ref<ThingExt> t3 = new ThingExt("3", 3);
        t1->addChild(*t2);
        t2->addChild(*t3);

        t2->domain = new VoidDomain(*t2);
        t2->addFlags(entity_domain);

        Operation sightOp;
        OpVector res;
        //T1 can see itself
        ASSERT_TRUE(t1->test_lookAtEntity(sightOp, res, t1.get()));
        //T1 can see T2 since it's a child and there's no domain
        ASSERT_TRUE(t2->test_lookAtEntity(sightOp, res, t1.get()));
        //T1 can't see T3 since T2 has a Void domain
        ASSERT_FALSE(t3->test_lookAtEntity(sightOp, res, t1.get()));

        //T2 can see T1 since it's a parent and there's no domain
        ASSERT_TRUE(t1->test_lookAtEntity(sightOp, res, t2.get()));
        //T3 can't see T1 since T2 has a domain
        ASSERT_FALSE(t1->test_lookAtEntity(sightOp, res, t3.get()));

        //T2 can see itself
        ASSERT_TRUE(t2->test_lookAtEntity(sightOp, res, t2.get()));
        //T2 can see T3 even though T2 has a Void domain, since T2 is the parent
        ASSERT_TRUE(t3->test_lookAtEntity(sightOp, res, t2.get()));

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
        Ref<ThingExt> t1 = new ThingExt("1", 1);
//        t1->setAttr()
        Ref<ThingExt> t2 = new ThingExt("2", 2);
        t2->m_location.m_pos = WFMath::Point<3>::ZERO();
        t2->m_location.setBBox(bbox);
        Ref<ThingExt> t3 = new ThingExt("3", 3);
        t3->m_location.m_pos = WFMath::Point<3>::ZERO();
        t3->m_location.setBBox(bbox);
        Ref<ThingExt> t4 = new ThingExt("4", 4);
        t4->m_location.m_pos = WFMath::Point<3>::ZERO();
        t4->m_location.setBBox(bbox);
        Ref<ThingExt> t5 = new ThingExt("5", 5);
        t5->m_location.m_pos = WFMath::Point<3>::ZERO();
        t5->m_location.setBBox(bbox);
        Ref<ThingExt> t6 = new ThingExt("6", 6);
        Ref<ThingExt> t7 = new ThingExt("7", 7);
        Ref<ThingExt> t8 = new ThingExt("8", 8);
        t8->m_location.m_pos = WFMath::Point<3>::ZERO();
        t8->m_location.setBBox(bbox);
        t8->removeFlags(entity_perceptive);

        t2->domain = new PhysicalDomain(*t2);
        t2->addFlags(entity_domain);

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
        ASSERT_TRUE(t1->test_lookAtEntity(sightOp, res, t1.get()));
        //T1 can see T2 since it's a child and there's no domain
        ASSERT_TRUE(t2->test_lookAtEntity(sightOp, res, t1.get()));
        //T1 can't see T3 since T2 has a Physical domain and it doesn't allow external entities to look into it.
        ASSERT_FALSE(t3->test_lookAtEntity(sightOp, res, t1.get()));

        //T2 can see T1 since it's a parent and there's no domain
        ASSERT_TRUE(t1->test_lookAtEntity(sightOp, res, t2.get()));
        //T3 can't see T1 since T2 has a domain
        ASSERT_FALSE(t1->test_lookAtEntity(sightOp, res, t3.get()));

        //T2 can see itself
        ASSERT_TRUE(t2->test_lookAtEntity(sightOp, res, t2.get()));
        //T2 can see T3 since T2 has a Physical domain which allows it.
        ASSERT_TRUE(t3->test_lookAtEntity(sightOp, res, t2.get()));
        //T5 can see T3 since T2 has a Physical domain which allows it.
        ASSERT_TRUE(t3->test_lookAtEntity(sightOp, res, t5.get()));
        //T5 can see T4 since T2 has a Physical domain which allows it to see T3, and thus T4.
        ASSERT_TRUE(t4->test_lookAtEntity(sightOp, res, t5.get()));

        //T2 can see T6 since the parent of T6 is T5, which can be seen and has no domain.
        ASSERT_TRUE(t6->test_lookAtEntity(sightOp, res, t2.get()));
        //T3 can't see T7 since T2 has a Physical domain and T7 has an invalid pos.
        ASSERT_FALSE(t7->test_lookAtEntity(sightOp, res, t3.get()));
        //T4 can't see T5 since T4 isn't a direct child of T2
        ASSERT_FALSE(t5->test_lookAtEntity(sightOp, res, t4.get()));

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
        Ref<ThingExt> t1 = new ThingExt("1", 1);
        Ref<ThingExt> t2 = new ThingExt("2", 2);
        Ref<ThingExt> t3 = new ThingExt("3", 3);
        Ref<ThingExt> t4 = new ThingExt("4", 4);
        Ref<ThingExt> t5 = new ThingExt("5", 5);
        Ref<ThingExt> t6 = new ThingExt("6", 6);

        t1->addChild(*t2);
        t1->addChild(*t6);
        t2->addChild(*t3);
        t2->addChild(*t5);
        t3->addChild(*t4);

        t2->domain = new InventoryDomain(*t2);
        t2->addFlags(entity_domain);

        auto plantedOnProp = new PlantedOnProperty();
        plantedOnProp->data().entity = t2;
        t3->setProperty(PlantedOnProperty::property_name, plantedOnProp);

        Operation sightOp;
        OpVector res;
        //T1 can see itself
        ASSERT_TRUE(t1->test_lookAtEntity(sightOp, res, t1.get()));
        //T1 can see T2 since it's a child and there's no domain
        ASSERT_TRUE(t2->test_lookAtEntity(sightOp, res, t1.get()));
        //T1 can see T3 since T2 has an Inventory domain and T3 is wielded.
        ASSERT_TRUE(t3->test_lookAtEntity(sightOp, res, t1.get()));
        //T1 can see T4 since T2 has an Inventory domain and T3 is wielded, and T4 is a child.
        ASSERT_TRUE(t4->test_lookAtEntity(sightOp, res, t1.get()));
        //T1 can't see T5 since T2 has an Inventory domain and T5 isn't wielded.
        ASSERT_FALSE(t5->test_lookAtEntity(sightOp, res, t1.get()));

        //T6 can see T3 since T2 has an Inventory domain and T3 is wielded.
        ASSERT_TRUE(t3->test_lookAtEntity(sightOp, res, t6.get()));
        //T6 can see T4 since T2 has an Inventory domain and T3 is wielded, and T4 is a child.
        ASSERT_TRUE(t4->test_lookAtEntity(sightOp, res, t6.get()));
        //T6 can't see T5 since T2 has an Inventory domain and T5 isn't wielded.
        ASSERT_FALSE(t5->test_lookAtEntity(sightOp, res, t6.get()));

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
        Ref<ThingExt> t1 = new ThingExt("1", 1);
        Ref<ThingExt> t2 = new ThingExt("2", 2);
        t2->m_location.m_pos = WFMath::Point<3>::ZERO();
        t2->m_location.setBBox(bbox);
        Ref<ThingExt> t3 = new ThingExt("3", 3);
        t3->m_location.m_pos = WFMath::Point<3>::ZERO();
        t3->m_location.setBBox(bbox);
        Ref<ThingExt> t4 = new ThingExt("4", 4);
        Ref<ThingExt> t5 = new ThingExt("5", 5);
        t5->m_location.m_pos = WFMath::Point<3>::ZERO();
        t5->m_location.setBBox(bbox);
        Ref<ThingExt> t6 = new ThingExt("6", 6);

        t2->domain = new PhysicalDomain(*t2);
        t2->addFlags(entity_domain);

        t3->domain = new InventoryDomain(*t3);
        t3->addFlags(entity_domain);

        t1->addChild(*t2);
        t2->addChild(*t3);
        t2->addChild(*t5);
        t3->addChild(*t4);
        t3->addChild(*t6);

        auto plantedOnProp = new PlantedOnProperty();
        plantedOnProp->data().entity = t3;
        t4->setProperty(PlantedOnProperty::property_name, plantedOnProp);

        Operation sightOp;
        OpVector res;
        //T1 can see itself
        ASSERT_TRUE(t1->test_lookAtEntity(sightOp, res, t1.get()));
        //T1 can see T2 since it's a child and there's no domain
        ASSERT_TRUE(t2->test_lookAtEntity(sightOp, res, t1.get()));
        //T1 can't see T3 since T2 has an Physical domain and T1 is a parent.
        ASSERT_FALSE(t3->test_lookAtEntity(sightOp, res, t1.get()));
        //T1 can't see T4 since T2 has an Physical domain and T1 is a parent.
        ASSERT_FALSE(t4->test_lookAtEntity(sightOp, res, t1.get()));

        //T5 can see T3 since T2 has an Physical domain .
        ASSERT_TRUE(t3->test_lookAtEntity(sightOp, res, t5.get()));
        //T5 can see T4 since T2 has an Physical domain, T3 has an Inventory Domain and is close, and T4 is wielded.
        ASSERT_TRUE(t4->test_lookAtEntity(sightOp, res, t5.get()));
        //T5 can't see T6 since T2 has an Physical domain, T3 has an Inventory Domain and is close, and T6 isn't wielded.
        ASSERT_FALSE(t6->test_lookAtEntity(sightOp, res, t5.get()));

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
        Ref<ThingExt> t1 = new ThingExt("1", 1);
        t1->m_location.m_pos = WFMath::Point<3>::ZERO();
        t1->m_location.setBBox(bbox);
        Ref<ThingExt> t2 = new ThingExt("2", 2);
        t2->m_location.m_pos = WFMath::Point<3>::ZERO();
        t2->m_location.setBBox(bbox);
        Ref<ThingExt> creator = new ThingExt("creator", 10);
        creator->m_location.m_pos = WFMath::Point<3>::ZERO();
        creator->m_location.setBBox(bbox);
        creator->addFlags(entity_admin);

        Ref<ThingExt> t3 = new ThingExt("3", 3);
        t3->m_location.m_pos = WFMath::Point<3>::ZERO();
        t3->m_location.setBBox(bbox);
        Ref<ThingExt> t4 = new ThingExt("4", 4);
        Ref<ThingExt> t5 = new ThingExt("5", 5);

        t1->domain = new PhysicalDomain(*t1);
        t1->addFlags(entity_domain);

        t2->domain = new InventoryDomain(*t2);
        t2->addFlags(entity_domain);

        t1->addChild(*t2);
        t1->addChild(*creator);
        t1->addChild(*t3);
        t2->addChild(*t4);
        t2->addChild(*t5);

        auto plantedOnProp = new PlantedOnProperty();
        plantedOnProp->data().entity = t2;
        t4->setProperty(PlantedOnProperty::property_name, plantedOnProp);


        Operation sightOp;
        OpVector res;
        //T3 can see t4 since it's wielded
        ASSERT_TRUE(t4->test_lookAtEntity(sightOp, res, t3.get()));
        //"creator" can see t4 since it's wielded
        ASSERT_TRUE(t4->test_lookAtEntity(sightOp, res, creator.get()));
        //T3 can't see t5 since it's not wielded
        ASSERT_FALSE(t5->test_lookAtEntity(sightOp, res, t3.get()));
        //"creator" can see t5 since it's a "creator"
        ASSERT_TRUE(t5->test_lookAtEntity(sightOp, res, creator.get()));

        ASSERT_TRUE(verifyBroadcastContains(t4, {t1, t2, t3, t4, creator}));
        ASSERT_TRUE(verifyBroadcastContains(t5, {t2, t5})); //broadcasts won't be sent to creator, since they will be stopped at T2

    }
}


void ThingIntegration::test_reachability()
{
    WFMath::AxisBox<3> bbox(WFMath::Point<3>(-10, -10, -10), WFMath::Point<3>(10, 10, 10));

    auto createReachPropFn = [](double reach) {
        auto reachProp = new Property<double>();
        reachProp->data() = reach;
        return reachProp;
    };

    /**
     * First handle the case where there's no domains at all.
     *
     * All entities are placed at origo
     * Hierarchy looks like this:
     *
     *              T1              T4
     *         T2       T5
     *         T3
     *
     *
     */
    {
        Ref<ThingExt> t1 = new ThingExt("1", 1);
        Ref<ThingExt> t2 = new ThingExt("2", 2);
        Ref<ThingExt> t3 = new ThingExt("3", 3);
        Ref<ThingExt> t4 = new ThingExt("4", 4);
        Ref<ThingExt> t5 = new ThingExt("5", 5);
        t1->addChild(*t2);
        t1->addChild(*t5);
        t2->addChild(*t3);
        //T1 can reach itself
        ASSERT_TRUE(t1->canReach({t1, {}}));
        //T1 can reach T2 since it's a child and there's no domain
        ASSERT_TRUE(t1->canReach({t2, {}}));
        //T1 can reach T3 since it's a grandchild and there's no domain
        ASSERT_TRUE(t1->canReach({t3, {}}));
        //T1 can't reach T4 since it's not in the same graph
        ASSERT_FALSE(t1->canReach({t4, {}}));

        //T2 can reach T1 since it's a parent and there's no domain
        ASSERT_TRUE(t2->canReach({t1, {}}));
        //T3 can reach T1 since it's a grand parent and there's no domain
        ASSERT_TRUE(t3->canReach({t1, {}}));
        //T5 can reach T3 since they share T1 as parent/grand parent and there's no domain
        ASSERT_TRUE(t5->canReach({t3, {}}));
        //T4 can't reach T1 since it's not in the same graph
        ASSERT_FALSE(t4->canReach({t1, {}}));
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
        Ref<ThingExt> t1 = new ThingExt("1", 1);
        Ref<ThingExt> t2 = new ThingExt("2", 2);
        Ref<ThingExt> t3 = new ThingExt("3", 3);
        t1->addChild(*t2);
        t2->addChild(*t3);

        t2->domain = new VoidDomain(*t2);
        t2->addFlags(entity_domain);

        //T1 can reach itself
        ASSERT_TRUE(t1->canReach({t1, {}}));
        //T1 can reach T2 since it's a child and there's no domain
        ASSERT_TRUE(t1->canReach({t2, {}}));
        //T1 can't reach T3 since T2 has a Void domain
        ASSERT_FALSE(t1->canReach({t3, {}}));

        //T2 can reach T1 since it's a parent and there's no domain
        ASSERT_TRUE(t2->canReach({t1, {}}));
        //T3 can't reach T1 since T2 has a domain
        ASSERT_FALSE(t3->canReach({t1, {}}));

        //T2 can reach itself
        ASSERT_TRUE(t2->canReach({t2, {}}));
        //T2 can reach T3 even though T2 has a Void domain, since T2 is the parent
        ASSERT_TRUE(t2->canReach({t3, {}}));


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
        Ref<ThingExt> t1 = new ThingExt("1", 1);
        Ref<ThingExt> t2 = new ThingExt("2", 2);
        t2->m_location.m_pos = WFMath::Point<3>::ZERO();
        t2->m_location.setBBox(bbox);
        Ref<ThingExt> t3 = new ThingExt("3", 3);
        t3->m_location.m_pos = WFMath::Point<3>::ZERO();
        t3->m_location.setBBox(bbox);
        Ref<ThingExt> t4 = new ThingExt("4", 4);
        t4->m_location.m_pos = WFMath::Point<3>::ZERO();
        t4->m_location.setBBox(bbox);
        Ref<ThingExt> t5 = new ThingExt("5", 5);
        t5->m_location.m_pos = WFMath::Point<3>::ZERO();
        t5->m_location.setBBox(bbox);
        t5->setProperty("reach", createReachPropFn(10));
        Ref<ThingExt> t6 = new ThingExt("6", 6);
        Ref<ThingExt> t7 = new ThingExt("7", 7);
        Ref<ThingExt> t8 = new ThingExt("8", 8);
        t8->m_location.m_pos = WFMath::Point<3>::ZERO();
        t8->m_location.setBBox(bbox);
        t8->removeFlags(entity_perceptive);

        t2->domain = new PhysicalDomain(*t2);
        t2->addFlags(entity_domain);

        t1->addChild(*t2);
        t2->addChild(*t3);
        t2->addChild(*t5);
        t2->addChild(*t7);
        t2->addChild(*t8);
        t5->addChild(*t6);
        t3->addChild(*t4);

        //T1 can reach itself
        ASSERT_TRUE( t1->canReach({t1, {}}));
        //T1 can reach T2 since it's a child and there's no domain
        ASSERT_TRUE( t1->canReach({t2, {}}));
        //T1 can't reach T3 since T2 has a Physical domain and it doesn't allow external entities to reach into it.
        ASSERT_FALSE( t1->canReach({t3, {}}));

        //T2 can reach T1 since it's a parent and there's no domain
        ASSERT_TRUE( t2->canReach({t1, {}}));
        //T3 can't reach T1 since T2 has a domain
        ASSERT_FALSE( t3->canReach({t1, {}}));

        //T2 can reach itself
        ASSERT_TRUE( t2->canReach({t2, {}}));
        //T2 can reach T3 since T2 has a Physical domain which allows it.
        ASSERT_TRUE( t2->canReach({t3, {}}));
        //T5 can reach T3 since T2 has a Physical domain which allows it.
        ASSERT_TRUE( t5->canReach({t3, {}}));
        //T5 can reach T4 since T2 has a Physical domain which allows it to reach T3, and thus T4.
        ASSERT_TRUE( t5->canReach({t4, {}}));

        //T2 can reach T6 since the parent of T6 is T5, which can be reached and has no domain.
        ASSERT_TRUE( t2->canReach({t6, {}}));
        //T3 can't reach T7 since T2 has a Physical domain and T7 has an invalid pos.
        ASSERT_FALSE( t3->canReach({t7, {}}));
        //T4 can't reach T5 since T4 isn't a direct child of T2
        ASSERT_FALSE( t4->canReach({t5, {}}));


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
        Ref<ThingExt> t1 = new ThingExt("1", 1);
        Ref<ThingExt> t2 = new ThingExt("2", 2);
        Ref<ThingExt> t3 = new ThingExt("3", 3);
        Ref<ThingExt> t4 = new ThingExt("4", 4);
        Ref<ThingExt> t5 = new ThingExt("5", 5);
        Ref<ThingExt> t6 = new ThingExt("6", 6);

        t1->addChild(*t2);
        t1->addChild(*t6);
        t2->addChild(*t3);
        t2->addChild(*t5);
        t3->addChild(*t4);

        t2->domain = new InventoryDomain(*t2);
        t2->addFlags(entity_domain);
        auto entityProp = new EntityProperty();
        entityProp->data() = WeakEntityRef(t3);
        t2->setProperty("right_hand_wield", entityProp);

        //T1 can reach itself
        ASSERT_TRUE(t1->canReach({t1, {}}));
        //T1 can reach T2 since it's a child and there's no domain
        ASSERT_TRUE(t1->canReach({t2, {}}));
        //T1 can't reach T3 since T2 has an Inventory domain, even though T3 is wielded.
        ASSERT_FALSE(t1->canReach({t3, {}}));
        //T1 can't reach T4 since T2 has an Inventory domain, even though T3 is wielded, and T4 is a child.
        ASSERT_FALSE(t1->canReach({t4, {}}));
        //T1 can't reach T5 since T2 has an Inventory domain.
        ASSERT_FALSE(t1->canReach({t5, {}}));

        //T6 can't reach T3 since T2 has an Inventory domain, even though T3 is wielded.
        ASSERT_FALSE(t6->canReach({t3, {}}));
        //T6 can't reach T4 since T2 has an Inventory domain, even though T3 is wielded, and T4 is a child.
        ASSERT_FALSE(t6->canReach({t4, {}}));
        //T6 can't reach T5 since T2 has an Inventory domain.
        ASSERT_FALSE(t6->canReach({t5, {}}));


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
        Ref<ThingExt> t1 = new ThingExt("1", 1);
        Ref<ThingExt> t2 = new ThingExt("2", 2);
        t2->m_location.m_pos = WFMath::Point<3>::ZERO();
        t2->m_location.setBBox(bbox);
        Ref<ThingExt> t3 = new ThingExt("3", 3);
        t3->m_location.m_pos = WFMath::Point<3>::ZERO();
        t3->m_location.setBBox(bbox);
        Ref<ThingExt> t4 = new ThingExt("4", 4);
        Ref<ThingExt> t5 = new ThingExt("5", 5);
        t5->m_location.m_pos = WFMath::Point<3>::ZERO();
        t5->m_location.setBBox(bbox);
        t5->setProperty("reach", createReachPropFn(10));
        Ref<ThingExt> t6 = new ThingExt("6", 6);

        t2->domain = new PhysicalDomain(*t2);
        t2->addFlags(entity_domain);

        t3->domain = new InventoryDomain(*t3);
        t3->addFlags(entity_domain);

        t1->addChild(*t2);
        t2->addChild(*t3);
        t2->addChild(*t5);
        t3->addChild(*t4);
        t3->addChild(*t6);

        auto entityProp = new EntityProperty();
        entityProp->data() = WeakEntityRef(t4);
        t3->setProperty("right_hand_wield", entityProp);

        //T1 can reach itself
        ASSERT_TRUE(t1->canReach({t1, {}}));
        //T1 can reach T2 since it's a child and there's no domain
        ASSERT_TRUE(t1->canReach({t2, {}}));
        //T1 can't reach T3 since T2 has an Physical domain and T1 is a parent.
        ASSERT_FALSE(t1->canReach({t3, {}}));
        //T1 can't reach T4 since T2 has an Physical domain and T1 is a parent.
        ASSERT_FALSE(t1->canReach({t4, {}}));

        //T5 can reach T3 since T2 has an Physical domain .
        ASSERT_TRUE(t5->canReach({t3, {}}));
        //T5 can't reach T4 since T2 has an Physical domain, T3 has an Inventory Domain and is close, even though T4 is wielded.
        ASSERT_FALSE(t5->canReach({t4, {}}));
        //T5 can't reach T6 since T2 has an Physical domain, T3 has an Inventory Domain.
        ASSERT_FALSE(t5->canReach({t6, {}}));

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
        Ref<ThingExt> t1 = new ThingExt("1", 1);
        t1->m_location.m_pos = WFMath::Point<3>::ZERO();
        t1->m_location.setBBox(bbox);
        Ref<ThingExt> t2 = new ThingExt("2", 2);
        t2->m_location.m_pos = WFMath::Point<3>::ZERO();
        t2->m_location.setBBox(bbox);
        Ref<ThingExt> creator = new ThingExt("creator", 10);
        creator->m_location.m_pos = WFMath::Point<3>::ZERO();
        creator->m_location.setBBox(bbox);
        Ref<ThingExt> t3 = new ThingExt("3", 3);
        t3->m_location.m_pos = WFMath::Point<3>::ZERO();
        t3->m_location.setBBox(bbox);
        Ref<ThingExt> t4 = new ThingExt("4", 4);
        Ref<ThingExt> t5 = new ThingExt("5", 5);

        t1->domain = new PhysicalDomain(*t1);
        t1->addFlags(entity_domain);

        t2->domain = new InventoryDomain(*t2);
        t2->addFlags(entity_domain);

        t1->addChild(*t2);
        t1->addChild(*creator);
        t1->addChild(*t3);
        t2->addChild(*t4);
        t2->addChild(*t5);

        auto entityProp = new EntityProperty();
        entityProp->data() = WeakEntityRef(t4);
        t2->setProperty("right_hand_wield", entityProp);

        //T3 can't reach t4 even though it's wielded
        ASSERT_FALSE(t3->canReach({t4, {}}));
        //"creator" can reach t4 since it's a "creator"
        ASSERT_TRUE(creator->canReach({t4, {}}));
        //T3 can't reach t5
        ASSERT_FALSE(t3->canReach({t5, {}}));
        //"creator" can reach t5 since it's a "creator"
        ASSERT_TRUE(creator->canReach({t5, {}}));

    }


    /**
    * Handle reaching entities in the same physical domain based on distance.
    *
    * Hierarchy looks like this:
    *
    *                T1*
    *       T2       T3      T4
    *
    * With T1 having a Physical domain.
    * And T2, T3 placed near each other, while T4 is placed a bit away.
    * T2 has no reach, T3 has a reach of 20 meters
    */
    {
        WFMath::AxisBox<3> smallBbox = {{-1, -1, -1},
                                        {1,  1,  1}};
        Ref<ThingExt> t1 = new ThingExt("1", 1);
        t1->m_location.m_pos = WFMath::Point<3>::ZERO();
        t1->m_location.setBBox({{-200, -200, -200}, {200, 200, 200}});
        Ref<ThingExt> t2 = new ThingExt("2", 2);
        t2->m_location.m_pos = {5, 0, 5};
        t2->m_location.setBBox(smallBbox);
        Ref<ThingExt> t3 = new ThingExt("3", 3);
        t3->m_location.m_pos = {10, 0, 10};
        t3->m_location.setBBox(smallBbox);
        auto reachProp = new Property<double>();
        reachProp->data() = 20.f;
        t3->setProperty("reach", reachProp);
        Ref<ThingExt> t4 = new ThingExt("4", 4);
        t4->m_location.m_pos = {100, 0, 100};
        t4->m_location.setBBox(smallBbox);

        t1->domain = new PhysicalDomain(*t1);
        t1->addFlags(entity_domain);

        t1->addChild(*t2);
        t1->addChild(*t3);
        t1->addChild(*t4);

        //T2 can't reach t3 since t2 has no reach
        ASSERT_FALSE(t2->canReach({t3, {}}));
        //T3 can reach t2 since they are close
        ASSERT_TRUE(t3->canReach({t2, {}}));
        //T3 can't reach t4 since it's far away
        ASSERT_FALSE(t3->canReach({t4, {}}));
        //T2 can't reach t4 since it's far away and t2 has no reach
        ASSERT_FALSE(t2->canReach({t4, {}}));
        //T3 can reach a close point in t1
        ASSERT_TRUE(t3->canReach({t1, {9, 0, 9}}));
        //T3 can't reach a far away point in t1
        ASSERT_FALSE(t3->canReach({t1, {90, 0, 90}}));
        //T2 can't reach a close point in t1 since it has no reach
        ASSERT_FALSE(t2->canReach({t1, {6, 0, 6}}));



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

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

#include "../TestBaseWithContext.h"

#include "rules/Domain.h"
#include "rules/simulation/Thing.h"
#include "rules/simulation/VoidDomain.h"
#include "rules/simulation/PhysicalDomain.h"
#include "rules/simulation/InventoryDomain.h"
#include "rules/simulation/EntityProperty.h"

#include "common/Inheritance.h"

#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/Operation.h>
#include <rules/simulation/AttachmentsProperty.h>
#include <rules/simulation/ModeDataProperty.h>
#include <rules/BBoxProperty.h>

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Entity::RootEntity;

struct ThingExt;

//Keep track of all created things and make sure they are destroyed when the Context is destroyed.
//This is needed to avoid recursive references, where an entity refers to its children, and the children to their parent.
//This might be removed if we instead store "parent" as a simple pointer.
static std::vector<Ref<ThingExt>> things;

struct ThingExt : public Thing
{
    std::unique_ptr<Domain> domain;

    explicit ThingExt(RouterId id)
            : Thing::Thing(id)
    {
        m_type = new TypeNode(id.m_id);
        addFlags(entity_perceptive);
        things.emplace_back(Ref<ThingExt>(this));
    }

    ~ThingExt()
    {
        delete m_type;
        m_type = nullptr;
        clearProperties();
    }

    bool test_lookAtEntity(const Operation& op, OpVector& res, LocatedEntity* watcher) const
    {
        return lookAtEntity(op, res, *watcher);
    }

    bool test_lookAtEntity(const Operation& op, OpVector& res, const Ref<LocatedEntity>& watcher) const
    {
        return lookAtEntity(op, res, *watcher);
    }

    Domain* getDomain() override
    {
        return domain.get();
    }

    const Domain* getDomain() const override
    {
        return domain.get();
    }

    void sendWorld(Operation op) override
    {
    }

    void destroy() override
    {
        m_parent = nullptr;
        if (m_contains) {
            m_contains->clear();
        }
    }
};

struct Context
{
    ~Context()
    {
        for (auto thing : things) {
            thing->destroy();
        }
        things.clear();
    }
};

struct ThingIntegration : public Cyphesis::TestBaseWithContext<Context>
{
    ThingIntegration()
    {
        ADD_TEST(test_visibility_levels);
        ADD_TEST(test_reachability);
        ADD_TEST(test_visibility);
    }

    void test_visibility_levels(Context& context)
    {
        WFMath::AxisBox<3> bbox(WFMath::Point<3>(-10, -10, -10), WFMath::Point<3>(10, 10, 10));

        /**
           * Handle the case where there's no domains at all.
           *
           * All entities are placed at origo
           * Hierarchy looks like this:
           * T2, T3 and T5 have visibility "protected"
           * T4 has visibility "private"
           * T6 is an admin
           *
           *              T1
           *         T2*      T5*   T6***
           *         T3*      T4**
           *         T7       T8
           *
           */
        {
            Ref<ThingExt> t1(new ThingExt(1));
            Ref<ThingExt> t2(new ThingExt(2));
            t2->addFlags(entity_visibility_protected);
            Ref<ThingExt> t3(new ThingExt(3));
            t3->addFlags(entity_visibility_protected);
            Ref<ThingExt> t4(new ThingExt(4));
            t4->addFlags(entity_visibility_private);
            Ref<ThingExt> t5(new ThingExt(5));
            t5->addFlags(entity_visibility_protected);
            Ref<ThingExt> t6(new ThingExt(6));
            t6->addFlags(entity_admin);
            Ref<ThingExt> t7(new ThingExt(7));
            Ref<ThingExt> t8(new ThingExt(8));
            t1->addChild(*t2);
            t1->addChild(*t5);
            t1->addChild(*t6);
            t2->addChild(*t3);
            t5->addChild(*t4);
            t3->addChild(*t7);
            t4->addChild(*t8);
            Operation sightOp;
            OpVector res;
            // Everyone can see themselves
            ASSERT_TRUE(t1->test_lookAtEntity(sightOp, res, t1.get()));
            ASSERT_TRUE(t2->test_lookAtEntity(sightOp, res, t2.get()));
            ASSERT_TRUE(t3->test_lookAtEntity(sightOp, res, t3.get()));
            ASSERT_TRUE(t4->test_lookAtEntity(sightOp, res, t4.get()));
            ASSERT_TRUE(t5->test_lookAtEntity(sightOp, res, t5.get()));
            ASSERT_TRUE(t6->test_lookAtEntity(sightOp, res, t6.get()));
            ASSERT_TRUE(t7->test_lookAtEntity(sightOp, res, t7.get()));

            // T1 can see T2 even though it's protected since it's a child
            ASSERT_TRUE(t2->test_lookAtEntity(sightOp, res, t1.get()));
            // T1 can't see T3 since it's protected and not a direct child
            ASSERT_FALSE(t3->test_lookAtEntity(sightOp, res, t1.get()));
            // T5 can't see T2 since it's protected, and not a child
            ASSERT_FALSE(t2->test_lookAtEntity(sightOp, res, t5.get()));
            // T5 can't see T4 since it's private
            ASSERT_FALSE(t4->test_lookAtEntity(sightOp, res, t5.get()));
            // T1 can't see T7 since it can't see T3
            ASSERT_FALSE(t7->test_lookAtEntity(sightOp, res, t1.get()));
            // T7 can see T1 since it's an ancestor
            ASSERT_TRUE(t1->test_lookAtEntity(sightOp, res, t7.get()));
            // T3 can see T2 since T3 is a child
            ASSERT_TRUE(t2->test_lookAtEntity(sightOp, res, t3.get()));
            // T8 can see T4, even though it's private, since T8 is a child
            ASSERT_TRUE(t4->test_lookAtEntity(sightOp, res, t8));
            // T8 can see T5, even though T4 private, since T8 is a child
            ASSERT_TRUE(t5->test_lookAtEntity(sightOp, res, t8));
            // T8 can see T1, even though T4 private, since T8 is a child
            ASSERT_TRUE(t1->test_lookAtEntity(sightOp, res, t8));

            // T6 can see all since it's an admin
            ASSERT_TRUE(t1->test_lookAtEntity(sightOp, res, t6.get()));
            ASSERT_TRUE(t2->test_lookAtEntity(sightOp, res, t6.get()));
            ASSERT_TRUE(t3->test_lookAtEntity(sightOp, res, t6.get()));
            ASSERT_TRUE(t4->test_lookAtEntity(sightOp, res, t6.get()));
            ASSERT_TRUE(t5->test_lookAtEntity(sightOp, res, t6.get()));
            ASSERT_TRUE(t7->test_lookAtEntity(sightOp, res, t6.get()));

        }

        /**
         * Handle the case where there's a physical domain and an Inventory domain.
         *
         * All entities are placed at origo
         * Hierarchy looks like this:
         * T1 has a physical domain
         * T6 has an inventory domain
         * T2, T3, T5 and T7 have visibility "protected"
         * T7 is wielded by T6 (but shouldn't be visible except to T6 since it's protected)
         * T8 is private and wielded by T6 (and shouldn't be visible since it's private)
         * T9 is wielded by T6
         *
         *              T1#
         *         T2*      T3       T6##
         *         T4       T5*   T7*  T8** T9
         */
        {
            Ref<ThingExt> t1(new ThingExt(1));
            t1->requirePropertyClassFixed<BBoxProperty>().data() = {{-128, -128, -128},
                                                                    {128,  128,  128}};
            t1->domain = std::make_unique<PhysicalDomain>(*t1);
            t1->addFlags(entity_domain);
            Ref<ThingExt> t2(new ThingExt(2));
            t2->requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>::ZERO();
            t2->requirePropertyClassFixed<BBoxProperty>().data() = bbox;
            t2->addFlags(entity_visibility_protected);
            Ref<ThingExt> t3(new ThingExt(3));
            t3->requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>::ZERO();
            t3->requirePropertyClassFixed<BBoxProperty>().data() = bbox;
            Ref<ThingExt> t4(new ThingExt(4));
            t4->requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>::ZERO();
            t4->requirePropertyClassFixed<BBoxProperty>().data() = bbox;
            Ref<ThingExt> t5(new ThingExt(5));
            t5->requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>::ZERO();
            t5->requirePropertyClassFixed<BBoxProperty>().data() = bbox;
            t5->addFlags(entity_visibility_protected);
            Ref<ThingExt> t6(new ThingExt(6));
            t6->requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>::ZERO();
            t6->requirePropertyClassFixed<BBoxProperty>().data() = bbox;
            t6->domain = std::make_unique<InventoryDomain>(*t6);
            t6->addFlags(entity_domain);
            Ref<ThingExt> t7(new ThingExt(7));
            t7->requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>::ZERO();
            t7->requirePropertyClassFixed<BBoxProperty>().data() = bbox;
            t7->addFlags(entity_visibility_protected);
            {
                auto modeDataProp = std::make_unique<ModeDataProperty>();
                modeDataProp->setPlantedData({t6->getIntId()});
                t7->setProperty(ModeDataProperty::property_name, std::move(modeDataProp));
            }
            Ref<ThingExt> t8(new ThingExt(8));
            t8->requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>::ZERO();
            t8->requirePropertyClassFixed<BBoxProperty>().data() = bbox;
            t8->addFlags(entity_visibility_private);
            {
                auto modeDataProp = std::make_unique<ModeDataProperty>();
                modeDataProp->setPlantedData({t6->getIntId()});
                t8->setProperty(ModeDataProperty::property_name, std::move(modeDataProp));
            }
            Ref<ThingExt> t9(new ThingExt(9));
            t9->requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>::ZERO();
            t9->requirePropertyClassFixed<BBoxProperty>().data() = bbox;
            {
                auto modeDataProp = std::make_unique<ModeDataProperty>();
                modeDataProp->setPlantedData({t6->getIntId()});
                t9->setProperty(ModeDataProperty::property_name, std::move(modeDataProp));
            }

            t1->addChild(*t2);
            t1->addChild(*t3);
            t1->addChild(*t6);
            t2->addChild(*t4);
            t3->addChild(*t5);
            t6->addChild(*t7);
            t6->addChild(*t8);
            t6->addChild(*t9);
            Operation sightOp;
            OpVector res;
            // Everyone can see themselves
            ASSERT_TRUE(t1->test_lookAtEntity(sightOp, res, t1.get()));
            ASSERT_TRUE(t2->test_lookAtEntity(sightOp, res, t2.get()));
            ASSERT_TRUE(t3->test_lookAtEntity(sightOp, res, t3.get()));
            ASSERT_TRUE(t4->test_lookAtEntity(sightOp, res, t4.get()));
            ASSERT_TRUE(t5->test_lookAtEntity(sightOp, res, t5.get()));

            // T4 can see T2 even though it's protected since it's a child
            ASSERT_TRUE(t2->test_lookAtEntity(sightOp, res, t4.get()));
            // T2 can see T3
            ASSERT_TRUE(t3->test_lookAtEntity(sightOp, res, t2.get()));
            // T3 can't see T2 since it's protected, and not a child
            ASSERT_FALSE(t2->test_lookAtEntity(sightOp, res, t3.get()));
            // T3 can't see T4 since T2 is protected
            ASSERT_FALSE(t4->test_lookAtEntity(sightOp, res, t3.get()));
            // T6 can see T7 since it's a child and protected
            ASSERT_TRUE(t7->test_lookAtEntity(sightOp, res, t6.get()));
            // T6 can't see T8 since it's private
            ASSERT_FALSE(t8->test_lookAtEntity(sightOp, res, t6.get()));
            // T3 can't see T7 since it's protected, even though it's wielded
            ASSERT_FALSE(t7->test_lookAtEntity(sightOp, res, t3.get()));
            // T3 can't see T8 since it's private, even though it's wielded
            ASSERT_FALSE(t8->test_lookAtEntity(sightOp, res, t3.get()));
            // T3 can see T9 since it's wielded (and public)
            ASSERT_TRUE(t9->test_lookAtEntity(sightOp, res, t3.get()));
        }
    }

    void test_visibility(Context& context)
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
            Ref<ThingExt> t1(new ThingExt(1));
            Ref<ThingExt> t2(new ThingExt(2));
            Ref<ThingExt> t3(new ThingExt(3));
            Ref<ThingExt> t4(new ThingExt(4));
            Ref<ThingExt> t5(new ThingExt(5));
            t1->addChild(*t2);
            t1->addChild(*t5);
            t2->addChild(*t3);
            Operation sightOp;
            OpVector res;
            // T1 can see itself
            ASSERT_TRUE(t1->test_lookAtEntity(sightOp, res, t1.get()));
            // T1 can see T2 since it's a child and there's no domain
            ASSERT_TRUE(t2->test_lookAtEntity(sightOp, res, t1.get()));
            // T1 can see T3 since it's a grandchild and there's no domain
            ASSERT_TRUE(t3->test_lookAtEntity(sightOp, res, t1.get()));
            // T1 can't see T4 since it's not in the same graph
            ASSERT_FALSE(t4->test_lookAtEntity(sightOp, res, t1.get()));

            // T2 can see T1 since it's a parent and there's no domain
            ASSERT_TRUE(t1->test_lookAtEntity(sightOp, res, t2.get()));
            // T3 can see T1 since it's a grand parent and there's no domain
            ASSERT_TRUE(t1->test_lookAtEntity(sightOp, res, t3.get()));
            // T5 can see T3 since they share T1 as parent/grand parent and there's no domain
            ASSERT_TRUE(t3->test_lookAtEntity(sightOp, res, t5.get()));
            // T4 can't see T1 since it's not in the same graph
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
            Ref<ThingExt> t1(new ThingExt(1));
            Ref<ThingExt> t2(new ThingExt(2));
            Ref<ThingExt> t3(new ThingExt(3));
            t1->addChild(*t2);
            t2->addChild(*t3);

            t2->domain = std::make_unique<VoidDomain>(*t2);
            t2->addFlags(entity_domain);

            Operation sightOp;
            OpVector res;
            // T1 can see itself
            ASSERT_TRUE(t1->test_lookAtEntity(sightOp, res, t1.get()));
            // T1 can see T2 since it's a child and there's no domain
            ASSERT_TRUE(t2->test_lookAtEntity(sightOp, res, t1.get()));
            // T1 can't see T3 since T2 has a Void domain
            ASSERT_FALSE(t3->test_lookAtEntity(sightOp, res, t1.get()));

            // T2 can see T1 since it's a parent and there's no domain
            ASSERT_TRUE(t1->test_lookAtEntity(sightOp, res, t2.get()));
            // T3 can't see T1 since T2 has a domain
            ASSERT_FALSE(t1->test_lookAtEntity(sightOp, res, t3.get()));

            // T2 can see itself
            ASSERT_TRUE(t2->test_lookAtEntity(sightOp, res, t2.get()));
            // T2 can see T3 even though T2 has a Void domain, since T2 is the parent
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
            Ref<ThingExt> t1(new ThingExt(1));
            //        t1->setAttrValue()
            Ref<ThingExt> t2(new ThingExt(2));
            t2->requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>::ZERO();
            t2->requirePropertyClassFixed<BBoxProperty>().data() = bbox;
            Ref<ThingExt> t3(new ThingExt(3));
            t3->requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>::ZERO();
            t3->requirePropertyClassFixed<BBoxProperty>().data() = bbox;
            Ref<ThingExt> t4(new ThingExt(4));
            t4->requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>::ZERO();
            t4->requirePropertyClassFixed<BBoxProperty>().data() = bbox;
            Ref<ThingExt> t5(new ThingExt(5));
            t5->requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>::ZERO();
            t5->requirePropertyClassFixed<BBoxProperty>().data() = bbox;
            Ref<ThingExt> t6(new ThingExt(6));
            Ref<ThingExt> t7(new ThingExt(7));
            Ref<ThingExt> t8(new ThingExt(8));
            t8->requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>::ZERO();
            t8->requirePropertyClassFixed<BBoxProperty>().data() = bbox;
            t8->removeFlags(entity_perceptive);

            t2->domain = std::make_unique<PhysicalDomain>(*t2);
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
            // T1 can see itself
            ASSERT_TRUE(t1->test_lookAtEntity(sightOp, res, t1.get()));
            // T1 can see T2 since it's a child and there's no domain
            ASSERT_TRUE(t2->test_lookAtEntity(sightOp, res, t1.get()));
            // T1 can't see T3 since T2 has a Physical domain and it doesn't allow external entities to look into it.
            ASSERT_FALSE(t3->test_lookAtEntity(sightOp, res, t1.get()));

            // T2 can see T1 since it's a parent and there's no domain
            ASSERT_TRUE(t1->test_lookAtEntity(sightOp, res, t2.get()));
            // T3 can't see T1 since T2 has a domain
            ASSERT_FALSE(t1->test_lookAtEntity(sightOp, res, t3.get()));

            // T2 can see itself
            ASSERT_TRUE(t2->test_lookAtEntity(sightOp, res, t2.get()));
            // T2 can see T3 since T2 has a Physical domain which allows it.
            ASSERT_TRUE(t3->test_lookAtEntity(sightOp, res, t2.get()));
            // T5 can see T3 since T2 has a Physical domain which allows it.
            ASSERT_TRUE(t3->test_lookAtEntity(sightOp, res, t5.get()));
            // T5 can see T4 since T2 has a Physical domain which allows it to see T3, and thus T4.
            ASSERT_TRUE(t4->test_lookAtEntity(sightOp, res, t5.get()));

            // T2 can see T6 since the parent of T6 is T5, which can be seen and has no domain.
            ASSERT_TRUE(t6->test_lookAtEntity(sightOp, res, t2.get()));
            // T3 can't see T7 since T2 has a Physical domain and T7 has an invalid pos.
            ASSERT_FALSE(t7->test_lookAtEntity(sightOp, res, t3.get()));
            // T4 can't see T5 since T4 isn't a direct child of T2
            ASSERT_FALSE(t5->test_lookAtEntity(sightOp, res, t4.get()));

            ASSERT_TRUE(verifyBroadcastContains(t1, {t1}));
            ASSERT_TRUE(verifyBroadcastContains(t2, {t2, t3, t5, t1}));
            // T7 should not be visible since it has an invalid position
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
            Ref<ThingExt> t1(new ThingExt(1));
            Ref<ThingExt> t2(new ThingExt(2));
            Ref<ThingExt> t3(new ThingExt(3));
            Ref<ThingExt> t4(new ThingExt(4));
            Ref<ThingExt> t5(new ThingExt(5));
            Ref<ThingExt> t6(new ThingExt(6));

            t1->addChild(*t2);
            t1->addChild(*t6);
            t2->addChild(*t3);
            t2->addChild(*t5);
            t3->addChild(*t4);

            t2->domain = std::make_unique<InventoryDomain>(*t2);
            t2->addFlags(entity_domain);

            auto modeDataProp = std::make_unique<ModeDataProperty>();
            modeDataProp->setPlantedData({t2->getIntId()});
            t3->setProperty(ModeDataProperty::property_name, std::move(modeDataProp));

            Operation sightOp;
            OpVector res;
            // T1 can see itself
            ASSERT_TRUE(t1->test_lookAtEntity(sightOp, res, t1.get()));
            // T1 can see T2 since it's a child and there's no domain
            ASSERT_TRUE(t2->test_lookAtEntity(sightOp, res, t1.get()));
            // T1 can see T3 since T2 has an Inventory domain and T3 is wielded.
            ASSERT_TRUE(t3->test_lookAtEntity(sightOp, res, t1.get()));
            // T1 can see T4 since T2 has an Inventory domain and T3 is wielded, and T4 is a child.
            ASSERT_TRUE(t4->test_lookAtEntity(sightOp, res, t1.get()));
            // T1 can't see T5 since T2 has an Inventory domain and T5 isn't wielded.
            ASSERT_FALSE(t5->test_lookAtEntity(sightOp, res, t1.get()));

            // T6 can see T3 since T2 has an Inventory domain and T3 is wielded.
            ASSERT_TRUE(t3->test_lookAtEntity(sightOp, res, t6.get()));
            // T6 can see T4 since T2 has an Inventory domain and T3 is wielded, and T4 is a child.
            ASSERT_TRUE(t4->test_lookAtEntity(sightOp, res, t6.get()));
            // T6 can't see T5 since T2 has an Inventory domain and T5 isn't wielded.
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
            Ref<ThingExt> t1(new ThingExt(1));
            Ref<ThingExt> t2(new ThingExt(2));
            t2->requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>::ZERO();
            t2->requirePropertyClassFixed<BBoxProperty>().data() = bbox;
            Ref<ThingExt> t3(new ThingExt(3));
            t3->requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>::ZERO();
            t3->requirePropertyClassFixed<BBoxProperty>().data() = bbox;
            Ref<ThingExt> t4(new ThingExt(4));
            Ref<ThingExt> t5(new ThingExt(5));
            t5->requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>::ZERO();
            t5->requirePropertyClassFixed<BBoxProperty>().data() = bbox;
            Ref<ThingExt> t6(new ThingExt(6));

            t2->domain = std::make_unique<PhysicalDomain>(*t2);
            t2->addFlags(entity_domain);

            t3->domain = std::make_unique<InventoryDomain>(*t3);
            t3->addFlags(entity_domain);

            t1->addChild(*t2);
            t2->addChild(*t3);
            t2->addChild(*t5);
            t3->addChild(*t4);
            t3->addChild(*t6);

            auto modeDataProp = std::make_unique<ModeDataProperty>();
            modeDataProp->setPlantedData({t3->getIntId()});
            t4->setProperty(ModeDataProperty::property_name, std::move(modeDataProp));

            Operation sightOp;
            OpVector res;
            // T1 can see itself
            ASSERT_TRUE(t1->test_lookAtEntity(sightOp, res, t1.get()));
            // T1 can see T2 since it's a child and there's no domain
            ASSERT_TRUE(t2->test_lookAtEntity(sightOp, res, t1.get()));
            // T1 can't see T3 since T2 has an Physical domain and T1 is a parent.
            ASSERT_FALSE(t3->test_lookAtEntity(sightOp, res, t1.get()));
            // T1 can't see T4 since T2 has an Physical domain and T1 is a parent.
            ASSERT_FALSE(t4->test_lookAtEntity(sightOp, res, t1.get()));

            // T5 can see T3 since T2 has an Physical domain .
            ASSERT_TRUE(t3->test_lookAtEntity(sightOp, res, t5.get()));
            // T5 can see T4 since T2 has an Physical domain, T3 has an Inventory Domain and is close, and T4 is wielded.
            ASSERT_TRUE(t4->test_lookAtEntity(sightOp, res, t5.get()));
            // T5 can't see T6 since T2 has an Physical domain, T3 has an Inventory Domain and is close, and T6 isn't wielded.
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
            Ref<ThingExt> t1(new ThingExt(1));
            t1->requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>::ZERO();
            t1->requirePropertyClassFixed<BBoxProperty>().data() = bbox;
            Ref<ThingExt> t2(new ThingExt(2));
            t2->requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>::ZERO();
            t2->requirePropertyClassFixed<BBoxProperty>().data() = bbox;
            Ref<ThingExt> creator(new ThingExt(RouterId("creator", 10)));
            creator->requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>::ZERO();
            creator->requirePropertyClassFixed<BBoxProperty>().data() = bbox;
            creator->addFlags(entity_admin);

            Ref<ThingExt> t3(new ThingExt(3));
            t3->requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>::ZERO();
            t3->requirePropertyClassFixed<BBoxProperty>().data() = bbox;
            Ref<ThingExt> t4(new ThingExt(4));
            Ref<ThingExt> t5(new ThingExt(5));

            t1->domain = std::make_unique<PhysicalDomain>(*t1);
            t1->addFlags(entity_domain);

            t2->domain = std::make_unique<InventoryDomain>(*t2);
            t2->addFlags(entity_domain);

            t1->addChild(*t2);
            t1->addChild(*creator);
            t1->addChild(*t3);
            t2->addChild(*t4);
            t2->addChild(*t5);

            auto modeDataProp = std::make_unique<ModeDataProperty>();
            modeDataProp->setPlantedData({t2->getIntId()});
            t4->setProperty(ModeDataProperty::property_name, std::move(modeDataProp));

            Operation sightOp;
            OpVector res;
            // T3 can see t4 since it's wielded
            ASSERT_TRUE(t4->test_lookAtEntity(sightOp, res, t3.get()));
            //"creator" can see t4 since it's wielded
            ASSERT_TRUE(t4->test_lookAtEntity(sightOp, res, creator.get()));
            // T3 can't see t5 since it's not wielded
            ASSERT_FALSE(t5->test_lookAtEntity(sightOp, res, t3.get()));
            //"creator" can see t5 since it's a "creator"
            ASSERT_TRUE(t5->test_lookAtEntity(sightOp, res, creator.get()));

            ASSERT_TRUE(verifyBroadcastContains(t4, {t1, t2, t3, t4, creator}));
            ASSERT_TRUE(verifyBroadcastContains(t5, {t2, t5}));  // broadcasts won't be sent to creator, since they will be stopped at T2
        }
    }

    void test_reachability(Context& context)
    {
        WFMath::AxisBox<3> bbox(WFMath::Point<3>(-10, -10, -10), WFMath::Point<3>(10, 10, 10));

        auto createReachPropFn = [](double reach) {
            auto reachProp = std::make_unique<Property<double>>();
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
            Ref<ThingExt> t1(new ThingExt(1));
            Ref<ThingExt> t2(new ThingExt(2));
            Ref<ThingExt> t3(new ThingExt(3));
            Ref<ThingExt> t4(new ThingExt(4));
            Ref<ThingExt> t5(new ThingExt(5));
            t1->addChild(*t2);
            t1->addChild(*t5);
            t2->addChild(*t3);
            // T1 can reach itself
            ASSERT_TRUE(t1->canReach({t1, {}}));
            // T1 can reach T2 since it's a child and there's no domain
            ASSERT_TRUE(t1->canReach({t2, {}}));
            // T1 can reach T3 since it's a grandchild and there's no domain
            ASSERT_TRUE(t1->canReach({t3, {}}));
            // T1 can't reach T4 since it's not in the same graph
            ASSERT_FALSE(t1->canReach({t4, {}}));

            // T2 can reach T1 since it's a parent and there's no domain
            ASSERT_TRUE(t2->canReach({t1, {}}));
            // T3 can reach T1 since it's a grand parent and there's no domain
            ASSERT_TRUE(t3->canReach({t1, {}}));
            // T5 can reach T3 since they share T1 as parent/grand parent and there's no domain
            ASSERT_TRUE(t5->canReach({t3, {}}));
            // T4 can't reach T1 since it's not in the same graph
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
            Ref<ThingExt> t1(new ThingExt(1));
            Ref<ThingExt> t2(new ThingExt(2));
            Ref<ThingExt> t3(new ThingExt(3));
            t1->addChild(*t2);
            t2->addChild(*t3);

            t2->domain = std::make_unique<VoidDomain>(*t2);
            t2->addFlags(entity_domain);

            // T1 can reach itself
            ASSERT_TRUE(t1->canReach({t1, {}}));
            // T1 can reach T2 since it's a child and there's no domain
            ASSERT_TRUE(t1->canReach({t2, {}}));
            // T1 can't reach T3 since T2 has a Void domain
            ASSERT_FALSE(t1->canReach({t3, {}}));

            // T2 can reach T1 since it's a parent and there's no domain
            ASSERT_TRUE(t2->canReach({t1, {}}));
            // T3 can't reach T1 since T2 has a domain
            ASSERT_FALSE(t3->canReach({t1, {}}));

            // T2 can reach itself
            ASSERT_TRUE(t2->canReach({t2, {}}));
            // T2 can reach T3 even though T2 has a Void domain, since T2 is the parent
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
            Ref<ThingExt> t1(new ThingExt(1));
            Ref<ThingExt> t2(new ThingExt(2));
            t2->requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>::ZERO();
            t2->requirePropertyClassFixed<BBoxProperty>().data() = bbox;
            Ref<ThingExt> t3(new ThingExt(3));
            t3->requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>::ZERO();
            t3->requirePropertyClassFixed<BBoxProperty>().data() = bbox;
            Ref<ThingExt> t4(new ThingExt(4));
            t4->requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>::ZERO();
            t4->requirePropertyClassFixed<BBoxProperty>().data() = bbox;
            Ref<ThingExt> t5(new ThingExt(5));
            t5->requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>::ZERO();
            t5->requirePropertyClassFixed<BBoxProperty>().data() = bbox;
            t5->setProperty("reach", createReachPropFn(10));
            Ref<ThingExt> t6(new ThingExt(6));
            Ref<ThingExt> t7(new ThingExt(7));
            Ref<ThingExt> t8(new ThingExt(8));
            t8->requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>::ZERO();
            t8->requirePropertyClassFixed<BBoxProperty>().data() = bbox;
            t8->removeFlags(entity_perceptive);

            t2->domain = std::make_unique<PhysicalDomain>(*t2);
            t2->addFlags(entity_domain);

            t1->addChild(*t2);
            t2->addChild(*t3);
            t2->addChild(*t5);
            t2->addChild(*t7);
            t2->addChild(*t8);
            t5->addChild(*t6);
            t3->addChild(*t4);

            // T1 can reach itself
            ASSERT_TRUE(t1->canReach({t1, {}}));
            // T1 can reach T2 since it's a child and there's no domain
            ASSERT_TRUE(t1->canReach({t2, {}}));
            // T1 can't reach T3 since T2 has a Physical domain and it doesn't allow external entities to reach into it.
            ASSERT_FALSE(t1->canReach({t3, {}}));

            // T2 can reach T1 since it's a parent and there's no domain
            ASSERT_TRUE(t2->canReach({t1, {}}));
            // T3 can't reach T1 since T2 has a domain
            ASSERT_FALSE(t3->canReach({t1, {}}));

            // T2 can reach itself
            ASSERT_TRUE(t2->canReach({t2, {}}));
            // T2 can reach T3 since T2 has a Physical domain which allows it.
            ASSERT_TRUE(t2->canReach({t3, {}}));
            // T5 can reach T3 since T2 has a Physical domain which allows it.
            ASSERT_TRUE(t5->canReach({t3, {}}));
            // T5 can reach T4 since T2 has a Physical domain which allows it to reach T3, and thus T4.
            ASSERT_TRUE(t5->canReach({t4, {}}));

            // T2 can reach T6 since the parent of T6 is T5, which can be reached and has no domain.
            ASSERT_TRUE(t2->canReach({t6, {}}));
            // T3 can't reach T7 since T2 has a Physical domain and T7 has an invalid pos.
            ASSERT_FALSE(t3->canReach({t7, {}}));
            // T4 can't reach T5 since T4 isn't a direct child of T2
            ASSERT_FALSE(t4->canReach({t5, {}}));
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
            Ref<ThingExt> t1(new ThingExt(1));
            Ref<ThingExt> t2(new ThingExt(2));
            Ref<ThingExt> t3(new ThingExt(3));
            Ref<ThingExt> t4(new ThingExt(4));
            Ref<ThingExt> t5(new ThingExt(5));
            Ref<ThingExt> t6(new ThingExt(6));

            t1->addChild(*t2);
            t1->addChild(*t6);
            t2->addChild(*t3);
            t2->addChild(*t5);
            t3->addChild(*t4);

            t2->domain = std::make_unique<InventoryDomain>(*t2);
            t2->addFlags(entity_domain);
            auto entityProp = new EntityProperty();
            entityProp->data() = WeakEntityRef(t3);
            t2->setProperty("right_hand_wield", std::unique_ptr<PropertyBase>(entityProp));

            // T1 can reach itself
            ASSERT_TRUE(t1->canReach({t1, {}}));
            // T1 can reach T2 since it's a child and there's no domain
            ASSERT_TRUE(t1->canReach({t2, {}}));
            // T1 can't reach T3 since T2 has an Inventory domain, even though T3 is wielded.
            ASSERT_FALSE(t1->canReach({t3, {}}));
            // T1 can't reach T4 since T2 has an Inventory domain, even though T3 is wielded, and T4 is a child.
            ASSERT_FALSE(t1->canReach({t4, {}}));
            // T1 can't reach T5 since T2 has an Inventory domain.
            ASSERT_FALSE(t1->canReach({t5, {}}));

            // T6 can't reach T3 since T2 has an Inventory domain, even though T3 is wielded.
            ASSERT_FALSE(t6->canReach({t3, {}}));
            // T6 can't reach T4 since T2 has an Inventory domain, even though T3 is wielded, and T4 is a child.
            ASSERT_FALSE(t6->canReach({t4, {}}));
            // T6 can't reach T5 since T2 has an Inventory domain.
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
            Ref<ThingExt> t1(new ThingExt(1));
            Ref<ThingExt> t2(new ThingExt(2));
            t2->requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>::ZERO();
            t2->requirePropertyClassFixed<BBoxProperty>().data() = bbox;
            Ref<ThingExt> t3(new ThingExt(3));
            t3->requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>::ZERO();
            t3->requirePropertyClassFixed<BBoxProperty>().data() = bbox;
            Ref<ThingExt> t4(new ThingExt(4));
            Ref<ThingExt> t5(new ThingExt(5));
            t5->requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>::ZERO();
            t5->requirePropertyClassFixed<BBoxProperty>().data() = bbox;
            t5->setProperty("reach", createReachPropFn(10));
            Ref<ThingExt> t6(new ThingExt(6));

            t2->domain = std::make_unique<PhysicalDomain>(*t2);
            t2->addFlags(entity_domain);

            t3->domain = std::make_unique<InventoryDomain>(*t3);
            t3->addFlags(entity_domain);

            t1->addChild(*t2);
            t2->addChild(*t3);
            t2->addChild(*t5);
            t3->addChild(*t4);
            t3->addChild(*t6);

            auto entityProp = new EntityProperty();
            entityProp->data() = WeakEntityRef(t4);
            t3->setProperty("right_hand_wield", std::unique_ptr<PropertyBase>(entityProp));

            // T1 can reach itself
            ASSERT_TRUE(t1->canReach({t1, {}}));
            // T1 can reach T2 since it's a child and there's no domain
            ASSERT_TRUE(t1->canReach({t2, {}}));
            // T1 can't reach T3 since T2 has an Physical domain and T1 is a parent.
            ASSERT_FALSE(t1->canReach({t3, {}}));
            // T1 can't reach T4 since T2 has an Physical domain and T1 is a parent.
            ASSERT_FALSE(t1->canReach({t4, {}}));

            // T5 can reach T3 since T2 has an Physical domain .
            ASSERT_TRUE(t5->canReach({t3, {}}));
            // T5 can't reach T4 since T2 has an Physical domain, T3 has an Inventory Domain and is close, even though T4 is wielded.
            ASSERT_FALSE(t5->canReach({t4, {}}));
            // T5 can't reach T6 since T2 has an Physical domain, T3 has an Inventory Domain.
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
            Ref<ThingExt> t1(new ThingExt(1));
            t1->requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>::ZERO();
            t1->requirePropertyClassFixed<BBoxProperty>().data() = bbox;
            Ref<ThingExt> t2(new ThingExt(2));
            t2->requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>::ZERO();
            t2->requirePropertyClassFixed<BBoxProperty>().data() = bbox;
            Ref<ThingExt> creator(new ThingExt(RouterId("creator", 10)));
            creator->addFlags(entity_admin);
            creator->requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>::ZERO();
            creator->requirePropertyClassFixed<BBoxProperty>().data() = bbox;
            Ref<ThingExt> t3(new ThingExt(3));
            t3->requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>::ZERO();
            t3->requirePropertyClassFixed<BBoxProperty>().data() = bbox;
            Ref<ThingExt> t4(new ThingExt(4));
            Ref<ThingExt> t5(new ThingExt(5));

            t1->domain = std::make_unique<PhysicalDomain>(*t1);
            t1->addFlags(entity_domain);

            t2->domain = std::make_unique<InventoryDomain>(*t2);
            t2->addFlags(entity_domain);

            t1->addChild(*t2);
            t1->addChild(*creator);
            t1->addChild(*t3);
            t2->addChild(*t4);
            t2->addChild(*t5);

            auto entityProp = new EntityProperty();
            entityProp->data() = WeakEntityRef(t4);
            t2->setProperty("right_hand_wield", std::unique_ptr<PropertyBase>(entityProp));

            // T3 can't reach t4 even though it's wielded
            ASSERT_FALSE(t3->canReach({t4, {}}));
            //"creator" can reach t4 since it's a "creator"
            ASSERT_TRUE(creator->canReach({t4, {}}));
            // T3 can't reach t5
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
            Ref<ThingExt> t1(new ThingExt(1));
            t1->requirePropertyClassFixed<PositionProperty>().data() = WFMath::Point<3>::ZERO();
            t1->requirePropertyClassFixed<BBoxProperty>().data() = {{-200, -200, -200},
                                                                    {200,  200,  200}};
            Ref<ThingExt> t2(new ThingExt(2));
            t2->requirePropertyClassFixed<PositionProperty>().data() = {5, 0, 5};
            t2->requirePropertyClassFixed<BBoxProperty>().data() = smallBbox;
            Ref<ThingExt> t3(new ThingExt(3));
            t3->requirePropertyClassFixed<PositionProperty>().data() = {10, 0, 10};
            t3->requirePropertyClassFixed<BBoxProperty>().data() = smallBbox;
            auto reachProp = new Property<double>();
            reachProp->data() = 20.f;
            t3->setProperty("reach", std::unique_ptr<PropertyBase>(reachProp));
            Ref<ThingExt> t4(new ThingExt(4));
            t4->requirePropertyClassFixed<PositionProperty>().data() = {100, 0, 100};
            t4->requirePropertyClassFixed<BBoxProperty>().data() = smallBbox;

            t1->domain = std::make_unique<PhysicalDomain>(*t1);
            t1->addFlags(entity_domain);

            t1->addChild(*t2);
            t1->addChild(*t3);
            t1->addChild(*t4);

            // T2 can't reach t3 since t2 has no reach
            ASSERT_FALSE(t2->canReach({t3, {}}));
            // T3 can reach t2 since they are close
            ASSERT_TRUE(t3->canReach({t2, {}}));
            // T3 can't reach t4 since it's far away
            ASSERT_FALSE(t3->canReach({t4, {}}));
            // T2 can't reach t4 since it's far away and t2 has no reach
            ASSERT_FALSE(t2->canReach({t4, {}}));
            // T3 can reach a close point in t1
            ASSERT_TRUE(t3->canReach({t1, {9, 0, 9}}));
            // T3 can't reach a far away point in t1
            ASSERT_FALSE(t3->canReach({t1, {90, 0, 90}}));
            // T2 can't reach a close point in t1 since it has no reach
            ASSERT_FALSE(t2->canReach({t1, {6, 0, 6}}));
        }
    }
};

int main()
{
    ThingIntegration t;

    return t.run();
}

// stubs

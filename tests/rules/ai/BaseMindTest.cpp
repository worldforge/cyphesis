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

#include "../../TestBase.h"

#include "rules/ai/BaseMind.h"
#include "../../TestPropertyManager.h"

#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/SmartPtr.h>

#include <cassert>
#include "rules/SimpleTypeStore.h"

class BaseMindtest : public Cyphesis::TestBase
{
    protected:
        Ref<BaseMind> bm;
        std::unique_ptr<TestPropertyManager> propertyManager;
        std::unique_ptr<TypeStore> typeStore;
    public:
        BaseMindtest();

        void setup();

        void teardown();

        void test_getMap();

        void test_sleep();

        void test_awake();

        void test_operation();

        void test_sightOperation();

        void test_sightCreateOperation();

        void test_sightDeleteOperation();

        void test_sightMoveOperation();

        void test_sightSetOperation();

        void test_soundOperation();

        void test_appearanceOperation();

        void test_disappearanceOperation();

        void test_unseenOperation();
};

BaseMindtest::BaseMindtest()
{
    ADD_TEST(BaseMindtest::test_getMap);
    ADD_TEST(BaseMindtest::test_sleep);
    ADD_TEST(BaseMindtest::test_awake);
    ADD_TEST(BaseMindtest::test_operation);
    ADD_TEST(BaseMindtest::test_sightOperation);
    ADD_TEST(BaseMindtest::test_sightCreateOperation);
    ADD_TEST(BaseMindtest::test_sightDeleteOperation);
    ADD_TEST(BaseMindtest::test_sightMoveOperation);
    ADD_TEST(BaseMindtest::test_sightSetOperation);
    ADD_TEST(BaseMindtest::test_soundOperation);
    ADD_TEST(BaseMindtest::test_appearanceOperation);
    ADD_TEST(BaseMindtest::test_disappearanceOperation);
    ADD_TEST(BaseMindtest::test_unseenOperation);
}

void BaseMindtest::setup()
{
    propertyManager = std::make_unique<TestPropertyManager>();
    typeStore = std::make_unique<SimpleTypeStore>(*propertyManager);
    bm = new BaseMind(1, "2", *typeStore);
}

void BaseMindtest::teardown()
{
    propertyManager.reset();
    typeStore.reset();
    bm = nullptr;
}

void BaseMindtest::test_getMap()
{
    (void) bm->getMap();
}

void BaseMindtest::test_sleep()
{
    bm->sleep();

    ASSERT_TRUE(!bm->isAwake());
}

void BaseMindtest::test_awake()
{
    bm->awake();

    ASSERT_TRUE(bm->isAwake());
}

void BaseMindtest::test_operation()
{
    OpVector res;
    Atlas::Objects::Operation::Get g;
    bm->operation(g, res);
}

void BaseMindtest::test_sightOperation()
{
    OpVector res;
    Atlas::Objects::Operation::Sight op;
    bm->operation(op, res);

    op->setArgs1(Atlas::Objects::Entity::Anonymous());
    bm->operation(op, res);

    op->setArgs1(Atlas::Objects::Entity::RootEntity(0));
    bm->operation(op, res);

    op->setArgs1(Atlas::Objects::Operation::Get());
    bm->operation(op, res);
}

void BaseMindtest::test_sightCreateOperation()
{
    Atlas::Objects::Operation::Create sub_op;
    Atlas::Objects::Operation::Sight op;
    op->setArgs1(sub_op);
    OpVector res;
    bm->operation(op, res);

    sub_op->setArgs1(Atlas::Objects::Entity::Anonymous());
    bm->operation(op, res);

    sub_op->setArgs1(Atlas::Objects::Entity::Anonymous(0));
    bm->operation(op, res);
}

void BaseMindtest::test_sightDeleteOperation()
{
    Atlas::Objects::Operation::Delete sub_op;
    Atlas::Objects::Operation::Sight op;
    op->setArgs1(sub_op);
    OpVector res;
    bm->operation(op, res);

    sub_op->setArgs1(Atlas::Objects::Entity::Anonymous());
    bm->operation(op, res);

    sub_op->setArgs1(Atlas::Objects::Entity::Anonymous(0));
    bm->operation(op, res);

    Atlas::Objects::Entity::Anonymous arg;
    arg->setId("2");
    sub_op->setArgs1(arg);
    bm->operation(op, res);
}

void BaseMindtest::test_sightMoveOperation()
{
    Atlas::Objects::Operation::Move sub_op;
    Atlas::Objects::Operation::Sight op;
    op->setArgs1(sub_op);
    OpVector res;
    bm->operation(op, res);

    sub_op->setArgs1(Atlas::Objects::Entity::Anonymous());
    bm->operation(op, res);

    sub_op->setArgs1(Atlas::Objects::Entity::Anonymous(0));
    bm->operation(op, res);

    Atlas::Objects::Entity::Anonymous arg;
    arg->setId("2");
    sub_op->setArgs1(arg);
    bm->operation(op, res);
}

void BaseMindtest::test_sightSetOperation()
{
    Atlas::Objects::Operation::Set sub_op;
    Atlas::Objects::Operation::Sight op;
    op->setArgs1(sub_op);
    OpVector res;
    bm->operation(op, res);

    sub_op->setArgs1(Atlas::Objects::Entity::Anonymous());
    bm->operation(op, res);

    sub_op->setArgs1(Atlas::Objects::Entity::Anonymous(0));
    bm->operation(op, res);

    Atlas::Objects::Entity::Anonymous arg;
    arg->setId("2");
    sub_op->setArgs1(arg);
    bm->operation(op, res);
}

void BaseMindtest::test_soundOperation()
{
    OpVector res;
    Atlas::Objects::Operation::Sound op;
    bm->operation(op, res);

    op->setArgs1(Atlas::Objects::Operation::Get());
    bm->operation(op, res);
}

void BaseMindtest::test_appearanceOperation()
{
    OpVector res;
    Atlas::Objects::Operation::Appearance op;
    bm->operation(op, res);

    Atlas::Objects::Entity::Anonymous arg;
    op->setArgs1(arg);
    bm->operation(op, res);

    arg->setId("2");
    bm->operation(op, res);

    arg->setStamp(0);
    bm->operation(op, res);

    arg->setStamp(23);
    bm->operation(op, res);
}

void BaseMindtest::test_disappearanceOperation()
{
    OpVector res;
    Atlas::Objects::Operation::Disappearance op;
    bm->operation(op, res);

    Atlas::Objects::Entity::Anonymous arg;
    op->setArgs1(arg);
    bm->operation(op, res);

    arg->setId("2");
    bm->operation(op, res);
}

void BaseMindtest::test_unseenOperation()
{
    OpVector res;
    Atlas::Objects::Operation::Unseen op;
    bm->operation(op, res);

    Atlas::Objects::Entity::Anonymous arg;
    op->setArgs1(arg);
    bm->operation(op, res);

    arg->setId("2");
    bm->operation(op, res);
}

int main()
{
    BaseMindtest t;

    return t.run();
}

// stubs

#include "rules/Script.h"

#include "common/Inheritance.h"
#include "common/log.h"
#include "common/TypeNode.h"

#include "../../stubs/common/stubcustom.h"
#include "../../stubs/common/stubRouter.h"
#include "../../stubs/common/stubInheritance.h"
#include "../../stubs/rules/ai/stubMemEntity.h"
#include "../../stubs/rules/stubLocatedEntity.h"
#include "../../stubs/common/stubProperty.h"
#include "../../stubs/common/stubPropertyManager.h"


#include "../../stubs/rules/stubScript.h"
#include "../../stubs/rules/stubLocation.h"
#include "../../stubs/common/stubTypeNode.h"
#include "../../stubs/rules/ai/stubTypeResolver.h"
#include "../../stubs/rules/stubSimpleTypeStore.h"
#include "../../stubs/common/stublog.h"
#include "../../stubs/common/stubid.h"
#include "../../stubs/rules/stubPhysicalProperties.h"


static inline WFMath::CoordType sqr(WFMath::CoordType x)
{
    return x * x;
}

WFMath::CoordType squareDistance(const Point3D& u, const Point3D& v)
{
    return (sqr(u.x() - v.x()) + sqr(u.y() - v.y()) + sqr(u.z() - v.z()));
}

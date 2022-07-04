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

#include "common/Router.h"

#include "common/log.h"

#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/SmartPtr.h>

#include <cassert>

class TestRouter : public Router {
  public:
    TestRouter(RouterId id) : Router(id) { }

    virtual void externalOperation(const Operation &, Link &) { }
    virtual void operation(const Operation &, OpVector &) { }
};

class Routertest : public Cyphesis::TestBase
{
  protected:
    Router * m_router;
  public:
    Routertest();

    void setup();
    void teardown();

    void test_error();
    void test_error_to();
    void test_error_serialno();
    void test_error_serialno_to();
    void test_clientError();
    void test_addToMessage();
    void test_addToEntity();
};

Routertest::Routertest() : m_router(0)
{
    ADD_TEST(Routertest::test_error);
    ADD_TEST(Routertest::test_error_to);
    ADD_TEST(Routertest::test_error_serialno);
    ADD_TEST(Routertest::test_error_serialno_to);
    ADD_TEST(Routertest::test_clientError);
    ADD_TEST(Routertest::test_addToMessage);
    ADD_TEST(Routertest::test_addToEntity);
}

void Routertest::setup()
{
    m_router = new TestRouter(1);
}

void Routertest::teardown()
{
    delete m_router;
}

void Routertest::test_error()
{
    OpVector res;
    Atlas::Objects::Operation::Get op;

    m_router->error(op, "test failure", res);

    ASSERT_EQUAL(res.size(), 1u);

    const Atlas::Objects::Operation::RootOperation & reply = res.front();

    ASSERT_EQUAL(reply->getClassNo(),
                 Atlas::Objects::Operation::ERROR_NO);
    ASSERT_TRUE(reply->isDefaultTo());
    ASSERT_TRUE(reply->isDefaultSerialno());
    ASSERT_EQUAL(reply->getArgs().size(), 2u);
    ASSERT_EQUAL(reply->getArgs()[1]->getClassNo(),
                 op->getClassNo());
    ASSERT_TRUE(reply->getArgs().front()->getAttr("message").isString());
    ASSERT_EQUAL(reply->getArgs().front()->getAttr("message").String(),
                 "test failure");
}

void Routertest::test_error_to()
{
    OpVector res;
    Atlas::Objects::Operation::Get op;

    m_router->error(op, "test failure", res, "2");

    ASSERT_EQUAL(res.size(), 1u);

    const Atlas::Objects::Operation::RootOperation & reply = res.front();

    ASSERT_EQUAL(reply->getClassNo(),
                 Atlas::Objects::Operation::ERROR_NO);
    ASSERT_TRUE(!reply->isDefaultTo());
    ASSERT_EQUAL(reply->getTo(), "2");
    ASSERT_TRUE(reply->isDefaultSerialno());
    ASSERT_EQUAL(reply->getArgs().size(), 2u);
    ASSERT_EQUAL(reply->getArgs()[1]->getClassNo(),
                 op->getClassNo());
    ASSERT_TRUE(reply->getArgs().front()->getAttr("message").isString());
    ASSERT_EQUAL(reply->getArgs().front()->getAttr("message").String(),
                 "test failure");
}

void Routertest::test_error_serialno()
{
    OpVector res;
    Atlas::Objects::Operation::Get op;
    op->setSerialno(23);

    m_router->error(op, "test failure", res);

    ASSERT_EQUAL(res.size(), 1u);

    const Atlas::Objects::Operation::RootOperation & reply = res.front();

    ASSERT_EQUAL(reply->getClassNo(),
                 Atlas::Objects::Operation::ERROR_NO);
    ASSERT_TRUE(reply->isDefaultTo());
    // FIXME We probably should set this, regardless of whether TO
    // is set
    ASSERT_TRUE(reply->isDefaultRefno());
    ASSERT_EQUAL(reply->getArgs().size(), 2u);
    ASSERT_EQUAL(reply->getArgs()[1]->getClassNo(),
                 op->getClassNo());
    ASSERT_TRUE(reply->getArgs().front()->getAttr("message").isString());
    ASSERT_EQUAL(reply->getArgs().front()->getAttr("message").String(),
                 "test failure");
}

void Routertest::test_error_serialno_to()
{
    OpVector res;
    Atlas::Objects::Operation::Get op;
    op->setSerialno(23);

    m_router->error(op, "test failure", res, "2");

    ASSERT_EQUAL(res.size(), 1u);

    const Atlas::Objects::Operation::RootOperation & reply = res.front();

    ASSERT_EQUAL(reply->getClassNo(),
                 Atlas::Objects::Operation::ERROR_NO);
    ASSERT_TRUE(!reply->isDefaultTo());
    ASSERT_EQUAL(reply->getTo(), "2");
    ASSERT_TRUE(!reply->isDefaultRefno());
    ASSERT_EQUAL(reply->getRefno(), op->getSerialno());
    ASSERT_EQUAL(reply->getArgs().size(), 2u);
    ASSERT_EQUAL(reply->getArgs()[1]->getClassNo(),
                 op->getClassNo());
    ASSERT_TRUE(reply->getArgs().front()->getAttr("message").isString());
    ASSERT_EQUAL(reply->getArgs().front()->getAttr("message").String(),
                 "test failure");
}

void Routertest::test_clientError()
{
    OpVector res;
    Atlas::Objects::Operation::Get op;

    m_router->clientError(op, "test failure", res);

    ASSERT_EQUAL(res.size(), 1u);

    const Atlas::Objects::Operation::RootOperation & reply = res.front();

    ASSERT_EQUAL(reply->getClassNo(),
                 Atlas::Objects::Operation::ERROR_NO);
    ASSERT_TRUE(reply->isDefaultTo());
    ASSERT_TRUE(reply->isDefaultSerialno());
    ASSERT_EQUAL(reply->getArgs().size(), 2u);
    ASSERT_EQUAL(reply->getArgs()[1]->getClassNo(),
                 op->getClassNo());
    ASSERT_TRUE(reply->getArgs().front()->getAttr("message").isString());
    ASSERT_EQUAL(reply->getArgs().front()->getAttr("message").String(),
                 "test failure");
}

void Routertest::test_addToMessage()
{
    Atlas::Message::MapType msg;

    m_router->addToMessage(msg);

    ASSERT_TRUE(msg.find("objtype") != msg.end());
    ASSERT_TRUE(msg["objtype"].isString());
    ASSERT_EQUAL(msg["objtype"].String(), "obj");
    ASSERT_TRUE(msg.find("id") != msg.end());
    ASSERT_TRUE(msg["id"].isString());
    ASSERT_EQUAL(msg["id"].String(), "1");
}

void Routertest::test_addToEntity()
{
    Atlas::Objects::Entity::Anonymous ent;

    m_router->addToEntity(ent);

    ASSERT_TRUE(!ent->isDefaultObjtype());
    ASSERT_EQUAL(ent->getObjtype(), "obj");
    ASSERT_TRUE(!ent->isDefaultId());
    ASSERT_EQUAL(ent->getId(), "1");
}

int main()
{
    Routertest t;

    return t.run();
}

// stubs
#include "../stubs/common/stublog.h"

// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2003 Alistair Riddoch
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

#include "common/Inheritance.h"
#include "common/Property.h"
#include "common/log.h"
#include "common/OperationRouter.h"
#include "common/TypeNode.h"

#include "common/operations/Setup.h"

#include <cassert>

using Atlas::Message::Element;
using Atlas::Message::ListType;
using Atlas::Objects::Factories;
using Atlas::Objects::generic_factory;
using Atlas::Objects::Root;
using Atlas::Objects::smart_dynamic_cast;
using Atlas::Objects::Operation::Login;
using Atlas::Objects::Operation::RootOperation;

Atlas::Objects::Factories factories;

static void descendTree(const Root & type, Inheritance & i, int & count)
{
    assert(type.isValid());
    ++count;
    if (!type->hasAttr("children")) {
        return;
    }

    Element childrenobj = type->getAttr("children");
    assert(childrenobj.isList());
    ListType & children = childrenobj.asList();

    
    ListType::const_iterator Iend = children.end();
    for (ListType::const_iterator I = children.begin(); I != Iend; ++I) {
        const Element & e = *I;
        assert(e.isString());
        const std::string & es = e.asString();
        assert(!es.empty());
        const Root & child = i.getClass(es, Visibility::PRIVATE);
        descendTree(child, i, count);
    }

}

class Inheritancetest : public Cyphesis::TestBase
{
  public:
    static int SQUIGGLYMUFF_NO;
    
    Inheritancetest();

    virtual void setup();
    virtual void teardown();

    void test_builtins();
    void test_hasClass();
    void test_getType();
    void test_getClass();
    void test_tree();
    void test_addChild_unknown_parent();
    void test_addChild();
    void test_addChild_duplicate();
    void test_updateClass_nonexist();
    void test_updateClass_operation();
    void test_updateClass_change_parent();
    void test_isTypeOf_string();
    void test_isTypeOf_TypeNode();
    void test_isTypeOf_TypeNode2();
    void test_flush();

        Inheritance* m_inheritance;
};

int Inheritancetest::SQUIGGLYMUFF_NO = OP_INVALID;

Inheritancetest::Inheritancetest()
{
    if (SQUIGGLYMUFF_NO  == OP_INVALID) {
        SQUIGGLYMUFF_NO = factories.addFactory("squigglymuff",
                                                            &generic_factory, &Atlas::Objects::defaultInstance<Atlas::Objects::RootData>);
    }

    ADD_TEST(Inheritancetest::test_builtins);
    ADD_TEST(Inheritancetest::test_hasClass);
    ADD_TEST(Inheritancetest::test_getType);
    ADD_TEST(Inheritancetest::test_getClass);
    ADD_TEST(Inheritancetest::test_tree);
    ADD_TEST(Inheritancetest::test_addChild_unknown_parent);
    ADD_TEST(Inheritancetest::test_addChild);
    ADD_TEST(Inheritancetest::test_addChild_duplicate);
    ADD_TEST(Inheritancetest::test_updateClass_nonexist);
    ADD_TEST(Inheritancetest::test_updateClass_operation);
    ADD_TEST(Inheritancetest::test_updateClass_change_parent);
    ADD_TEST(Inheritancetest::test_isTypeOf_string);
    ADD_TEST(Inheritancetest::test_isTypeOf_TypeNode);
    ADD_TEST(Inheritancetest::test_isTypeOf_TypeNode2);
    ADD_TEST(Inheritancetest::test_flush);
}

void Inheritancetest::setup()
{
    m_inheritance = new Inheritance(factories);
}

void Inheritancetest::teardown()
{
    delete m_inheritance;
}

void Inheritancetest::test_builtins()
{
    Inheritance & i = Inheritance::instance();

    assert(&i == &Inheritance::instance());
}

void Inheritancetest::test_hasClass()
{
    Inheritance & i = Inheritance::instance();

    ASSERT_TRUE(i.hasClass("root"));
    ASSERT_TRUE(i.hasClass("root_entity"));
    ASSERT_TRUE(i.hasClass("root_operation"));
    ASSERT_TRUE(i.hasClass("login"));
}

void Inheritancetest::test_getType()
{
    Inheritance & i = Inheritance::instance();

    const TypeNode * rt_node = i.getType("root");
    assert(rt_node != 0);

}

void Inheritancetest::test_getClass()
{
    Inheritance & i = Inheritance::instance();

    const Root & rt = i.getClass("root", Visibility::PRIVATE);

    ASSERT_EQUAL(rt->getId(), "root");
    ASSERT_TRUE(rt->getParent().empty());
}

void Inheritancetest::test_tree()
{
    Inheritance & i = Inheritance::instance();

    const Root & rt = i.getClass("root", Visibility::PRIVATE);

    // Make sure the type tree is coherent, and contains a decent
    // number of types.
    int count = 0;
    descendTree(rt, i, count);
    assert(count > 20);
}

void Inheritancetest::test_addChild_unknown_parent()
{
    Inheritance & i = Inheritance::instance();

    // Make sure inserting a type with unknown parent fails with null
    Root r;
    r->setId("squigglymuff");
    r->setParent("ludricous_test_parent");
    ASSERT_NULL(i.addChild(r));

    ASSERT_TRUE(!i.hasClass("squigglymuff"));
}

void Inheritancetest::test_addChild()
{
    Inheritance & i = Inheritance::instance();

    Root r;
    r->setId("squigglymuff");
    r->setParent("root_operation");
    ASSERT_NOT_NULL(i.addChild(r));

    ASSERT_TRUE(i.hasClass("squigglymuff"));
}

void Inheritancetest::test_addChild_duplicate()
{
    Inheritance & i = Inheritance::instance();

    {
        Root r;
        r->setId("squigglymuff");
        r->setParent("root_operation");
        ASSERT_NOT_NULL(i.addChild(r));
    }

    ASSERT_TRUE(i.hasClass("squigglymuff"));

    // Make sure adding a duplicate fails. 
    Root r;
    r->setId("squigglymuff");
    r->setParent("root_operation");
    ASSERT_NULL(i.addChild(r));

}

void Inheritancetest::test_updateClass_nonexist()
{
    Inheritance & i = Inheritance::instance();

    {
        Root r;

        r->setId("squigglymuff");
        r->setParent("ludricous_test_parent");
        
        int ret = i.updateClass("squigglymuff", r);
        ASSERT_EQUAL(ret, -1);
    }

}

void Inheritancetest::test_updateClass_operation()
{
    Inheritance & i = Inheritance::instance();
    {
        Root r;
        r->setId("squigglymuff");
        r->setParent("root_operation");
        ASSERT_NOT_NULL(i.addChild(r));

        ASSERT_TRUE(i.hasClass("squigglymuff"));
    }
    {
        Root r;

        r->setId("squigglymuff");
        r->setParent("root_operation");
        
        int ret = i.updateClass("squigglymuff", r);
        ASSERT_EQUAL(ret, 0);
    }
}

void Inheritancetest::test_updateClass_change_parent()
{
    Inheritance & i = Inheritance::instance();
    {
        Root r;
        r->setId("squigglymuff");
        r->setParent("root_operation");
        ASSERT_NOT_NULL(i.addChild(r));
    }
    {
        Root r;

        r->setId("squigglymuff");
        r->setParent("action");
        
        int ret = i.updateClass("squigglymuff", r);
        ASSERT_EQUAL(ret, -1);
    }
}

void Inheritancetest::test_isTypeOf_string()
{
    Inheritance & i = Inheritance::instance();

    assert(!i.isTypeOf("ludricous_test_parent", "root_operation"));
    assert(i.isTypeOf("disappearance", "disappearance"));
    assert(i.isTypeOf("disappearance", "root_operation"));
    assert(i.isTypeOf("root_operation", "root_operation"));
    assert(!i.isTypeOf("root_operation", "talk"));
}

void Inheritancetest::test_isTypeOf_TypeNode()
{
    Inheritance & i = Inheritance::instance();

    const TypeNode * disappearance = i.getType("disappearance");
    assert(disappearance != 0);
    
    const TypeNode * root_operation = i.getType("root_operation");
    assert(root_operation != 0);
    
    assert(!i.isTypeOf(disappearance, "ludicrous_test_parent"));
    assert(!i.isTypeOf(disappearance, "root_entity"));
    assert(i.isTypeOf(disappearance, "root_operation"));
    assert(i.isTypeOf(disappearance, "disappearance"));
    assert(i.isTypeOf(root_operation, "root_operation"));
}

void Inheritancetest::test_isTypeOf_TypeNode2()
{
    Inheritance & i = Inheritance::instance();

    const TypeNode * disappearance = i.getType("disappearance");
    assert(disappearance != 0);
    
    const TypeNode * root_operation = i.getType("root_operation");
    assert(root_operation != 0);
    
    const TypeNode * root_entity = i.getType("root_entity");
    assert(root_entity != 0);
    
    assert(!i.isTypeOf(disappearance, root_entity));
    assert(i.isTypeOf(disappearance, root_operation));
    assert(i.isTypeOf(disappearance, disappearance));
    assert(i.isTypeOf(root_operation, root_operation));
}

void Inheritancetest::test_flush()
{
    Inheritance & i = Inheritance::instance();

    i.flush();

    // Make sure the type for root can no longer be retrieved
    const TypeNode * no_root_node = i.getType("root");
    assert(no_root_node == 0);
    const Root & non_root = i.getClass("root", Visibility::PRIVATE);
    assert(i.getAllObjects().empty());
    assert(!non_root.isValid());

    // Make sure installing a child of root now fails.
    Root r;
    r->setId("squigglymuff");
    r->setParent("root");
    assert(i.addChild(r) == 0);

    assert(!i.hasClass("root"));
    assert(!i.hasClass("root_entity"));
    assert(!i.hasClass("root_operation"));
    assert(!i.hasClass("login"));
    assert(!i.hasClass("squigglymuff"));
}

int main()
{
    Inheritancetest t;

    return t.run();
}

// stubs
#include "../stubs/common/stubcustom.h"

#define STUB_TypeNode_TypeNode
TypeNode::TypeNode(std::string name,
                   const Atlas::Objects::Root & d) : m_name(name),
                                                     m_privateDescription(d),
                                                     m_protectedDescription(d),
                                                     m_publicDescription(d),
                                                     m_parent(0)
{
}

#define STUB_TypeNode_isTypeOf
bool TypeNode::isTypeOf(const std::string & base_type) const
{
    const TypeNode * node = this;
    do {
        if (node->name() == base_type) {
            return true;
        }
        node = node->parent();
    } while (node != 0);
    return false;
}

bool TypeNode::isTypeOf(const TypeNode * base_type) const
{
    const TypeNode * node = this;
    do {
        if (node == base_type) {
            return true;
        }
        node = node->parent();
    } while (node != 0);
    return false;
}
#include "../stubs/common/stubTypeNode.h"
#include "../stubs/common/stublog.h"

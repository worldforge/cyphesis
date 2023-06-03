// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2005 Alistair Riddoch
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
#include "../TestWorld.h"

#include "rules/simulation/CorePropertyManager.h"
#include "server/EntityBuilder.h"
#include "server/EntityFactory.h"
#include "server/ArchetypeFactory.h"

#include "rules/simulation/Thing.h"
#include "rules/simulation/Entity.h"
#include "rules/Script.h"
#include "rules/simulation/Task.h"

#include "common/compose.hpp"
#include "common/id.h"
#include "common/Inheritance.h"
#include "common/log.h"
#include "common/Monitors.h"
#include "common/Property_impl.h"
#include "common/TypeNode.h"
#include "common/Variable.h"
#include "common/ScriptKit.h"

#include "../stubs/rules/simulation/stubEntity.h"
#include "../stubs/rules/stubLocation.h"
#include "../stubs/common/stubVariable.h"
#include "../stubs/common/stubMonitors.h"

#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/RootOperation.h>

#include <cstdlib>

#include <cassert>
#include "rules/AtlasProperties.h"
#include "rules/PhysicalProperties.h"

using Atlas::Message::Element;
using Atlas::Message::ListType;
using Atlas::Message::MapType;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Root;
namespace {
    Atlas::Objects::Factories factories;
}
class EntityBuildertest : public Cyphesis::TestBase
{
    private:
        Ref<Entity> e;
        BaseWorld* test_world;
        EntityBuilder* eb;
    public:
        EntityBuildertest();

        void setup();

        void teardown();

        void test_sequence1();

        void test_sequence2();

        void test_sequence3();

        void test_sequence4();

        void test_sequence5();

        void test_installFactory_duplicate();

        Inheritance* inheritance;
};

EntityBuildertest::EntityBuildertest()
{
    ADD_TEST(EntityBuildertest::test_sequence1);
    ADD_TEST(EntityBuildertest::test_sequence2);
    ADD_TEST(EntityBuildertest::test_sequence3);
    ADD_TEST(EntityBuildertest::test_sequence4);
    ADD_TEST(EntityBuildertest::test_sequence5);
    ADD_TEST(EntityBuildertest::test_installFactory_duplicate);
}

void EntityBuildertest::setup()
{
    inheritance = new Inheritance(factories);
    e = new Entity(1);
    test_world = new TestWorld(e);
    eb = new EntityBuilder();

    eb->installBaseFactory("thing", "game_entity", std::make_unique<EntityFactory<Thing>>());
}

void EntityBuildertest::teardown()
{
    delete test_world;
    e = nullptr;
    delete eb;
    delete inheritance;
}

enum action
{
    DO_NOTHING,
    SET_POS,
    SET_VELOCITY
} LocatedEntity_merge_action = DO_NOTHING;

void EntityBuildertest::test_sequence1()
{
    Anonymous attributes;

    assert(eb->newEntity(1, "world", attributes) == nullptr);
    assert(eb->newEntity(1, "nonexistant", attributes) == nullptr);
    assert(eb->newEntity(1, "thing", attributes) != nullptr);
}

void EntityBuildertest::test_sequence2()
{
    Anonymous attributes;

    // Create a normal Entity
    auto test_ent = eb->newEntity(1, "thing", attributes);
    assert(test_ent);

    // Create an entity specifying an attrbute
    attributes->setAttr("funky", "true");

    test_ent = eb->newEntity(1, "thing", attributes);
    assert(test_ent);

    // Create an entity causing VELOCITY to be set
    attributes = Anonymous();

    attributes->setVelocity(std::vector<double>(3, 1.5));

    LocatedEntity_merge_action = SET_VELOCITY;

    test_ent = eb->newEntity(1, "thing", attributes);
    assert(test_ent);

    LocatedEntity_merge_action = DO_NOTHING;

    // Create an entity causing VELOCITY to be set for no obvious reason
    attributes = Anonymous();

    LocatedEntity_merge_action = SET_VELOCITY;

    test_ent = eb->newEntity(1, "thing", attributes);
    assert(test_ent);

    LocatedEntity_merge_action = DO_NOTHING;

    // Create an entity specifying a LOC
    attributes = Anonymous();

    attributes->setLoc("1");

    test_ent = eb->newEntity(1, "thing", attributes);
    assert(test_ent);
}

void EntityBuildertest::test_sequence3()
{
    // Attributes for test entities being created
    Anonymous attributes;

    // Create an entity which is an instance of one of the core classes
    auto test_ent = eb->newEntity(1, "thing", attributes);
    assert(test_ent);
}

void EntityBuildertest::test_sequence4()
{
    TypeNode factoryType("custom_type");
    // Attributes for test entities being created
    Anonymous attributes;

    // Check that creating an entity of a type we know we have not yet
    // installed results in a null pointer.
    assert(eb->newEntity(1, "custom_type", attributes) == nullptr);

    // Get a reference to the internal dictionary of entity factories.
    const auto& factory_dict = eb->getFactories();

    // Make sure it has some factories in it already.
    assert(!factory_dict.empty());

    // Assert the dictionary does not contain the factory we know we have
    // have not yet installed.
    assert(factory_dict.find("custom_type") == factory_dict.end());

    // Set up a type description for a new type, and install it
    EntityFactoryBase* custom_type_factory = new EntityFactory<Entity>();
    custom_type_factory->m_attributes["test_custom_type_attr"] =
        "test_value";
    {
        int ret;
        ret = eb->installFactory("custom_type",
                                 atlasClass("custom_type", "thing"),
                                 std::unique_ptr<EntityFactoryBase>(custom_type_factory));
        custom_type_factory->m_type = &factoryType;

        ASSERT_EQUAL(ret, 0);
    }

    PropertyBase* p = new Property<std::string>;
    p->set("test_value");
    custom_type_factory->m_type->injectProperty("test_custom_type_attr", std::unique_ptr<PropertyBase>(p));

    // Check that the factory dictionary now contains a factory for
    // the custom type we just installed.
    auto I = factory_dict.find("custom_type");
    assert(I != factory_dict.end());
    assert(custom_type_factory == I->second.get());

    MapType::const_iterator J;
    // Check the factory has the attributes we described on the custom
    // type.
    J = custom_type_factory->m_attributes.find("test_custom_type_attr");
    assert(J != custom_type_factory->m_attributes.end());
    assert(J->second.isString());
    assert(J->second.String() == "test_value");

    // Create an instance of our custom type, ensuring that it works.
    auto test_ent = eb->newEntity(1, "custom_type", attributes);
    assert(test_ent);

    assert(test_ent->getType() == custom_type_factory->m_type);

    // Check that creating an entity of a type we know we have not yet
    // installed results in a null pointer.
    assert(eb->newEntity(1, "custom_inherited_type", attributes) == nullptr);

    // Assert the dictionary does not contain the factory we know we have
    // have not yet installed.
    assert(factory_dict.find("custom_inherited_type") == factory_dict.end());
}

void EntityBuildertest::test_sequence5()
{
    TypeNode factoryType("custom_scripted_type");
    Anonymous attributes;

    // Get a reference to the internal dictionary of entity factories.
    const auto& factory_dict = eb->getFactories();

    // Make sure it has some factories in it already.
    assert(!factory_dict.empty());

    // Assert the dictionary does not contain the factory we know we have
    // have not yet installed.
    assert(factory_dict.find("custom_scripted_type") == factory_dict.end());

    // Set up a type description for a new type, and install it
    EntityFactoryBase* custom_type_factory = new EntityFactory<Entity>();
    custom_type_factory->m_attributes["test_custom_type_attr"] =
        "test_value";

    {
        eb->installFactory("custom_scripted_type",
                           atlasClass("custom_scripted_type", "thing"),
                           std::unique_ptr<EntityFactoryBase>(custom_type_factory));
        custom_type_factory->m_type = &factoryType;
    }

    // Check that the factory dictionary now contains a factory for
    // the custom type we just installed.
    auto I = factory_dict.find("custom_scripted_type");
    assert(I != factory_dict.end());
    assert(custom_type_factory == I->second.get());

    // Create an instance of our custom type, ensuring that it works.
    auto test_ent = eb->newEntity(1, "custom_scripted_type", attributes);
    assert(test_ent);

    assert(test_ent->getType() == custom_type_factory->m_type);
    test_ent->destroy();
}

void EntityBuildertest::test_installFactory_duplicate()
{
    auto& factory_dict = eb->getFactories();

    EntityFactoryBase* custom_type_factory = new EntityFactory<Entity>();

    int ret = eb->installFactory("custom_type",
                                 atlasClass("custom_type", "thing"),
                                 std::unique_ptr<EntityFactoryBase>(custom_type_factory));

    ASSERT_EQUAL(ret, 0);
    ASSERT_TRUE(factory_dict.find("custom_type") != factory_dict.end());
    ASSERT_EQUAL(factory_dict.find("custom_type")->second.get(), custom_type_factory);

    EntityFactoryBase* custom_type_factory2 = new EntityFactory<Entity>();

    ret = eb->installFactory("custom_type",
                             atlasClass("custom_type", "thing"),
                             std::unique_ptr<EntityFactoryBase>(custom_type_factory2));

    ASSERT_EQUAL(ret, -1);
    ASSERT_TRUE(factory_dict.find("custom_type") != factory_dict.end());
    ASSERT_EQUAL(factory_dict.find("custom_type")->second.get(), custom_type_factory);
}

int main(int argc, char** argv)
{
    EntityBuildertest t;

    return t.run();
}

// stubs





#define STUB_EntityFactory_newEntity

template<class T>
Ref<Entity> EntityFactory<T>::newEntity(RouterId id,
                                               const Atlas::Objects::Entity::RootEntity& attributes)
{
    ++m_createdCount;
    auto* e = new Entity(id);
    e->setType(m_type);
    return e;
}

#define STUB_EntityFactory_duplicateFactory

template<typename T>
std::unique_ptr<EntityFactoryBase> EntityFactory<T>::duplicateFactory()
{
    auto f = std::unique_ptr<EntityFactory<T>>(new EntityFactory<T>(*this));
    f->m_parent = this;
    return f;
}


#define STUB_ArchetypeFactory_duplicateFactory

std::unique_ptr<ArchetypeFactory> ArchetypeFactory::duplicateFactory()
{
    auto f = std::unique_ptr<ArchetypeFactory>(new ArchetypeFactory(*this));
    f->m_parent = this;
    return f;
}




class World;

template<>
Ref<Entity> EntityFactory<World>::newEntity(RouterId id,
                                                   const Atlas::Objects::Entity::RootEntity& attributes)
{
    return 0;
}

class Thing;

class Character;

class Creator;

class Plant;

class Stackable;

template
class EntityFactory<Entity>;

template
class EntityFactory<Thing>;

template
class EntityFactory<Character>;

template
class EntityFactory<Creator>;

template
class EntityFactory<Plant>;

template
class EntityFactory<Stackable>;

template
class EntityFactory<World>;

#define STUB_LocatedEntity_makeContainer

void LocatedEntity::makeContainer()
{
    if (m_contains == 0) {
        m_contains.reset(new LocatedEntitySet);
    }
}

#define STUB_LocatedEntity_merge

void LocatedEntity::merge(const MapType& ent)
{
    switch (LocatedEntity_merge_action) {
        case SET_POS:
            this->requirePropertyClassFixed<PositionProperty>().data().setValid();
            break;
        case SET_VELOCITY:
            this->requirePropertyClassFixed<VelocityProperty>().data().setValid();
            break;
        case DO_NOTHING:
        default:
            break;
    };

}

#ifndef STUB_Inheritance_Inheritance
#define STUB_Inheritance_Inheritance

Inheritance::Inheritance(Atlas::Objects::Factories& factories) : noClass(0), m_factories(factories)
{
    Atlas::Objects::Entity::Anonymous root_desc;

    root_desc->setObjtype("meta");
    root_desc->setId("root");

    TypeNode* root = new TypeNode("root", root_desc);

    atlasObjects["root"].reset(root);
    installStandardObjects(*this);
}

#endif //STUB_Inheritance_Inheritance

#ifndef STUB_Inheritance_getType
#define STUB_Inheritance_getType

const TypeNode* Inheritance::getType(const std::string& parent) const
{
    auto I = atlasObjects.find(parent);
    if (I == atlasObjects.end()) {
        return 0;
    }
    return I->second.get();
}

#endif //STUB_Inheritance_getType

#ifndef STUB_Inheritance_addChild
#define STUB_Inheritance_addChild

TypeNode* Inheritance::addChild(const Atlas::Objects::Root& obj)
{
    const std::string& child = obj->getId();
    const std::string& parent = obj->getParent();
    if (atlasObjects.find(child) != atlasObjects.end()) {
        std::cerr << String::compose("Installing type \"%1\"(\"%2\") "
                                     "which was already installed",
                                     child, parent) << std::endl;
        return 0;
    }
    auto I = atlasObjects.find(parent);
    if (I == atlasObjects.end()) {
        std::cerr << String::compose("Installing type \"%1\" "
                                     "which has unknown parent \"%2\".",
                                     child, parent) << std::endl;
        return 0;
    }
    Element children(ListType(1, child));
    if (I->second->description(Visibility::PRIVATE)->copyAttr("children", children) == 0) {
        assert(children.isList());
        children.asList().push_back(child);
    }
    I->second->description(Visibility::PRIVATE)->setAttr("children", children);

    TypeNode* type = new TypeNode(child, obj);
    type->setParent(I->second.get());

    atlasObjects[child].reset(type);

    return type;
}

#endif //STUB_Inheritance_addChild

void installStandardObjects(TypeStore& i)
{

    i.addChild(atlasClass("root_entity", "root"));
    i.addChild(atlasClass("admin_entity", "root_entity"));
    i.addChild(atlasClass("account", "admin_entity"));
    i.addChild(atlasClass("player", "account"));
    i.addChild(atlasClass("admin", "account"));
    i.addChild(atlasClass("game", "admin_entity"));
    i.addChild(atlasClass("game_entity", "root_entity"));
}


#define STUB_TypeNode_injectProperty

TypeNode::PropertiesUpdate TypeNode::injectProperty(const std::string& name,
                                                    std::unique_ptr<PropertyBase> p)
{
    m_defaults[name] = std::move(p);
    return {};
}

#ifndef STUB_BaseWorld_getEntity
#define STUB_BaseWorld_getEntity

Ref<LocatedEntity> BaseWorld::getEntity(const std::string& id) const
{
    return getEntity(integerId(id));
}

Ref<LocatedEntity> BaseWorld::getEntity(long id) const
{
    auto I = m_eobjects.find(id);
    if (I != m_eobjects.end()) {
        assert(I->second);
        return I->second;
    } else {
        return nullptr;
    }
}

#endif //STUB_BaseWorld_getEntity

#include "../stubs/rules/simulation/stubBaseWorld.h"
#include "../stubs/rules/simulation/stubCorePropertyManager.h"
#include "../stubs/common/stubPropertyManager.h"
#include "../stubs/common/stubid.h"
#include "../stubs/common/stubRouter.h"
#include "../stubs/rules/stubScript.h"
#include "../stubs/rules/simulation/stubTask.h"
#include "../stubs/common/stubTypeNode.h"
#include "../stubs/common/stubInheritance.h"
#include "../stubs/rules/stubLocatedEntity.h"
#include "../stubs/common/stublog.h"
#include "../stubs/server/stubArchetypeFactory.h"
#include "../stubs/rules/python/stubScriptsProperty.h"
#include "../stubs/server/stubEntityFactory.h"
#include "../stubs/server/stubEntityKit.h"

Root atlasClass(const std::string& name, const std::string& parent)
{
    Atlas::Objects::Entity::Anonymous r;
    r->setParent(parent);
    r->setObjtype("class");
    r->setId(name);
    return r;
}

sigc::signal<void()> python_reload_scripts;
#include "../stubs/rules/stubPhysicalProperties.h"
#include "../stubs/common/stubProperty.h"

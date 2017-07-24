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

#include "TestBase.h"
#include "TestWorld.h"

#include "server/CorePropertyManager.h"
#include "server/EntityBuilder.h"
#include "server/EntityFactory.h"
#include "server/ArchetypeFactory.h"

#include "rulesets/Entity.h"
#include "rulesets/Script.h"
#include "rulesets/Task.h"

#include "common/compose.hpp"
#include "common/id.h"
#include "common/Inheritance.h"
#include "common/log.h"
#include "common/Monitors.h"
#include "common/Property_impl.h"
#include "common/TypeNode.h"
#include "common/Variable.h"
#include "common/ScriptKit.h"
#include "common/TaskKit.h"

#include "stubs/rulesets/stubEntity.h"
#include "stubs/modules/stubLocation.h"
#include "stubs/common/stubVariable.h"
#include "stubs/common/stubMonitors.h"

#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/RootOperation.h>

#include <cstdlib>

#include <cassert>

using Atlas::Message::Element;
using Atlas::Message::ListType;
using Atlas::Message::MapType;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Root;

class TestScriptFactory : public ScriptKit<LocatedEntity> {
  protected:
    std::string m_package;
  public:
    TestScriptFactory() { }

    const std::string & package() const { return m_package; }
    int addScript(LocatedEntity * entity) const { return 0; }
    int refreshClass() { return 0; }
};

class TestTaskFactory : public TaskKit
{
  public:
    virtual int checkTarget(LocatedEntity * target) { return 0; }
    virtual Task * newTask(LocatedEntity & chr) { return 0; }
};

class EntityBuildertest : public Cyphesis::TestBase
{
  private:
    Entity * e;
    BaseWorld * test_world;
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
    void test_installTaskFactory_duplicate();
};

EntityBuildertest::EntityBuildertest()
{
    ADD_TEST(EntityBuildertest::test_sequence1);
    ADD_TEST(EntityBuildertest::test_sequence2);
    ADD_TEST(EntityBuildertest::test_sequence3);
    ADD_TEST(EntityBuildertest::test_sequence4);
    ADD_TEST(EntityBuildertest::test_sequence5);
    ADD_TEST(EntityBuildertest::test_installFactory_duplicate);
    ADD_TEST(EntityBuildertest::test_installTaskFactory_duplicate);
}

void EntityBuildertest::setup()
{
    e = new Entity("1", 1);
    test_world = new TestWorld(*e);
    EntityBuilder::init();
}

void EntityBuildertest::teardown()
{
    delete test_world;
    delete e;
    EntityBuilder::del();
    assert(EntityBuilder::instance() == 0);
    Inheritance::clear();
}

enum action {
  DO_NOTHING,
  SET_POS,
  SET_VELOCITY } LocatedEntity_merge_action = DO_NOTHING;

void EntityBuildertest::test_sequence1()
{
    Anonymous attributes;

    assert(EntityBuilder::instance() != 0);

    assert(EntityBuilder::instance()->newEntity("1", 1, "world", attributes, BaseWorld::instance()) == 0);
    assert(EntityBuilder::instance()->newEntity("1", 1, "nonexistant", attributes, BaseWorld::instance()) == 0);
    assert(EntityBuilder::instance()->newEntity("1", 1, "thing", attributes, BaseWorld::instance()) != 0);
}

void EntityBuildertest::test_sequence2()
{
    Anonymous attributes;

    assert(EntityBuilder::instance() != 0);

    // Create a normal Entity
    LocatedEntity * test_ent = EntityBuilder::instance()->newEntity("1", 1, "thing", attributes, BaseWorld::instance());
    assert(test_ent != 0);

    // Create an entity specifying an attrbute
    attributes->setAttr("funky", "true");

    test_ent = EntityBuilder::instance()->newEntity("1", 1, "thing", attributes, BaseWorld::instance());
    assert(test_ent != 0);

    // Create an entity causing VELOCITY to be set
    attributes = Anonymous();

    attributes->setVelocity(std::vector<double>(3, 1.5));

    LocatedEntity_merge_action = SET_VELOCITY;

    test_ent = EntityBuilder::instance()->newEntity("1", 1, "thing", attributes, BaseWorld::instance());
    assert(test_ent != 0);

    LocatedEntity_merge_action = DO_NOTHING;

    // Create an entity causing VELOCITY to be set for no obvious reason
    attributes = Anonymous();

    LocatedEntity_merge_action = SET_VELOCITY;

    test_ent = EntityBuilder::instance()->newEntity("1", 1, "thing", attributes, BaseWorld::instance());
    assert(test_ent != 0);

    LocatedEntity_merge_action = DO_NOTHING;

    // Create an entity specifying a LOC
    attributes = Anonymous();

    attributes->setLoc("1");

    test_ent = EntityBuilder::instance()->newEntity("1", 1, "thing", attributes, BaseWorld::instance());
    assert(test_ent != 0);
}

void EntityBuildertest::test_sequence3()
{
    EntityBuilder & entity_factory = *EntityBuilder::instance();

    // Attributes for test entities being created
    Anonymous attributes;

    // Create an entity which is an instance of one of the core classes
    LocatedEntity * test_ent = entity_factory.newEntity("1", 1, "thing", attributes, BaseWorld::instance());
    assert(test_ent != 0);
}

void EntityBuildertest::test_sequence4()
{
    EntityBuilder & entity_factory = *EntityBuilder::instance();
    // Attributes for test entities being created
    Anonymous attributes;

    // Check that creating an entity of a type we know we have not yet
    // installed results in a null pointer.
    assert(entity_factory.newEntity("1", 1, "custom_type", attributes, BaseWorld::instance()) == 0);

    // Get a reference to the internal dictionary of entity factories.
    const FactoryDict & factory_dict = entity_factory.m_entityFactories;

    // Make sure it has some factories in it already.
    assert(!factory_dict.empty());

    // Assert the dictionary does not contain the factory we know we have
    // have not yet installed.
    assert(factory_dict.find("custom_type") == factory_dict.end());

    // Set up a type description for a new type, and install it
    EntityFactoryBase * custom_type_factory = new EntityFactory<Entity>();
    custom_type_factory->m_attributes["test_custom_type_attr"] =
          "test_value";
    {
        int ret;
        ret = entity_factory.installFactory("custom_type",
                                            atlasClass("custom_type", "thing"),
                                            custom_type_factory);
        custom_type_factory->m_type = new TypeNode("custom_type");

        ASSERT_EQUAL(ret, 0);
    }

    PropertyBase * p = new Property<std::string>; 
    custom_type_factory->m_type->addProperty("test_custom_type_attr", p);
    p->set("test_value");

    // Check that the factory dictionary now contains a factory for
    // the custom type we just installed.
    FactoryDict::const_iterator I = factory_dict.find("custom_type");
    assert(I != factory_dict.end());
    assert(custom_type_factory == I->second);

    MapType::const_iterator J;
    // Check the factory has the attributes we described on the custom
    // type.
    J = custom_type_factory->m_attributes.find("test_custom_type_attr");
    assert(J != custom_type_factory->m_attributes.end());
    assert(J->second.isString());
    assert(J->second.String() == "test_value");

    // Create an instance of our custom type, ensuring that it works.
    LocatedEntity * test_ent = entity_factory.newEntity("1", 1, "custom_type", attributes, BaseWorld::instance());
    assert(test_ent != 0);

    assert(test_ent->getType() == custom_type_factory->m_type);

    // Check that creating an entity of a type we know we have not yet
    // installed results in a null pointer.
    assert(entity_factory.newEntity("1", 1, "custom_inherited_type", attributes, BaseWorld::instance()) == 0);

    // Assert the dictionary does not contain the factory we know we have
    // have not yet installed.
    assert(factory_dict.find("custom_inherited_type") == factory_dict.end());
}

void EntityBuildertest::test_sequence5()
{
    EntityBuilder & entity_factory = *EntityBuilder::instance();
    Anonymous attributes;

    // Get a reference to the internal dictionary of entity factories.
    const FactoryDict & factory_dict = entity_factory.m_entityFactories;

    // Make sure it has some factories in it already.
    assert(!factory_dict.empty());

    // Assert the dictionary does not contain the factory we know we have
    // have not yet installed.
    assert(factory_dict.find("custom_scripted_type") == factory_dict.end());

    // Set up a type description for a new type, and install it
    EntityFactoryBase * custom_type_factory = new EntityFactory<Entity>();
    custom_type_factory->m_attributes["test_custom_type_attr"] =
          "test_value";

    custom_type_factory->m_scriptFactory = new TestScriptFactory();

    {
        entity_factory.installFactory("custom_scripted_type",
              atlasClass("custom_scripted_type", "thing"),
              custom_type_factory);
        custom_type_factory->m_type = new TypeNode("custom_scripted_type");
    }

    // Check that the factory dictionary now contains a factory for
    // the custom type we just installed.
    FactoryDict::const_iterator I = factory_dict.find("custom_scripted_type");
    assert(I != factory_dict.end());
    assert(custom_type_factory == I->second);

    // Create an instance of our custom type, ensuring that it works.
    LocatedEntity * test_ent = entity_factory.newEntity("1", 1, "custom_scripted_type", attributes, BaseWorld::instance());
    assert(test_ent != 0);

    assert(test_ent->getType() == custom_type_factory->m_type);
}

void EntityBuildertest::test_installFactory_duplicate()
{
    EntityBuilder & entity_factory = *EntityBuilder::instance();
    FactoryDict & factories = entity_factory.m_entityFactories;

    EntityFactoryBase * custom_type_factory = new EntityFactory<Entity>();

    int ret = entity_factory.installFactory("custom_type",
                                            atlasClass("custom_type", "thing"),
                                            custom_type_factory);

    ASSERT_EQUAL(ret, 0);
    ASSERT_TRUE(factories.find("custom_type") != factories.end());
    ASSERT_EQUAL(factories.find("custom_type")->second, custom_type_factory);

    EntityFactoryBase * custom_type_factory2 = new EntityFactory<Entity>();

    ret = entity_factory.installFactory("custom_type",
                                        atlasClass("custom_type", "thing"),
                                        custom_type_factory2);

    ASSERT_EQUAL(ret, -1);
    ASSERT_TRUE(factories.find("custom_type") != factories.end());
    ASSERT_EQUAL(factories.find("custom_type")->second, custom_type_factory);
}

void EntityBuildertest::test_installTaskFactory_duplicate()
{
    EntityBuilder & entity_factory = *EntityBuilder::instance();
    TaskFactoryDict & factories = entity_factory.m_taskFactories;

    std::string class_name("custom_task_type");

    TaskKit * factory = new TestTaskFactory();

    entity_factory.installTaskFactory(class_name, factory);

    ASSERT_TRUE(factories.find(class_name) != factories.end());
    ASSERT_EQUAL(factories.find(class_name)->second, factory);

    TaskKit * factory2 = new TestTaskFactory();

    entity_factory.installTaskFactory(class_name, factory2);

    // Check factory2 hasn't replaced the first one inserted
    ASSERT_TRUE(factories.find(class_name) != factories.end());
    ASSERT_EQUAL(factories.find(class_name)->second, factory);
}

int main(int argc, char ** argv)
{
    EntityBuildertest t;

    return t.run();
}

// stubs

void TestWorld::message(const Operation & op, LocatedEntity & ent)
{
}

LocatedEntity * TestWorld::addNewEntity(const std::string &,
                                 const Atlas::Objects::Entity::RootEntity &)
{
    return 0;
}

EntityKit::EntityKit() : m_createdCount(0)
{
}

EntityKit::~EntityKit()
{
}

EntityFactoryBase::EntityFactoryBase() : EntityKit::EntityKit(), m_scriptFactory(0)
{
}

EntityFactoryBase::~EntityFactoryBase()
{
    delete m_scriptFactory;
}

void EntityFactoryBase::addProperties()
{
}

void EntityFactoryBase::updateProperties()
{
}

ArchetypeFactory::ArchetypeFactory()
{
}

ArchetypeFactory::ArchetypeFactory(ArchetypeFactory& rhs)
{
}

ArchetypeFactory::~ArchetypeFactory()
{
}

void ArchetypeFactory::addProperties()
{
}

void ArchetypeFactory::updateProperties()
{
}

ArchetypeFactory * ArchetypeFactory::duplicateFactory()
{
    ArchetypeFactory * f = new ArchetypeFactory(*this);
    f->m_parent = this;
    return f;
}

LocatedEntity * ArchetypeFactory::newEntity(const std::string & id, long intId,
        const Atlas::Objects::Entity::RootEntity & attributes, LocatedEntity* location)
{
    return 0;
}


class World;

template <>
LocatedEntity * EntityFactory<World>::newEntity(const std::string & id, long intId,
        const Atlas::Objects::Entity::RootEntity & attributes, LocatedEntity* location)
{
    return 0;
}

template <class T>
EntityFactory<T>::EntityFactory(EntityFactory<T> & o)
{
}

template <class T>
EntityFactory<T>::EntityFactory()
{
}

template <class T>
EntityFactory<T>::~EntityFactory()
{
}

template <class T>
LocatedEntity * EntityFactory<T>::newEntity(const std::string & id, long intId,
        const Atlas::Objects::Entity::RootEntity & attributes, LocatedEntity* location)
{
    ++m_createdCount;
    Entity* e = new Entity(id, intId);
    e->setType(m_type);
    return e;
}

template <class T>
EntityFactoryBase * EntityFactory<T>::duplicateFactory()
{
    EntityFactoryBase * f = new EntityFactory<T>(*this);
    f->m_parent = this;
    return f;
}

class Thing;
class Character;
class Creator;
class Plant;
class Stackable;

template class EntityFactory<Entity>;
template class EntityFactory<Thing>;
template class EntityFactory<Character>;
template class EntityFactory<Creator>;
template class EntityFactory<Plant>;
template class EntityFactory<Stackable>;
template class EntityFactory<World>;

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

Domain * LocatedEntity::getMovementDomain()
{
    return 0;
}

const Domain * LocatedEntity::getMovementDomain() const
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

void LocatedEntity::merge(const MapType & ent)
{
    switch (LocatedEntity_merge_action) {
      case SET_POS:
        this->m_location.m_pos.setValid();
        break;
      case SET_VELOCITY:
        this->m_location.m_velocity.setValid();
        break;
      case DO_NOTHING:
      default:
        break;
    };

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


void log(LogLevel lvl, const std::string & msg)
{
}

Inheritance * Inheritance::m_instance = NULL;

Inheritance::Inheritance() : noClass(0)
{
    Atlas::Objects::Entity::Anonymous root_desc;

    root_desc->setObjtype("meta");
    root_desc->setId("root");

    TypeNode * root = new TypeNode("root", root_desc);

    atlasObjects["root"] = root;
}

Inheritance & Inheritance::instance()
{
    if (m_instance == NULL) {
        m_instance = new Inheritance();
        installStandardObjects();
    }
    return *m_instance;
}

void installStandardObjects()
{
    Inheritance & i = Inheritance::instance();

    i.addChild(atlasClass("root_entity", "root"));
    i.addChild(atlasClass("admin_entity", "root_entity"));
    i.addChild(atlasClass("account", "admin_entity"));
    i.addChild(atlasClass("player", "account"));
    i.addChild(atlasClass("admin", "account"));
    i.addChild(atlasClass("game", "admin_entity"));
    i.addChild(atlasClass("game_entity", "root_entity"));
}

const TypeNode * Inheritance::getType(const std::string & parent)
{
    TypeNodeDict::const_iterator I = atlasObjects.find(parent);
    if (I == atlasObjects.end()) {
        return 0;
    }
    return I->second;
}

bool Inheritance::isTypeOf(const TypeNode * instance,
                           const std::string & base_type) const
{
    return false;
}

bool Inheritance::isTypeOf(const std::string & instance,
                           const std::string & base_type) const
{
    return false;
}

TypeNode * Inheritance::addChild(const Root & obj)
{
    const std::string & child = obj->getId();
    const std::string & parent = obj->getParent();
    if (atlasObjects.find(child) != atlasObjects.end()) {
        std::cerr << String::compose("Installing type \"%1\"(\"%2\") "
                                   "which was already installed",
                                   child, parent) << std::endl;
        return 0;
    }
    TypeNodeDict::iterator I = atlasObjects.find(parent);
    if (I == atlasObjects.end()) {
        std::cerr << String::compose("Installing type \"%1\" "
                                   "which has unknown parent \"%2\".",
                                   child, parent) << std::endl;
        return 0;
    }
    Element children(ListType(1, child));
    if (I->second->description()->copyAttr("children", children) == 0) {
        assert(children.isList());
        children.asList().push_back(child);
    }
    I->second->description()->setAttr("children", children);

    TypeNode * type = new TypeNode(child, obj);
    type->setParent(I->second);

    atlasObjects[child] = type;

    return type;
}

void Inheritance::clear()
{
    if (m_instance != NULL) {
        delete m_instance;
        m_instance = NULL;
    }
}

TypeNode::TypeNode(const std::string & name) : m_name(name), m_parent(0)
{
}

TypeNode::TypeNode(const std::string & name,
                   const Atlas::Objects::Root & d) : m_name(name),
                                                     m_description(d),
                                                     m_parent(0)
{
}

TypeNode::~TypeNode()
{
}

void TypeNode::addProperty(const std::string & name,
                           PropertyBase * p)
{
    m_defaults[name] = p;
}

PropertyBase::PropertyBase(unsigned int flags) : m_flags(flags)
{
}

PropertyBase::~PropertyBase()
{
}

void PropertyBase::install(LocatedEntity *, const std::string & name)
{
}

void PropertyBase::remove(LocatedEntity *, const std::string & name)
{
}

void PropertyBase::apply(LocatedEntity *)
{
}

void PropertyBase::add(const std::string & s,
                       Atlas::Message::MapType & ent) const
{
    get(ent[s]);
}

void PropertyBase::add(const std::string & s,
                       const Atlas::Objects::Entity::RootEntity & ent) const
{
}

HandlerResult PropertyBase::operation(LocatedEntity *,
                                      const Operation &,
                                      OpVector &)
{
    return OPERATION_IGNORED;
}

template<>
void Property<int>::set(const Atlas::Message::Element & e)
{
    if (e.isInt()) {
        this->m_data = e.asInt();
    }
}

template<>
void Property<double>::set(const Atlas::Message::Element & e)
{
    if (e.isNum()) {
        this->m_data = e.asNum();
    }
}

template<>
void Property<std::string>::set(const Atlas::Message::Element & e)
{
    if (e.isString()) {
        this->m_data = e.String();
    }
}

template class Property<int>;
template class Property<double>;
template class Property<std::string>;

#include "stubs/common/stubRouter.h"
#include "stubs/rulesets/stubScript.h"
#include "stubs/rulesets/stubTask.h"
BaseWorld * BaseWorld::m_instance = 0;

BaseWorld::BaseWorld(LocatedEntity & gw) : m_gameWorld(gw)
{
    m_instance = this;
}

BaseWorld::~BaseWorld()
{
    m_instance = 0;
}

LocatedEntity& BaseWorld::getDefaultLocation() const {
    return m_gameWorld;
}

LocatedEntity& BaseWorld::getDefaultLocation() {
    return m_gameWorld;
}

LocatedEntity * BaseWorld::getEntity(const std::string & id) const
{
    long intId = integerId(id);

    EntityDict::const_iterator I = m_eobjects.find(intId);
    if (I != m_eobjects.end()) {
        assert(I->second != 0);
        return I->second;
    } else {
        return 0;
    }
}

CorePropertyManager::CorePropertyManager()
{
}

CorePropertyManager::~CorePropertyManager()
{
}

PropertyBase * CorePropertyManager::addProperty(const std::string & name,
                                                int type)
{
    return 0;
}

int CorePropertyManager::installFactory(const std::string & type_name,
                                        const Root & type_desc,
                                        PropertyKit * factory)
{
    return 0;
}

PropertyManager * PropertyManager::m_instance = 0;

PropertyManager::PropertyManager()
{
    assert(m_instance == 0);
    m_instance = this;
}

PropertyManager::~PropertyManager()
{
   m_instance = 0;
}

int PropertyManager::installFactory(const std::string & type_name,
                                    const Atlas::Objects::Root & type_desc,
                                    PropertyKit * factory)
{
    return 0;
}

long integerId(const std::string & id)
{
    long intId = strtol(id.c_str(), 0, 10);
    if (intId == 0 && id != "0") {
        intId = -1L;
    }

    return intId;
}

TaskKit::TaskKit() : m_target(0), m_scriptFactory(0)
{
}

TaskKit::~TaskKit()
{
}

Root atlasClass(const std::string & name, const std::string & parent)
{
    Atlas::Objects::Entity::Anonymous r;
    r->setParent(parent);
    r->setObjtype("class");
    r->setId(name);
    return r;
}

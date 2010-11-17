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

// $Id$

#include "TestWorld.h"

#include "server/CorePropertyManager.h"
#include "server/EntityBuilder.h"
#include "server/EntityFactory.h"
#include "server/ScriptFactory.h"

#include "rulesets/World.h"
#include "rulesets/Creator.h"
#include "rulesets/Plant.h"
#include "rulesets/Stackable.h"
#include "rulesets/Script.h"
#include "rulesets/TaskScript.h"

#include "common/compose.hpp"
#include "common/id.h"
#include "common/Inheritance.h"
#include "common/log.h"
#include "common/Monitors.h"
#include "common/Property_impl.h"
#include "common/TypeNode.h"
#include "common/Variable.h"

#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/RootOperation.h>

#include <cstdlib>

#include <cassert>

using Atlas::Message::Element;
using Atlas::Message::ListType;
using Atlas::Message::MapType;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Root;

class ExposedEntityBuilder : public EntityBuilder {
  public:
    explicit ExposedEntityBuilder(BaseWorld & w) : EntityBuilder(w) { }

    EntityKit * getNewFactory(const std::string & clss) {
        return EntityBuilder::getNewFactory(clss);
    }

    const FactoryDict & factoryDict() const { return m_entityFactories; }

};

class TestScriptFactory : public ScriptKit {
  public:
    TestScriptFactory() : ScriptKit("foo", "bar") { }

    int addScript(Entity * entity) { return 0; }
    int refreshClass() { return 0; }
};

enum action {
  DO_NOTHING,
  SET_POS,
  SET_VELOCITY } LocatedEntity_merge_action = DO_NOTHING;

int main(int argc, char ** argv)
{
    {
        Entity e("1", 1);
        TestWorld test_world(e);
        Anonymous attributes;

        EntityBuilder::init(test_world);

        assert(EntityBuilder::instance() != 0);

        assert(EntityBuilder::instance()->newEntity("1", 1, "world", attributes) == 0);
        assert(EntityBuilder::instance()->newEntity("1", 1, "nonexistant", attributes) == 0);
        assert(EntityBuilder::instance()->newEntity("1", 1, "thing", attributes) != 0);

        EntityBuilder::del();
        assert(EntityBuilder::instance() == 0);
        Inheritance::clear();
    }

    {
        Entity e("1", 1);
        TestWorld test_world(e);
        Anonymous attributes;

        EntityBuilder::init(test_world);

        assert(EntityBuilder::instance() != 0);

        // Create a normal Entity
        Entity * test_ent = EntityBuilder::instance()->newEntity("1", 1, "thing", attributes);
        assert(test_ent != 0);

        // Create an entity specifying an attrbute
        attributes->setAttr("funky", "true");

        test_ent = EntityBuilder::instance()->newEntity("1", 1, "thing", attributes);
        assert(test_ent != 0);

        // Create an entity causing VELOCITY to be set
        attributes = Anonymous();

        attributes->setVelocity(std::vector<double>(3, 1.5));

        LocatedEntity_merge_action = SET_VELOCITY;

        test_ent = EntityBuilder::instance()->newEntity("1", 1, "thing", attributes);
        assert(test_ent != 0);

        LocatedEntity_merge_action = DO_NOTHING;

        // Create an entity causing VELOCITY to be set for no obvious reason
        attributes = Anonymous();

        LocatedEntity_merge_action = SET_VELOCITY;

        test_ent = EntityBuilder::instance()->newEntity("1", 1, "thing", attributes);
        assert(test_ent != 0);

        LocatedEntity_merge_action = DO_NOTHING;

        // Create an entity specifying a LOC
        attributes = Anonymous();

        attributes->setLoc("1");

        test_ent = EntityBuilder::instance()->newEntity("1", 1, "thing", attributes);
        assert(test_ent != 0);

        EntityBuilder::del();
        assert(EntityBuilder::instance() == 0);
        Inheritance::clear();
    }

    {
        // Create a test world.
        Entity e("1", 1);
        TestWorld test_world(e);

        // Instance of EntityBuilder with all protected methods exposed
        // for testing
        ExposedEntityBuilder entity_factory(test_world);

        // Attributes for test entities being created
        Anonymous attributes;

        // Create an entity which is an instance of one of the core classes
        Entity * test_ent = entity_factory.newEntity("1", 1, "thing", attributes);
        assert(test_ent != 0);

        Inheritance::clear();
    }

    {
        // Create a test world.
        Entity e("1", 1);
        TestWorld test_world(e);

        // Instance of EntityBuilder with all protected methods exposed
        // for testing
        ExposedEntityBuilder entity_factory(test_world);

        // Attributes for test entities being created
        Anonymous attributes;


        // Check that creating an entity of a type we know we have not yet
        // installed results in a null pointer.
        assert(entity_factory.newEntity("1", 1, "custom_type", attributes) == 0);

        // Get a reference to the internal dictionary of entity factories.
        const FactoryDict & factory_dict = entity_factory.factoryDict();

        // Make sure it has some factories in it already.
        assert(!factory_dict.empty());

        // Assert the dictionary does not contain the factory we know we have
        // have not yet installed.
        assert(factory_dict.find("custom_type") == factory_dict.end());

        // Set up a type description for a new type, and install it
        EntityKit * custom_type_factory = new EntityFactory<Entity>();
        custom_type_factory->m_attributes["test_custom_type_attr"] =
              "test_value";
        {
            Anonymous custom_type_desc;
            custom_type_desc->setId("custom_type");
            custom_type_desc->setParents(std::list<std::string>(1, "thing"));
            entity_factory.installFactory("custom_type", "thing",
                                          custom_type_factory, custom_type_desc);
        }

        custom_type_factory->m_type->defaults()["test_custom_type_attr"] = 
              new Property<std::string>; 
        custom_type_factory->m_type->defaults()["test_custom_type_attr"]->set("test_value");

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
        Entity * test_ent = entity_factory.newEntity("1", 1, "custom_type", attributes);
        assert(test_ent != 0);

        assert(test_ent->getType() == custom_type_factory->m_type);

        // Check that creating an entity of a type we know we have not yet
        // installed results in a null pointer.
        assert(entity_factory.newEntity("1", 1, "custom_inherited_type", attributes) == 0);

        // Assert the dictionary does not contain the factory we know we have
        // have not yet installed.
        assert(factory_dict.find("custom_inherited_type") == factory_dict.end());
        Inheritance::clear();
    }

    {
        Entity e("1", 1);
        TestWorld test_world(e);
        ExposedEntityBuilder entity_factory(test_world);
        Anonymous attributes;

        // Get a reference to the internal dictionary of entity factories.
        const FactoryDict & factory_dict = entity_factory.factoryDict();

        // Make sure it has some factories in it already.
        assert(!factory_dict.empty());

        // Assert the dictionary does not contain the factory we know we have
        // have not yet installed.
        assert(factory_dict.find("custom_scripted_type") == factory_dict.end());

        // Set up a type description for a new type, and install it
        EntityKit * custom_type_factory = new EntityFactory<Entity>();
        custom_type_factory->m_attributes["test_custom_type_attr"] =
              "test_value";

        custom_type_factory->m_scriptFactory = new TestScriptFactory();

        {
            Anonymous custom_type_desc;
            custom_type_desc->setId("custom_scripted_type");
            custom_type_desc->setParents(std::list<std::string>(1, "thing"));
            entity_factory.installFactory("custom_scripted_type", "thing",
                                          custom_type_factory, custom_type_desc);
        }

        // Check that the factory dictionary now contains a factory for
        // the custom type we just installed.
        FactoryDict::const_iterator I = factory_dict.find("custom_scripted_type");
        assert(I != factory_dict.end());
        assert(custom_type_factory == I->second);

        // Create an instance of our custom type, ensuring that it works.
        Entity * test_ent = entity_factory.newEntity("1", 1, "custom_scripted_type", attributes);
        assert(test_ent != 0);

        assert(test_ent->getType() == custom_type_factory->m_type);

        Inheritance::clear();
    }
}

// stubs

Script noScript;

Monitors * Monitors::m_instance = NULL;

Monitors::Monitors()
{
}

Monitors::~Monitors()
{
}

Monitors * Monitors::instance()
{
    if (m_instance == NULL) {
        m_instance = new Monitors();
    }
    return m_instance;
}

void Monitors::watch(const::std::string & name, VariableBase * monitor)
{
}

EntityKit::EntityKit() : m_scriptFactory(0), m_createdCount(0)
{
}

EntityKit::~EntityKit()
{
    if (m_scriptFactory != 0) {
        delete m_scriptFactory;
    }
}

template <>
Entity * EntityFactory<World>::newEntity(const std::string & id, long intId)
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
Entity * EntityFactory<T>::newEntity(const std::string & id, long intId)
{
    ++m_createdCount;
    return new Entity(id, intId);
}

template <class T>
int EntityFactory<T>::populate(Entity &)
{
    return 0;
}

template <class T>
EntityKit * EntityFactory<T>::duplicateFactory()
{
    EntityKit * f = new EntityFactory<T>(*this);
    f->m_parent = this;
    return f;
}

template class EntityFactory<Entity>;
template class EntityFactory<Thing>;
template class EntityFactory<Character>;
template class EntityFactory<Creator>;
template class EntityFactory<Plant>;
template class EntityFactory<Stackable>;
template class EntityFactory<World>;

Entity::Entity(const std::string & id, long intId) :
        LocatedEntity(id, intId), m_motion(0), m_flags(0)
{
}

Entity::~Entity()
{
}

void Entity::destroy()
{
    destroyed.emit();
}

void Entity::ActuateOperation(const Operation &, OpVector &)
{
}

void Entity::AppearanceOperation(const Operation &, OpVector &)
{
}

void Entity::AttackOperation(const Operation &, OpVector &)
{
}

void Entity::CombineOperation(const Operation &, OpVector &)
{
}

void Entity::CreateOperation(const Operation &, OpVector &)
{
}

void Entity::DeleteOperation(const Operation &, OpVector &)
{
}

void Entity::DisappearanceOperation(const Operation &, OpVector &)
{
}

void Entity::DivideOperation(const Operation &, OpVector &)
{
}

void Entity::EatOperation(const Operation &, OpVector &)
{
}

void Entity::ImaginaryOperation(const Operation &, OpVector &)
{
}

void Entity::LookOperation(const Operation &, OpVector &)
{
}

void Entity::MoveOperation(const Operation &, OpVector &)
{
}

void Entity::NourishOperation(const Operation &, OpVector &)
{
}

void Entity::SetOperation(const Operation &, OpVector &)
{
}

void Entity::SightOperation(const Operation &, OpVector &)
{
}

void Entity::SoundOperation(const Operation &, OpVector &)
{
}

void Entity::TalkOperation(const Operation &, OpVector &)
{
}

void Entity::TickOperation(const Operation &, OpVector &)
{
}

void Entity::TouchOperation(const Operation &, OpVector &)
{
}

void Entity::UpdateOperation(const Operation &, OpVector &)
{
}

void Entity::UseOperation(const Operation &, OpVector &)
{
}

void Entity::WieldOperation(const Operation &, OpVector &)
{
}

void Entity::externalOperation(const Operation & op)
{
}

void Entity::operation(const Operation & op, OpVector & res)
{
}

void Entity::addToMessage(Atlas::Message::MapType & omap) const
{
}

void Entity::addToEntity(const Atlas::Objects::Entity::RootEntity & ent) const
{
}

void Entity::setAttr(const std::string & name,
                     const Atlas::Message::Element & attr)
{
}

const PropertyBase * Entity::getProperty(const std::string & name) const
{
    return 0;
}

PropertyBase * Entity::modProperty(const std::string & name)
{
    return 0;
}

void Entity::onContainered()
{
}

void Entity::onUpdated()
{
}

LocatedEntity::LocatedEntity(const std::string & id, long intId) :
               Router(id, intId),
               m_refCount(0), m_seq(0),
               m_script(&noScript), m_type(0), m_contains(0)
{
}

LocatedEntity::~LocatedEntity()
{
}

bool LocatedEntity::hasAttr(const std::string & name) const
{
    return false;
}

bool LocatedEntity::getAttr(const std::string & name, Atlas::Message::Element & attr) const
{
    return false;
}

bool LocatedEntity::getAttrType(const std::string & name,
                                Atlas::Message::Element & attr,
                                int type) const
{
    return false;
}

void LocatedEntity::setAttr(const std::string & name, const Atlas::Message::Element & attr)
{
    return;
}

const PropertyBase * LocatedEntity::getProperty(const std::string & name) const
{
    return 0;
}

void LocatedEntity::onContainered()
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

void log(LogLevel lvl, const std::string & msg)
{
}

Inheritance * Inheritance::m_instance = NULL;

Inheritance::Inheritance()
{
    Atlas::Objects::Entity::Anonymous root_desc;

    root_desc->setParents(std::list<std::string>(0));
    root_desc->setObjtype("meta");
    root_desc->setId("root");

    TypeNode * root = new TypeNode;
    root->name() = "root";
    root->description() = root_desc;

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

TypeNode * Inheritance::addChild(const Root & obj,
                                 const PropertyDict & defaults)
{
    const std::string & child = obj->getId();
    const std::string & parent = obj->getParents().front();
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

    TypeNode * type = new TypeNode;
    type->name() = child;
    type->description() = obj;
    type->defaults() = defaults;
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

TypeNode::TypeNode() : m_parent(0)
{
}

TypeNode::~TypeNode()
{
}

PropertyBase::PropertyBase(unsigned int flags) : m_flags(flags)
{
}

PropertyBase::~PropertyBase()
{
}

void PropertyBase::install(Entity *)
{
}

void PropertyBase::apply(Entity *)
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

Router::Router(const std::string & id, long intId) : m_id(id),
                                                             m_intId(intId)
{
}

Router::~Router()
{
}

void Router::addToMessage(Atlas::Message::MapType & omap) const
{
}

void Router::addToEntity(const Atlas::Objects::Entity::RootEntity & ent) const
{
}

void Router::error(const Operation & op,
                   const std::string & errstring,
                   OpVector & res,
                   const std::string & to) const
{
}

Script::Script()
{
}

/// \brief Script destructor
Script::~Script()
{
}

bool Script::operation(const std::string & opname,
                       const Atlas::Objects::Operation::RootOperation & op,
                       OpVector & res)
{
   return false;
}

void Script::hook(const std::string & function, LocatedEntity * entity)
{
}

void Location::addToMessage(MapType & omap) const
{
}

Location::Location() : m_loc(0)
{
}

Location::Location(LocatedEntity * rf, const Point3D & pos)
{
}

void Location::addToEntity(const Atlas::Objects::Entity::RootEntity & ent) const
{
}

int Location::readFromEntity(const Atlas::Objects::Entity::RootEntity & ent)
{
    return 0;
}

TaskScript::TaskScript(Character & chr) : Task(chr), m_script(0)
{
}

TaskScript::~TaskScript()
{
}

void TaskScript::setScript(Script * scrpt)
{
}

void TaskScript::irrelevant()
{
}

void TaskScript::initTask(const Operation & op, OpVector & res)
{
}

void TaskScript::TickOperation(const Operation & op, OpVector & res)
{
}

Task::Task(Character & chr) : m_refCount(0), m_serialno(0), m_obsolete(false), m_progress(-1), m_rate(-1), m_character(chr)
{
}

Task::~Task()
{
}

void Task::irrelevant()
{
}

BaseWorld * BaseWorld::m_instance = 0;

BaseWorld::BaseWorld(Entity & gw) : m_gameWorld(gw)
{
    m_instance = this;
}

BaseWorld::~BaseWorld()
{
    m_instance = 0;
}

Entity * BaseWorld::getEntity(const std::string & id) const
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

long integerId(const std::string & id)
{
    long intId = strtol(id.c_str(), 0, 10);
    if (intId == 0 && id != "0") {
        intId = -1L;
    }

    return intId;
}

VariableBase::~VariableBase()
{
}

template <typename T>
Variable<T>::Variable(const T & variable) : m_variable(variable)
{
}

template <typename T>
Variable<T>::~Variable()
{
}

template <typename T>
void Variable<T>::send(std::ostream & o)
{
}

template class Variable<int>;
template class Variable<std::string>;
template class Variable<const char *>;

Root atlasClass(const std::string & name, const std::string & parent)
{
    Atlas::Objects::Entity::Anonymous r;
    r->setParents(std::list<std::string>(1, parent));
    r->setObjtype("class");
    r->setId(name);
    return r;
}

ScriptKit::ScriptKit(const std::string & package,
                     const std::string & type) : m_package(package),
                                                 m_type(type)
{
}

ScriptKit::~ScriptKit()
{
}

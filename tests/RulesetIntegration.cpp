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

#include "rulesets/World.h"
#include "rulesets/Python_API.h"
#include "rulesets/Character.h"

#include "server/Ruleset.h"
#include "server/EntityBuilder.h"
#include "server/EntityFactory.h"
#include "server/ArchetypeFactory.h"

#include "common/Inheritance.h"
#include "common/TypeNode.h"

#include <Atlas/Objects/Anonymous.h>

#include <cassert>

using Atlas::Message::MapType;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Root;

class ExposedEntityBuilder : public EntityBuilder {
  public:
    explicit ExposedEntityBuilder() : EntityBuilder() {
        m_instance = this;
    }

    const FactoryDict & factoryDict() const { return m_entityFactories; }

};

/// Integrations to try: Installation of all types of rules into
/// builders and factories. Creation of entity has all the right things.
/// Installation of rules via Admin. Persistence calls from Ruleset.
class Rulesetintegration : public Cyphesis::TestBase
{
  protected:
    World * m_entity;
    TestWorld * m_test_world;
    ExposedEntityBuilder * m_entity_builder;
  public:
    Rulesetintegration();

    void setup();
    void teardown();

    void test_init();
    void test_sequence();
    void test_property_type();
};

Rulesetintegration::Rulesetintegration()
{
    ADD_TEST(Rulesetintegration::test_init);
    ADD_TEST(Rulesetintegration::test_sequence);
    ADD_TEST(Rulesetintegration::test_property_type);
}

void Rulesetintegration::setup()
{
    m_entity = new World("1", 1);
    m_test_world = new TestWorld(*m_entity);
    m_entity_builder = new ExposedEntityBuilder();
}

void Rulesetintegration::teardown()
{
    delete m_entity_builder;
    delete m_entity;
    delete m_test_world;
    Inheritance::clear();
}

void Rulesetintegration::test_init()
{
    assert(Ruleset::instance() == 0);

    Ruleset::init("b08f221d-a177-45c7-be11-5be4195b6c40");

    assert(Ruleset::instance() != 0);

    Ruleset::del();
    assert(Ruleset::instance() == 0);
}

void Rulesetintegration::test_sequence()
{
    {
        Atlas::Message::Element val;

        // Instance of Ruleset with all protected methods exposed
        // for testing
        EntityBuilder * test_eb = EntityBuilder::instance();
        assert(test_eb == m_entity_builder);
        Ruleset test_ruleset(test_eb);

        // Attributes for test entities being created
        Anonymous attributes;

        // Create an entity which is an instance of one of the core classes
        LocatedEntity * test_ent = test_eb->newEntity("1", 1,
                                                      "thing",
                                                      attributes,
                                                      *m_test_world);
        assert(test_ent != 0);

        // Check that creating an entity of a type we know we have not yet
        // installed results in a null pointer.
        assert(test_eb->newEntity("1", 1, "custom_type", attributes, *m_test_world) == 0);

        // Set up a type description for a new type, and install it.
        {
            Root custom_type_description;
            MapType attrs;
            MapType test_custom_type_attr;
            test_custom_type_attr["default"] = "test_value";
            test_custom_type_attr["visibility"] = "public";
            attrs["test_custom_type_attr"] = test_custom_type_attr;
            custom_type_description->setAttr("attributes", attrs);
            custom_type_description->setId("custom_type");
            custom_type_description->setParent("thing");
            custom_type_description->setObjtype("class");

            int ret = test_ruleset.installRule("custom_type", "custom",
                                           custom_type_description);

            assert(ret == 0);
        }

        // Check that the factory dictionary now contains a factory for
        // the custom type we just installed.
        EntityFactoryBase * custom_type_factory = dynamic_cast<EntityFactoryBase*>(test_eb->getClassFactory("custom_type"));
        assert(custom_type_factory != 0);
        assert(custom_type_factory->m_type != 0);
        assert(custom_type_factory->m_type ==
               Inheritance::instance().getType("custom_type"));

        const Root & check_class = Inheritance::instance().getClass("custom_type");
        assert(check_class.isValid());
        assert(check_class->getId() == "custom_type");
        assert(check_class->getParent() == "thing");

        // Check the factory has the attributes we described on the custom
        // type.
        MapType::const_iterator J = custom_type_factory->m_attributes.find("test_custom_type_attr");
        assert(J != custom_type_factory->m_attributes.end());
        assert(J->second.isString());
        assert(J->second.String() == "test_value");

        J = custom_type_factory->m_classAttributes.find("test_custom_type_attr");
        assert(J != custom_type_factory->m_classAttributes.end());
        assert(J->second.isString());
        assert(J->second.String() == "test_value");

        TypeNode * custom_type_node = custom_type_factory->m_type;
        PropertyDict::const_iterator K = custom_type_node->defaults().find("test_custom_type_attr");
        assert(K != custom_type_node->defaults().end());
        Atlas::Message::Element custom_type_val;
        assert(K->second->get(custom_type_val) == 0);
        assert(custom_type_val == "test_value");

        // Create an instance of our custom type, ensuring that it works.
        test_ent = test_eb->newEntity("1", 1, "custom_type", attributes, *m_test_world);
        assert(test_ent != 0);

        // Reset val.
        val = Atlas::Message::Element();
        assert(val.isNone());

        // Reset val.
        val = Atlas::Message::Element();
        assert(val.isNone());

        // Check the custom type has the attribute described when the
        // custom type was installed.
        assert(test_ent->getAttr("test_custom_type_attr", val) == 0);
        assert(val.isString());
        assert(val.String() == "test_value");

        // Check that creating an entity of a type we know we have not yet
        // installed results in a null pointer.
        assert(test_eb->newEntity("1", 1, "custom_inherited_type", attributes, *m_test_world) == 0);

        // Set up a type description for a second new type which inherits
        // from the first, and install it.
        {
            Root custom_inherited_type_description;
            MapType attrs;
            MapType test_custom_type_attr;
            test_custom_type_attr["default"] = "test_inherited_value";
            test_custom_type_attr["visibility"] = "public";
            attrs["test_custom_inherited_type_attr"] = test_custom_type_attr;
            custom_inherited_type_description->setAttr("attributes", attrs);
            custom_inherited_type_description->setId("custom_inherited_type");
            custom_inherited_type_description->setParent("custom_type");
            custom_inherited_type_description->setObjtype("class");

            std::string dependent, reason;
            int ret = test_ruleset.installRule("custom_inherited_type",
                                               "custom",
                                           custom_inherited_type_description);

            assert(ret == 0);
            assert(dependent.empty());
            assert(reason.empty());
        }

        // Check that the factory dictionary does contain the factory for
        // the second newly installed type
        EntityFactoryBase * custom_inherited_type_factory = dynamic_cast<EntityFactoryBase*>(test_eb->getClassFactory("custom_inherited_type"));
        assert(custom_inherited_type_factory != 0);

        // Check that the factory has inherited the attributes from the
        // first custom type
        J = custom_inherited_type_factory->m_attributes.find("test_custom_type_attr");
        assert(J != custom_inherited_type_factory->m_attributes.end());
        assert(J->second.isString());
        assert(J->second.String() == "test_value");

        // Check that the factory has the attributes specified when installing
        // it
        J = custom_inherited_type_factory->m_attributes.find("test_custom_inherited_type_attr");
        assert(J != custom_inherited_type_factory->m_attributes.end());
        assert(J->second.isString());
        assert(J->second.String() == "test_inherited_value");

        // Creat an instance of the second custom type, ensuring it works.
        test_ent = test_eb->newEntity("1", 1, "custom_inherited_type", attributes, *m_test_world);
        assert(test_ent != 0);

        // Reset val.
        val = Atlas::Message::Element();
        assert(val.isNone());

        // Reset val.
        val = Atlas::Message::Element();
        assert(val.isNone());

        // Check the instance of the second custom type has the attribute
        // described when the first custom type was installed.
        assert(test_ent->getAttr("test_custom_type_attr", val) == 0);
        assert(val.isString());
        assert(val.String() == "test_value");

        // Reset val.
        val = Atlas::Message::Element();
        assert(val.isNone());

        // Check the custom type has the attribute described when the
        // second custom type was installed
        assert(test_ent->getAttr("test_custom_inherited_type_attr", val) == 0);
        assert(val.isString());
        assert(val.String() == "test_inherited_value");

        // FIXME TODO Modify a type, and ensure attribute propagate to inherited types.

        // Make sure than attempting to modify a non-existant type fails
        {
            Anonymous nonexistant_description;
            MapType attrs;
            MapType test_custom_type_attr;

            test_custom_type_attr["default"] = "no_value";
            test_custom_type_attr["visibility"] = "public";
            attrs["no_custom_type_attr"] = test_custom_type_attr;

            nonexistant_description->setId("nonexistant");
            nonexistant_description->setAttr("attributes", attrs);

            int ret = test_ruleset.modifyRule("nonexistant",
                                              nonexistant_description);

            assert(ret != 0);
        }

        // Modify the second custom type removing its custom attribute
        {
            Anonymous new_custom_inherited_type_description;
            new_custom_inherited_type_description->setObjtype("class");
            new_custom_inherited_type_description->setId("custom_inherited_type");
            new_custom_inherited_type_description->setAttr("attributes", MapType());

            // No parent
            int ret = test_ruleset.modifyRule("custom_inherited_type",
                                              new_custom_inherited_type_description);
            assert(ret != 0);

            // empty parent
            new_custom_inherited_type_description->setParent("");

            ret = test_ruleset.modifyRule("custom_inherited_type",
                                          new_custom_inherited_type_description);
            assert(ret != 0);

            // wrong parent
            new_custom_inherited_type_description->setParent("wrong_parent");

            ret = test_ruleset.modifyRule("custom_inherited_type",
                                          new_custom_inherited_type_description);
            assert(ret != 0);

            new_custom_inherited_type_description->setParent("custom_type");

            ret = test_ruleset.modifyRule("custom_inherited_type",
                                          new_custom_inherited_type_description);

            assert(ret == 0);
        }

        // Check that the factory dictionary does contain the factory for
        // the second newly installed type
        custom_inherited_type_factory = dynamic_cast<EntityFactoryBase*>(test_eb->getClassFactory("custom_inherited_type"));
        assert(custom_inherited_type_factory != 0);

        // Check that the factory has inherited the attributes from the
        // first custom type
        J = custom_inherited_type_factory->m_attributes.find("test_custom_type_attr");
        assert(J != custom_inherited_type_factory->m_attributes.end());
        assert(J->second.isString());
        assert(J->second.String() == "test_value");

        // Check that the factory no longer has the attributes we removed
        J = custom_inherited_type_factory->m_attributes.find("test_custom_inherited_type_attr");
        assert(J == custom_inherited_type_factory->m_attributes.end());

        // Creat an instance of the second custom type, ensuring it works.
        test_ent = test_eb->newEntity("1", 1, "custom_inherited_type", attributes, *m_test_world);
        assert(test_ent != 0);

        // Reset val.
        val = Atlas::Message::Element();
        assert(val.isNone());

        // Make sure test nonexistant attribute isn't present
        assert(test_ent->getAttr("nonexistant", val) != 0);
        // Make sure nonexistant attribute isn't present
        assert(test_ent->getAttr("nonexistant_attribute", val) != 0);

        // Reset val.
        val = Atlas::Message::Element();
        assert(val.isNone());

        // Reset val.
        val = Atlas::Message::Element();
        assert(val.isNone());

        // Check the instance of the second custom type has the attribute
        // described when the first custom type was installed.
        assert(test_ent->getAttr("test_custom_type_attr", val) == 0);
        assert(val.isString());
        assert(val.String() == "test_value");

        // Reset val.
        val = Atlas::Message::Element();
        assert(val.isNone());

        // Check the custom type has the attribute described when the
        // second custom type was installed
        assert(test_ent->getAttr("test_custom_inherited_type_attr", val) != 0);

        // Modify the first custom type removing its custom attribute
        {
            Anonymous new_custom_type_description;
            new_custom_type_description->setObjtype("class");
            new_custom_type_description->setId("custom_type");
            new_custom_type_description->setAttr("attributes", MapType());
            new_custom_type_description->setParent("thing");

            int ret = test_ruleset.modifyRule("custom_type", new_custom_type_description);

            assert(ret == 0);
        }

        // Check that the factory dictionary now contains a factory for
        // the custom type we just installed.
        custom_type_factory = dynamic_cast<EntityFactoryBase*>(test_eb->getClassFactory("custom_type"));
        assert(custom_type_factory != 0);

        // Check the factory has the attributes we described on the custom
        // type.
        J = custom_type_factory->m_attributes.find("test_custom_type_attr");
        assert(J == custom_type_factory->m_attributes.end());

        // Create an instance of our custom type, ensuring that it works.
        test_ent = test_eb->newEntity("1", 1, "custom_type", attributes, *m_test_world);
        assert(test_ent != 0);

        // Reset val.
        val = Atlas::Message::Element();
        assert(val.isNone());

        // Reset val.
        val = Atlas::Message::Element();
        assert(val.isNone());

        // Check the custom type no longer has the custom attribute
        assert(test_ent->getAttr("test_custom_type_attr", val) != 0);

        // Check that the factory dictionary does contain the factory for
        // the second newly installed type
        custom_inherited_type_factory = dynamic_cast<EntityFactoryBase*>(test_eb->getClassFactory("custom_inherited_type"));
        assert(custom_inherited_type_factory != 0);

        // Check that the factory no longer has inherited the attributes
        // from the first custom type which we removed
        J = custom_inherited_type_factory->m_attributes.find("test_custom_type_attr");
        assert(J == custom_inherited_type_factory->m_attributes.end());

        // Check that the factory no longer has the attributes we removed
        J = custom_inherited_type_factory->m_attributes.find("test_custom_inherited_type_attr");
        assert(J == custom_inherited_type_factory->m_attributes.end());

        // Creat an instance of the second custom type, ensuring it works.
        test_ent = test_eb->newEntity("1", 1, "custom_inherited_type", attributes, *m_test_world);
        assert(test_ent != 0);

        // Reset val.
        val = Atlas::Message::Element();
        assert(val.isNone());

        // Make sure test nonexistant attribute isn't present
        assert(test_ent->getAttr("nonexistant", val) != 0);
        // Make sure nonexistant attribute isn't present
        assert(test_ent->getAttr("nonexistant_attribute", val) != 0);

        // Reset val.
        val = Atlas::Message::Element();
        assert(val.isNone());

        // Reset val.
        val = Atlas::Message::Element();
        assert(val.isNone());

        // Check the instance of the second custom type has the attribute
        // described when the first custom type was installed.
        assert(test_ent->getAttr("test_custom_type_attr", val) != 0);

        // Reset val.
        val = Atlas::Message::Element();
        assert(val.isNone());

        // Check the custom type has the attribute described when the
        // second custom type was installed
        assert(test_ent->getAttr("test_custom_inherited_type_attr", val) != 0);

        // Add more custom attributes to the first type
        {
            Anonymous new_custom_type_description;
            new_custom_type_description->setObjtype("class");
            MapType attrs;
            MapType test_custom_type_attr;

            test_custom_type_attr["default"] = "test_value";
            test_custom_type_attr["visibility"] = "public";
            attrs["test_custom_type_attr"] = test_custom_type_attr;

            MapType new_custom_type_attr;

            new_custom_type_attr["default"] = "new_value";
            new_custom_type_attr["visibility"] = "public";
            attrs["new_custom_type_attr"] = new_custom_type_attr;

            new_custom_type_description->setId("custom_type");
            new_custom_type_description->setAttr("attributes", attrs);
            new_custom_type_description->setParent("thing");

            int ret = test_ruleset.modifyRule("custom_type", new_custom_type_description);

            assert(ret == 0);
            
        }

        // Check that the factory dictionary now contains a factory for
        // the custom type we just installed.
        custom_type_factory = dynamic_cast<EntityFactoryBase*>(test_eb->getClassFactory("custom_type"));

        // Check the factory has the attributes we described on the custom
        // type.
        J = custom_type_factory->m_attributes.find("test_custom_type_attr");
        assert(J != custom_type_factory->m_attributes.end());
        assert(J->second.isString());
        assert(J->second.String() == "test_value");

        J = custom_type_factory->m_attributes.find("new_custom_type_attr");
        assert(J != custom_type_factory->m_attributes.end());
        assert(J->second.isString());
        assert(J->second.String() == "new_value");

        // Create an instance of our custom type, ensuring that it works.
        test_ent = test_eb->newEntity("1", 1, "custom_type", attributes, *m_test_world);
        assert(test_ent != 0);

        // Reset val.
        val = Atlas::Message::Element();
        assert(val.isNone());

        // Reset val.
        val = Atlas::Message::Element();
        assert(val.isNone());

        // Check the custom type now has the custom attributes
        assert(test_ent->getAttr("test_custom_type_attr", val) == 0);
        assert(val.isString());
        assert(val.String() == "test_value");

        assert(test_ent->getAttr("new_custom_type_attr", val) == 0);
        assert(val.isString());
        assert(val.String() == "new_value");

        // Check that the factory dictionary does contain the factory for
        // the second newly installed type
        custom_inherited_type_factory = dynamic_cast<EntityFactoryBase*>(test_eb->getClassFactory("custom_inherited_type"));
        assert(custom_inherited_type_factory != 0);

        // Check that the factory now has inherited the attributes
        // from the first custom type
        J = custom_inherited_type_factory->m_attributes.find("test_custom_type_attr");
        assert(J != custom_inherited_type_factory->m_attributes.end());
        assert(J->second.isString());
        assert(J->second.String() == "test_value");

        J = custom_inherited_type_factory->m_attributes.find("new_custom_type_attr");
        assert(J != custom_inherited_type_factory->m_attributes.end());
        assert(J->second.isString());
        assert(J->second.String() == "new_value");

        // Check that the factory no longer has the attributes we removed
        J = custom_inherited_type_factory->m_attributes.find("test_custom_inherited_type_attr");
        assert(J == custom_inherited_type_factory->m_attributes.end());

        // Creat an instance of the second custom type, ensuring it works.
        test_ent = test_eb->newEntity("1", 1, "custom_inherited_type", attributes, *m_test_world);
        assert(test_ent != 0);

        // Reset val.
        val = Atlas::Message::Element();
        assert(val.isNone());

        // Make sure test nonexistant attribute isn't present
        assert(test_ent->getAttr("nonexistant", val) != 0);
        // Make sure nonexistant attribute isn't present
        assert(test_ent->getAttr("nonexistant_attribute", val) != 0);

        // Reset val.
        val = Atlas::Message::Element();
        assert(val.isNone());

        // Reset val.
        val = Atlas::Message::Element();
        assert(val.isNone());

        // Check the instance of the second custom type has the attribute
        // described when the first custom type was installed.
        assert(test_ent->getAttr("test_custom_type_attr", val) == 0);
        assert(val.isString());
        assert(val.String() == "test_value");

        assert(test_ent->getAttr("new_custom_type_attr", val) == 0);
        assert(val.isString());
        assert(val.String() == "new_value");

        // Reset val.
        val = Atlas::Message::Element();
        assert(val.isNone());

        // Check the custom type no longer has the attribute described when the
        // second custom type was installed
        assert(test_ent->getAttr("test_custom_inherited_type_attr", val) != 0);

    }
}

void Rulesetintegration::test_property_type()
{
    Ruleset test_ruleset(m_entity_builder);
    // Stub back in PropertyManager, and stub out everything it depends
    // on.
}

int main()
{
    database_flag = false;

    // init_python_api("6525a56d-7139-4016-8c1c-c2e77ab50039");

    Rulesetintegration t;

    return t.run();
}

void TestWorld::message(const Operation & op, LocatedEntity & ent)
{
}

LocatedEntity * TestWorld::addNewEntity(const std::string &,
                                 const Atlas::Objects::Entity::RootEntity &)
{
    return 0;
}

// stubs

#include "server/Connection.h"
#include "server/CorePropertyManager.h"
#include "server/Juncture.h"
#include "server/Player.h"
#include "server/ServerAccount.h"
#include "server/ServerRouting.h"

#include "rulesets/PythonScriptFactory.h"
#include "rulesets/Task.h"
#include "rulesets/Creator.h"
#include "rulesets/Plant.h"
#include "rulesets/Stackable.h"

#include "stubs/rulesets/stubCreator.h"
#include "stubs/server/stubAdmin.h"
#include "stubs/modules/stubLocation.h"

Account::Account(Connection * conn,
                 const std::string & uname,
                 const std::string & passwd,
                 const std::string & id,
                 long intId) :
         ConnectableRouter(id, intId, conn),
         m_username(uname), m_password(passwd)
{
}

Account::~Account()
{
}

LocatedEntity * Account::addNewCharacter(const std::string & typestr,
                                  const RootEntity & ent,
                                  const Root & arg)
{
    return 0;
}

LocatedEntity * Account::createCharacterEntity(const std::string &,
                                const Atlas::Objects::Entity::RootEntity &,
                                const Atlas::Objects::Root &)
{
    return 0;
}

int Account::connectCharacter(LocatedEntity *chr)
{
    return 0;
}

const char * Account::getType() const
{
    return "account";
}

void Account::store() const
{
}

bool Account::isPersisted() const {
    return true;
}

void Account::createObject(const std::string & type_str,
                           const Root & arg,
                           const Operation & op,
                           OpVector & res)
{
}

void Account::addCharacter(LocatedEntity * chr)
{
}

void Account::addToMessage(MapType & omap) const
{
}

void Account::addToEntity(const RootEntity & ent) const
{
}

void Account::externalOperation(const Operation & op, Link &)
{
}

void Account::operation(const Operation & op, OpVector & res)
{
}

void Account::LogoutOperation(const Operation &, OpVector &)
{
}

void Account::CreateOperation(const Operation &, OpVector &)
{
}

void Account::SetOperation(const Operation &, OpVector &)
{
}

void Account::ImaginaryOperation(const Operation &, OpVector &)
{
}

void Account::TalkOperation(const Operation &, OpVector &)
{
}

void Account::LookOperation(const Operation &, OpVector &)
{
}

void Account::GetOperation(const Operation &, OpVector &)
{
}

void Account::OtherOperation(const Operation &, OpVector &)
{
}

ConnectableRouter::ConnectableRouter(const std::string & id,
                                 long iid,
                                 Connection *c) :
                 Router(id, iid),
                 m_connection(c)
{
}

ConnectableRouter::~ConnectableRouter()
{
}

#include "stubs/server/stubConnection.h"

CorePropertyManager::CorePropertyManager()
{
}

CorePropertyManager::~CorePropertyManager()
{
}

PropertyBase * CorePropertyManager::addProperty(const std::string & name,
                                                int type)
{
    return new SoftProperty;
}

int CorePropertyManager::installFactory(const std::string & type_name,
                                        const Atlas::Objects::Root & type_desc,
                                        PropertyKit * factory)
{
    return 0;
}

Plant::Plant(const std::string& id, long idInt)
:Thing::Thing(id, idInt)
{
}

Plant::~Plant(){}

void Plant::NourishOperation(const Operation & op, OpVector &)
{
}

void Plant::TickOperation(const Operation & op, OpVector &)
{
}

void Plant::TouchOperation(const Operation & op, OpVector &)
{
}

Stackable::Stackable(const std::string& id, long idInt)
:Thing::Thing(id, idInt)
{
}

Stackable::~Stackable(){}

void Stackable::CombineOperation(const Operation & op, OpVector &)
{
}

void Stackable::DivideOperation(const Operation & op, OpVector &)
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
    return new Entity(id, intId);
}

class World;

Juncture::Juncture(Connection * c, const std::string & id, long iid) :
          ConnectableRouter(id, iid, c),
          m_address(0),
          m_peer(0),
          m_connectRef(0)
{
}

Juncture::~Juncture()
{
}

void Juncture::externalOperation(const Operation & op, Link &)
{
}

void Juncture::operation(const Operation & op, OpVector & res)
{
}

void Juncture::addToMessage(MapType & omap) const
{
}

void Juncture::addToEntity(const RootEntity & ent) const
{
}

std::set<std::string> Player::playableTypes;

Player::Player(Connection * conn,
               const std::string & username,
               const std::string & passwd,
               const std::string & id,
               long intId) :
        Account(conn, username, passwd, id, intId)
{
}

Player::~Player() { }

const char * Player::getType() const
{
    return "player";
}

void Player::addToMessage(MapType & omap) const
{
}

void Player::addToEntity(const Atlas::Objects::Entity::RootEntity & ent) const
{
}

int Player::characterError(const Operation & op,
                           const Root & ent, OpVector & res) const
{
    return 0;
}

ServerAccount::ServerAccount(Connection * conn,
             const std::string & username,
             const std::string & passwd,
             const std::string & id,
             long intId) :
       Account(conn, username, passwd, id, intId)
{
}

ServerAccount::~ServerAccount()
{
}

const char * ServerAccount::getType() const
{
    return "server";
}

int ServerAccount::characterError(const Operation & op,
                                  const Root & ent,
                                  OpVector & res) const
{
    return -1;
}

void ServerAccount::createObject(const std::string & type_str,
                                 const Root & arg,
                                 const Operation & op,
                                 OpVector & res)
{
}

void ServerRouting::addObject(Router * obj)
{
}

Router * ServerRouting::getObject(const std::string & id) const
{
    return 0;
}

Entity::Entity(const std::string & id, long intId) :
        LocatedEntity(id, intId)
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

void Entity::GetOperation(const Operation &, OpVector &)
{
}

void Entity::InfoOperation(const Operation &, OpVector &)
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

void Entity::RelayOperation(const Operation &, OpVector &)
{
}

void Entity::externalOperation(const Operation & op, Link &)
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

PropertyBase * Entity::setAttr(const std::string & name,
                               const Atlas::Message::Element & attr)
{
    return 0;
}

const PropertyBase * Entity::getProperty(const std::string & name) const
{
    return 0;
}

PropertyBase * Entity::setProperty(const std::string & name,
                                   PropertyBase * prop)
{
    return m_properties[name] = prop;
}

PropertyBase * Entity::modProperty(const std::string & name)
{
    return 0;
}

void Entity::installDelegate(int class_no, const std::string & delegate)
{
}

void Entity::removeDelegate(int class_no, const std::string & delegate)
{
}

Domain * Entity::getMovementDomain()
{
    return 0;
}

const Domain * Entity::getMovementDomain() const
{
    return 0;
}

void Entity::sendWorld(const Operation & op)
{
}

void Entity::onContainered(const LocatedEntity*)
{
}

void Entity::onUpdated()
{
}

void Entity::setType(const TypeNode* t) {
    m_type = t;
}

LocatedEntity::LocatedEntity(const std::string & id, long intId) :
               Router(id, intId),
               m_refCount(0), m_seq(0),
               m_script(0), m_type(0), m_flags(0), m_contains(0)
{
}

LocatedEntity::~LocatedEntity()
{
}

void LocatedEntity::makeContainer()
{
}

bool LocatedEntity::hasAttr(const std::string & name) const
{
    return false;
}

int LocatedEntity::getAttr(const std::string & name,
                           Atlas::Message::Element & attr) const
{
    PropertyDict::const_iterator I = m_properties.find(name);
    if (I != m_properties.end()) {
        return I->second->get(attr);
    }
    if (m_type != 0) {
        I = m_type->defaults().find(name);
        if (I != m_type->defaults().end()) {
            return I->second->get(attr);
        }
    }
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

void LocatedEntity::merge(const MapType & ent)
{
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
void LocatedEntity::addChild(LocatedEntity& childEntity)
{
}

void LocatedEntity::removeChild(LocatedEntity& childEntity)
{
}

void LocatedEntity::setType(const TypeNode* t) {
    m_type = t;
}
std::vector<Atlas::Objects::Root> LocatedEntity::getThoughts() const
{
    return std::vector<Atlas::Objects::Root>();
}
PythonClass::PythonClass(const std::string & package,
                         const std::string & type,
                         struct _typeobject * base) : m_package(package),
                                                      m_type(type),
                                                      m_base(base),
                                                      m_module(0),
                                                      m_class(0)
{
}

PythonClass::~PythonClass()
{
}

int PythonClass::load()
{
    return 0;
}

int PythonClass::getClass(struct _object *)
{
    return 0;
}

int PythonClass::refresh()
{
    return 0;
}

template <class T>
PythonScriptFactory<T>::PythonScriptFactory(const std::string & package,
                                         const std::string & type) :
                                         PythonClass(package,
                                                     type,
                                                     0)
{
}

template <class T>
PythonScriptFactory<T>::~PythonScriptFactory()
{
}

template <class T>
int PythonScriptFactory<T>::setup()
{
    return 0;
}

template <class T>
const std::string & PythonScriptFactory<T>::package() const
{
    return m_package;
}

template <class T>
int PythonScriptFactory<T>::addScript(T * entity) const
{
    return 0;
}

template <class T>
int PythonScriptFactory<T>::refreshClass()
{
    return 0;
}

template class PythonScriptFactory<LocatedEntity>;
template class PythonScriptFactory<Task>;

void Task::initTask(const Operation & op, OpVector & res)
{
}

void Task::operation(const Operation & op, OpVector & res)
{
}

Task::Task(LocatedEntity & chr) : m_refCount(0), m_serialno(0), m_obsolete(false
), m_progress(-1), m_rate(-1), m_owner(chr)
{
}

Task::~Task()
{
}

void Task::irrelevant()
{
}

Thing::Thing(const std::string & id, long intId) :
       Entity(id, intId)
{
}

Thing::~Thing()
{
}

void Thing::DeleteOperation(const Operation & op, OpVector & res)
{
}

void Thing::MoveOperation(const Operation & op, OpVector & res)
{
}

void Thing::SetOperation(const Operation & op, OpVector & res)
{
}

void Thing::LookOperation(const Operation & op, OpVector & res)
{
}

void Thing::CreateOperation(const Operation & op, OpVector & res)
{
}

void Thing::UpdateOperation(const Operation & op, OpVector & res)
{
}

World::World(const std::string & id, long intId) : Thing(id, intId)
{
}

World::~World()
{
}

void World::LookOperation(const Operation & op, OpVector & res)
{
}

void World::MoveOperation(const Operation & op, OpVector & res)
{
}

void World::DeleteOperation(const Operation & op, OpVector & res)
{
}

void World::RelayOperation(const Operation & op, OpVector & res)
{
}


Character::Character(const std::string& id, long int intId) :
        Thing(id, intId), m_movement(*(Movement*)(nullptr)){

}

Character::~Character() {

}

void Character::metabolise(OpVector & res, double ammount)
{
}

void Character::wieldDropped()
{
}

LocatedEntity * Character::findInContains(LocatedEntity * ent,
                                          const std::string & id)
{
  return nullptr;
}

LocatedEntity * Character::findInInventory(const std::string & id)
{
  return nullptr;
}

int Character::linkExternal(Link * link)
{
    return 0;
}

int Character::unlinkExternal(Link * link)
{
    return 0;
}

int Character::startTask(Task * task, const Operation & op, OpVector & res)
{
  return 0;
}

void Character::updateTask(OpVector & res)
{
}

void Character::clearTask(OpVector & res)
{
}

void Character::ImaginaryOperation(const Operation & op, OpVector & res)
{
}

void Character::InfoOperation(const Operation & op, OpVector & res)
{
}

void Character::TickOperation(const Operation & op, OpVector & res)
{
}

void Character::TalkOperation(const Operation & op, OpVector & res)
{
}

void Character::NourishOperation(const Operation & op, OpVector & res)
{
}

void Character::UseOperation(const Operation & op, OpVector & res)
{
}

void Character::WieldOperation(const Operation & op, OpVector & res)
{
}

void Character::AttackOperation(const Operation & op, OpVector & res)
{
}

void Character::ActuateOperation(const Operation & op, OpVector & res)
{
}

void Character::RelayOperation(const Operation & op, OpVector &)
{
}

void Character::mindActuateOperation(const Operation & op, OpVector & res)
{
}

void Character::mindAttackOperation(const Operation & op, OpVector & res)
{
}

void Character::mindSetupOperation(const Operation & op, OpVector & res)
{
}

void Character::mindUseOperation(const Operation & op, OpVector & res)
{
}

void Character::mindUpdateOperation(const Operation & op, OpVector & res)
{
}

void Character::mindWieldOperation(const Operation & op, OpVector & res)
{
}

void Character::mindTickOperation(const Operation & op, OpVector & res)
{
}

void Character::mindMoveOperation(const Operation & op, OpVector & res)
{
}

void Character::mindSetOperation(const Operation & op, OpVector & res)
{
}

void Character::mindCombineOperation(const Operation & op, OpVector & res)
{
}

void Character::mindCreateOperation(const Operation & op, OpVector & res)
{
}

void Character::mindDeleteOperation(const Operation & op, OpVector & res)
{
}

void Character::mindDivideOperation(const Operation & op, OpVector & res)
{
}

void Character::mindImaginaryOperation(const Operation & op, OpVector & res)
{
}

void Character::mindTalkOperation(const Operation & op, OpVector & res)
{
}

void Character::mindLookOperation(const Operation & op, OpVector & res)
{
}

void Character::mindEatOperation(const Operation & op, OpVector & res)
{
}

void Character::mindGoalInfoOperation(const Operation & op, OpVector & res)
{
}

void Character::mindTouchOperation(const Operation & op, OpVector & res)
{
}

void Character::mindOtherOperation(const Operation & op, OpVector & res)
{
}
void Character::mindThinkOperation(const Operation & op, OpVector & res)
{
}
std::vector<Atlas::Objects::Root> Character::getThoughts() const
{
    return std::vector<Atlas::Objects::Root>();
}

bool Character::w2mAppearanceOperation(const Operation & op)
{
    return true;
}

bool Character::w2mDisappearanceOperation(const Operation & op)
{
    return true;
}

bool Character::w2mErrorOperation(const Operation & op)
{
    return true;
}

bool Character::w2mSetupOperation(const Operation & op)
{
    return false;
}

void Character::mindThoughtOperation(const Operation & op, OpVector & res)
{
}

bool Character::w2mTickOperation(const Operation & op)
{
    return false;
}

bool Character::w2mUnseenOperation(const Operation & op)
{
    return true;
}

bool Character::w2mSightOperation(const Operation & op)
{
    return true;
}

bool Character::w2mSoundOperation(const Operation & op)
{
    return true;
}

bool Character::w2mTouchOperation(const Operation & op)
{
    return true;
}

void Character::sendMind(const Operation & op, OpVector & res)
{
}

void Character::mind2body(const Operation & op, OpVector & res)
{
}

bool Character::world2mind(const Operation & op)
{
    return false;
}

void Character::filterExternalOperation(const Operation & op)
{
}

void Character::operation(const Operation & op, OpVector & res)
{
}

void Character::externalOperation(const Operation & op, Link & link)
{
}

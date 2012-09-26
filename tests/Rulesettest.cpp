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

#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include "server/Ruleset.h"
#include "server/EntityBuilder.h"

#include "common/const.h"
#include "common/globals.h"
#include "common/Inheritance.h"

#include <Atlas/Objects/Anonymous.h>

#include <cassert>

using Atlas::Message::MapType;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Root;

class ExposedRuleset : public Ruleset {
  public:
    ExposedRuleset(EntityBuilder * eb) : Ruleset(eb) { }

    void getRulesFromFiles(const std::string & ruleset,
                           std::map<std::string, Root> & rules) {
        Ruleset::getRulesFromFiles(ruleset, rules);
    }

};

const std::string data_path = TESTDATADIR;

int main(int argc, char ** argv)
{
    int ret;

    std::string ruleset("caaa1085-9ef4-4dc2-b1ad-3d1f15b31060");

    {
        database_flag = true;
        EntityBuilder::init();
        Ruleset::init(ruleset);

        assert(Ruleset::instance() != 0);

        assert(EntityBuilder::instance() != 0);

        Ruleset::del();
        assert(Ruleset::instance() == 0);
        EntityBuilder::del();
        assert(EntityBuilder::instance() == 0);
        Inheritance::clear();
    }

    {
        database_flag = false;
        etc_directory = data_path + "/ruleset1/etc";
        EntityBuilder::init();
        Ruleset::init(ruleset);

        assert(Ruleset::instance() != 0);

        assert(EntityBuilder::instance() != 0);

        Ruleset::del();
        assert(Ruleset::instance() == 0);
        EntityBuilder::del();
        assert(EntityBuilder::instance() == 0);
        Inheritance::clear();
    }

    {
        database_flag = false;
        etc_directory = data_path + "/ruleset2/etc";
        EntityBuilder::init();
        Ruleset::init(ruleset);

        assert(Ruleset::instance() != 0);

        assert(EntityBuilder::instance() != 0);

        Ruleset::del();
        assert(Ruleset::instance() == 0);
        EntityBuilder::del();
        assert(EntityBuilder::instance() == 0);
        Inheritance::clear();
    }

    {
        // Create a test world.

        // Instance of Ruleset with all protected methods exposed
        // for testing
        EntityBuilder::init();
        EntityBuilder * test_eb = EntityBuilder::instance();
        ExposedRuleset test_ruleset(test_eb);

        // Attributes for test entities being created
        Anonymous attributes;

        // Set up a simple class description for a new type, and install it.
        {
            Root custom_type_description;
            custom_type_description->setObjtype("class");
            custom_type_description->setId("f134c3e0");
            custom_type_description->setParents(std::list<std::string>(1, "thing"));

            ret = test_ruleset.installRule("f134c3e0", "custom",
                                           custom_type_description);
            // Add this to inheritance, so future tests work
            Inheritance::instance().addChild(custom_type_description);

            assert(ret == 0);
        }

        // Set up a simple class description for a new type, and install it.
        {
            Root custom_type_description;
            custom_type_description->setObjtype("class");
            custom_type_description->setId("667aa324");
            custom_type_description->setParents(std::list<std::string>(1, "task"));

            ret = test_ruleset.installRule("667aa324", "custom",
                                           custom_type_description);
            // Add this to inheritance, so future tests work
            Inheritance::instance().addChild(custom_type_description);

            assert(ret == 0);
        }

        // Set up a simple class description for a new type, and install it.
        {
            Root custom_type_description;
            custom_type_description->setObjtype("op_definition");
            custom_type_description->setId("17c4e87e");
            custom_type_description->setParents(std::list<std::string>(1, "get"));

            ret = test_ruleset.installRule("17c4e87e", "custom",
                                           custom_type_description);
            // Add this to inheritance, so future tests work
            Inheritance::instance().addChild(custom_type_description);

            assert(ret == 0);
        }

        // Overlength name
        {
            std::string class_name(consts::id_len * 2, 'f');
            Root custom_type_description;
            custom_type_description->setObjtype("class");
            custom_type_description->setId(class_name);
            custom_type_description->setParents(std::list<std::string>(1, "thing"));

            ret = test_ruleset.installRule(class_name, "custom",
                                           custom_type_description);
            // Add this to inheritance, so future tests work
            Inheritance::instance().addChild(custom_type_description);

            assert(ret == -1);
        }

        // Empty parents list
        {
            std::string class_name("2cde57d0");
            Root custom_type_description;
            custom_type_description->setObjtype("class");
            custom_type_description->setId(class_name);
            custom_type_description->setParents(std::list<std::string>());

            ret = test_ruleset.installRule(class_name, "custom",
                                           custom_type_description);
            // Add this to inheritance, so future tests work
            Inheritance::instance().addChild(custom_type_description);

            assert(ret == -1);
        }

        // Empty parent string
        {
            std::string class_name("beb29f55");
            Root custom_type_description;
            custom_type_description->setObjtype("class");
            custom_type_description->setId(class_name);
            custom_type_description->setParents(std::list<std::string>(1, ""));

            ret = test_ruleset.installRule(class_name, "custom",
                                           custom_type_description);
            // Add this to inheritance, so future tests work
            Inheritance::instance().addChild(custom_type_description);

            assert(ret == -1);
        }

        // Unknown kind of rule
        {
            std::string class_name("9394c6dd");
            Root custom_type_description;
            custom_type_description->setObjtype("481c53630c98");
            custom_type_description->setId(class_name);
            custom_type_description->setParents(std::list<std::string>(1, "thing"));

            ret = test_ruleset.installRule(class_name, "custom",
                                           custom_type_description);
            // Add this to inheritance, so future tests work
            Inheritance::instance().addChild(custom_type_description);

            assert(ret == -1);
        }

        // Set up a type description for a new type, and install it.
        {
            Root custom_type_description;
            custom_type_description->setObjtype("class");
            MapType attrs;
            MapType test_custom_type_attr;
            test_custom_type_attr["default"] = "test_value";
            test_custom_type_attr["visibility"] = "public";
            attrs["test_custom_type_attr"] = test_custom_type_attr;
            custom_type_description->setAttr("attributes", attrs);
            custom_type_description->setId("custom_type");
            custom_type_description->setParents(std::list<std::string>(1, "thing"));

            ret = test_ruleset.installRule("custom_type", "custom",
                                           custom_type_description);
            // Add this to inheritance, so future tests work
            Inheritance::instance().addChild(custom_type_description);

            assert(ret == 0);
        }

        // Set up a type description for a second new type which inherits
        // from the first, and install it.
        {
            Root custom_inherited_type_description;
            custom_inherited_type_description->setObjtype("class");
            MapType attrs;
            MapType test_custom_type_attr;
            test_custom_type_attr["default"] = "test_inherited_value";
            test_custom_type_attr["visibility"] = "public";
            attrs["test_custom_inherited_type_attr"] = test_custom_type_attr;
            custom_inherited_type_description->setAttr("attributes", attrs);
            custom_inherited_type_description->setId("custom_inherited_type");
            custom_inherited_type_description->setParents(std::list<std::string>(1, "custom_type"));

            ret = test_ruleset.installRule("custom_inherited_type", "custom",
                                           custom_inherited_type_description);

            // Add this to inheritance, so future tests work
            Inheritance::instance().addChild(custom_inherited_type_description);
            assert(ret == 0);
        }

        // Make sure than attempting to modify a non-existant type fails
        {
            Anonymous nonexistant_description;
            nonexistant_description->setObjtype("class");
            MapType attrs;
            MapType test_custom_type_attr;

            test_custom_type_attr["default"] = "no_value";
            test_custom_type_attr["visibility"] = "public";
            attrs["no_custom_type_attr"] = test_custom_type_attr;

            nonexistant_description->setId("nonexistant");
            nonexistant_description->setAttr("attributes", attrs);

            ret = test_ruleset.modifyRule("nonexistant", nonexistant_description);

            assert(ret != 0);
        }

        // Modify the second custom type removing its custom attribute
        {
            Anonymous new_custom_inherited_type_description;
            new_custom_inherited_type_description->setObjtype("class");
            new_custom_inherited_type_description->setId("custom_inherited_type");
            new_custom_inherited_type_description->setAttr("attributes", MapType());

            // No parents
            ret = test_ruleset.modifyRule("custom_inherited_type",
                                          new_custom_inherited_type_description);
            assert(ret != 0);

            // empty parents
            new_custom_inherited_type_description->setParents(std::list<std::string>());

            ret = test_ruleset.modifyRule("custom_inherited_type",
                                          new_custom_inherited_type_description);
            assert(ret != 0);

            // wrong parents
            new_custom_inherited_type_description->setParents(std::list<std::string>(1, "wrong_parent"));

            ret = test_ruleset.modifyRule("custom_inherited_type",
                                          new_custom_inherited_type_description);
            assert(ret != 0);

            new_custom_inherited_type_description->setParents(std::list<std::string>(1, "custom_type"));

            ret = test_ruleset.modifyRule("custom_inherited_type",
                                          new_custom_inherited_type_description);

            assert(ret == 0);
        }

        // Modify the first custom type removing its custom attribute
        {
            Anonymous new_custom_type_description;
            new_custom_type_description->setObjtype("class");
            new_custom_type_description->setId("custom_type");
            new_custom_type_description->setAttr("attributes", MapType());
            new_custom_type_description->setParents(std::list<std::string>(1, "thing"));

            ret = test_ruleset.modifyRule("custom_type", new_custom_type_description);

            assert(ret == 0);
        }

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
            new_custom_type_description->setParents(std::list<std::string>(1, "thing"));

            ret = test_ruleset.modifyRule("custom_type", new_custom_type_description);

            assert(ret == 0);
            
        }
    }
}

// stub functions

#include "server/EntityRuleHandler.h"
#include "server/OpRuleHandler.h"
#include "server/TaskRuleHandler.h"
#include "server/Persistence.h"

#include "common/AtlasFileLoader.h"
#include "common/log.h"
#include "common/TypeNode.h"

int OpRuleHandler::check(const Atlas::Objects::Root & desc)
{
    if (desc->getObjtype() != "op_definition") {
        return -1;
    }
    return 0;
}

int OpRuleHandler::install(const std::string & name,
                           const std::string & parent,
                           const Atlas::Objects::Root & description,
                           std::string & dependent,
                           std::string & reason)
{
    return 0;
}

int OpRuleHandler::update(const std::string & name,
                          const Atlas::Objects::Root & desc)
{
    return 0;
}

int EntityRuleHandler::check(const Atlas::Objects::Root & desc)
{
    if (desc->getObjtype() != "class") {
        return -1;
    }
    return m_builder->isTask(desc->getParents().front()) ? -1 : 0;
}

int EntityRuleHandler::install(const std::string & name,
                             const std::string & parent,
                             const Atlas::Objects::Root & description,
                             std::string & dependent,
                             std::string & reason)
{
    return 0;
}

int EntityRuleHandler::update(const std::string & name,
                            const Atlas::Objects::Root & desc)
{
    return 0;
}

int TaskRuleHandler::check(const Atlas::Objects::Root & desc)
{
    return m_builder->isTask(desc->getParents().front()) ? 0 : -1;
}

int TaskRuleHandler::install(const std::string & name,
                             const std::string & parent,
                             const Atlas::Objects::Root & description,
                             std::string & dependent,
                             std::string & reason)
{
    return 0;
}

int TaskRuleHandler::update(const std::string & name,
                            const Atlas::Objects::Root & desc)
{
    return 0;
}

RuleHandler::~RuleHandler()
{
}

EntityBuilder * EntityBuilder::m_instance = NULL;

EntityBuilder::EntityBuilder()
{
}

EntityBuilder::~EntityBuilder()
{
}

bool EntityBuilder::isTask(const std::string & class_name)
{
    if (class_name == "task") {
        return true;
    }
    return (m_taskFactories.find(class_name) != m_taskFactories.end());
}

Persistence * Persistence::m_instance = NULL;

Persistence::Persistence() : m_db(*(Database*)0)
{
}

Persistence * Persistence::instance()
{
    if (m_instance == NULL) {
        m_instance = new Persistence();
    }
    return m_instance;
}

int Persistence::updateRule(const Atlas::Objects::Root & rule,
                            const std::string & key)
{
    return 0;
}

int Persistence::getRules(std::map<std::string, Root> & t)
{
    return 0;
}

int Persistence::storeRule(const Atlas::Objects::Root & rule,
                           const std::string & key,
                           const std::string & ruleset)
{
    return 0;
}

AtlasFileLoader::AtlasFileLoader(const std::string & filename,
                                 std::map<std::string, Root> & m) :
                m_file(filename.c_str(), std::ios::in),
                m_count(0), m_messages(m)
{
}

AtlasFileLoader::~AtlasFileLoader()
{
}

void AtlasFileLoader::objectArrived(const Root & obj)
{
}

bool AtlasFileLoader::isOpen()
{
    return true;
}

/// Read input file to atlas codec.
void AtlasFileLoader::read()
{
}

Inheritance * Inheritance::m_instance = NULL;

Inheritance::Inheritance() : noClass(0)
{
}

Inheritance & Inheritance::instance()
{
    if (m_instance == NULL) {
        m_instance = new Inheritance();
    }
    return *m_instance;
}

void Inheritance::flush()
{
}

void Inheritance::clear()
{
    if (m_instance != NULL) {
        m_instance->flush();
        delete m_instance;
        m_instance = NULL;
    }
}

const Root & Inheritance::getClass(const std::string & parent)
{
    TypeNodeDict::const_iterator I = atlasObjects.find(parent);
    if (I == atlasObjects.end()) {
        return noClass;
    }
    return I->second->description();
}

TypeNode * Inheritance::addChild(const Root & obj)
{
    const std::string & child = obj->getId();
    TypeNode * type = new TypeNode(child, obj);
    atlasObjects[child] = type;
    return type;
}

int Inheritance::updateClass(const std::string & parent,
                             const Root & description)
{
    return 0;
}

TypeNode::TypeNode(const std::string & name,
                   const Atlas::Objects::Root & d) : m_name(name),
                                                     m_description(d),
                                                     m_parent(0)
{
}

void log(LogLevel lvl, const std::string & msg)
{
}

std::string etc_directory;
bool database_flag = true;


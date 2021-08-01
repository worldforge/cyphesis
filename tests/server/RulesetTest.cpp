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

#include "rules/LocatedEntity.h"

#include "server/Ruleset.h"
#include "server/EntityBuilder.h"

#include "common/const.h"
#include "common/globals.h"
#include "common/Inheritance.h"
#include "../NullPropertyManager.h"

#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/Factories.h>

#include <cassert>

using Atlas::Message::MapType;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Root;

class ExposedRuleset : public Ruleset {
  public:
    ExposedRuleset(EntityBuilder & eb, boost::asio::io_context& io_context, PropertyManager& propertyManager) : Ruleset(eb, io_context, propertyManager) { }

    void test_getRulesFromFiles(const std::string & ruleset,
                           std::map<std::string, Root> & rules) {
        Ruleset::getRulesFromFiles(ruleset, rules);
    }

};

const std::string data_path = TESTDATADIR;

int main(int argc, char ** argv)
{
    boost::asio::io_context io_context;
    int ret;
    NullPropertyManager propertyManager;

    std::string ruleset("caaa1085-9ef4-4dc2-b1ad-3d1f15b31060");

    {
        Atlas::Objects::Factories factories;
        Inheritance inheritance(factories);
        EntityBuilder eb;
        {
            Ruleset instance(eb, io_context, propertyManager);
            instance.loadRules(ruleset);

            assert(Ruleset::hasInstance());

        }
        assert(!Ruleset::hasInstance());
    }

    {
        etc_directory = data_path + "/ruleset1/etc";
        Atlas::Objects::Factories factories;
        Inheritance inheritance(factories);
        EntityBuilder eb;
        {
            Ruleset instance(eb, io_context, propertyManager);
            instance.loadRules(ruleset);

            assert(Ruleset::hasInstance());

        }
        assert(!Ruleset::hasInstance());
    }

    {
        etc_directory = data_path + "/ruleset2/etc";
        Atlas::Objects::Factories factories;
        Inheritance inheritance(factories);
        EntityBuilder eb;
        {
            Ruleset instance(eb, io_context, propertyManager);
            instance.loadRules(ruleset);

            assert(Ruleset::hasInstance());

        }
        assert(!Ruleset::hasInstance());
    }

    {
        // Create a test world.

        Atlas::Objects::Factories factories;
        Inheritance inheritance(factories);
        // Instance of Ruleset with all protected methods exposed
        // for testing
        EntityBuilder eb;
        ExposedRuleset test_ruleset(eb, io_context, propertyManager);

        // Attributes for test entities being created
        Anonymous attributes;

        // Set up a simple class description for a new type, and install it.
        {
            Root custom_type_description;
            custom_type_description->setObjtype("class");
            custom_type_description->setId("f134c3e0");
            custom_type_description->setParent("thing");

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
            custom_type_description->setParent("task");

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
            custom_type_description->setParent("get");

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
            custom_type_description->setParent("thing");

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
            custom_type_description->setParent("");

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
            custom_type_description->setParent("thing");

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
            attrs["test_custom_type_attr"] = test_custom_type_attr;
            custom_type_description->setAttr("attributes", attrs);
            custom_type_description->setId("custom_type");
            custom_type_description->setParent("thing");

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
            attrs["test_custom_inherited_type_attr"] = test_custom_type_attr;
            custom_inherited_type_description->setAttr("attributes", attrs);
            custom_inherited_type_description->setId("custom_inherited_type");
            custom_inherited_type_description->setParent("custom_type");

            ret = test_ruleset.installRule("custom_inherited_type", "custom",
                                           custom_inherited_type_description);

            // Add this to inheritance, so future tests work
            Inheritance::instance().addChild(custom_inherited_type_description);
            assert(ret == 0);
        }

        // Make sure than attempting to modify a non-existent type fails
        {
            Anonymous nonexistant_description;
            nonexistant_description->setObjtype("class");
            MapType attrs;
            MapType test_custom_type_attr;

            test_custom_type_attr["default"] = "no_value";
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

            // No parent
            ret = test_ruleset.modifyRule("custom_inherited_type",
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

        // Modify the first custom type removing its custom attribute
        {
            Anonymous new_custom_type_description;
            new_custom_type_description->setObjtype("class");
            new_custom_type_description->setId("custom_type");
            new_custom_type_description->setAttr("attributes", MapType());
            new_custom_type_description->setParent("thing");

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
            attrs["test_custom_type_attr"] = test_custom_type_attr;

            MapType new_custom_type_attr;

            new_custom_type_attr["default"] = "new_value";
            attrs["new_custom_type_attr"] = new_custom_type_attr;

            new_custom_type_description->setId("custom_type");
            new_custom_type_description->setAttr("attributes", attrs);
            new_custom_type_description->setParent("thing");

            ret = test_ruleset.modifyRule("custom_type", new_custom_type_description);

            assert(ret == 0);
            
        }
    }
}

// stub functions

#include "server/EntityRuleHandler.h"
#include "server/OpRuleHandler.h"
#include "server/PropertyRuleHandler.h"
#include "server/ArchetypeRuleHandler.h"
#include "server/Persistence.h"
#include "server/EntityFactory.h"
#include "rules/simulation/CorePropertyManager.h"

#include "common/AtlasFileLoader.h"
#include "common/log.h"
#include "common/TypeNode.h"

#define STUB_OpRuleHandler_check
int OpRuleHandler::check(const Atlas::Objects::Root & desc)
{
    if (desc->getObjtype() != "op_definition") {
        return -1;
    }
    return 0;
}

#include "../stubs/server/stubOpRuleHandler.h"

#define STUB_PropertyRuleHandler_check
int PropertyRuleHandler::check(const Atlas::Objects::Root & desc)
{
    if (desc->getObjtype() != "type") {
        return -1;
    }
    return 0;
}

#include "../stubs/server/stubPropertyRuleHandler.h"

#define STUB_EntityRuleHandler_check
int EntityRuleHandler::check(const Atlas::Objects::Root & desc)
{
    if (desc->getObjtype() != "class") {
        return -1;
    }
    return 0;
}


#define STUB_ArchetypeRuleHandler_check
int ArchetypeRuleHandler::check(const Atlas::Objects::Root & desc)
{
    if (desc->getObjtype() != "archetype") {
        return -1;
    }
    return 0;
}

#include "../stubs/server/stubArchetypeRuleHandler.h"
#include "../stubs/server/stubEntityBuilder.h"
#include "../stubs/server/stubPersistence.h"
#include "../stubs/common/stubglobals.h"

AtlasFileLoader::AtlasFileLoader(const Atlas::Objects::Factories& factories, const std::string & filename,
                                 std::map<std::string, Root> & m) :
    ObjectsDecoder(factories),
    m_file(filename.c_str(), std::ios::in),
                m_count(0), m_messages(m)
{
}

AtlasFileLoader::~AtlasFileLoader()
{
}

void AtlasFileLoader::objectArrived(Root obj)
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


#define STUB_Inheritance_getClass
const Atlas::Objects::Root& Inheritance::getClass(const std::string & parent, Visibility visibility) const
{
    auto I = atlasObjects.find(parent);
    if (I == atlasObjects.end()) {
        return noClass;
    }
    return I->second->description(visibility);
}

#define STUB_Inheritance_addChild
TypeNode* Inheritance::addChild(const Atlas::Objects::Root & obj)
{
    const std::string & child = obj->getId();
    TypeNode * type = new TypeNode(child, obj);
    atlasObjects[child].reset(type);
    return type;
}

#include "../stubs/common/stubInheritance.h"
#include "../stubs/common/stubTypeNode.h"
#include "../stubs/common/stublog.h"
#include "../stubs/server/stubEntityKit.h"
#include "../stubs/server/stubEntityFactory.h"
#include "../stubs/server/stubEntityRuleHandler.h"
#include "../stubs/common/stubPropertyManager.h"
#include "../stubs/common/stubAssetsManager.h"

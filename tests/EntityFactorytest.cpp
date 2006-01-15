// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#include "TestWorld.h"

#include "rulesets/World.h"
#include "rulesets/Python_API.h"

#include "server/EntityFactory.h"
#include "server/PersistantThingFactory.h"

#include <Atlas/Objects/Anonymous.h>

#include <cassert>

using Atlas::Message::MapType;
using Atlas::Objects::Entity::Anonymous;

class ExposedEntityFactory : public EntityFactory {
  public:
    explicit ExposedEntityFactory(BaseWorld & w) : EntityFactory(w) { }

    void getRulesFromFiles(MapType & rules) {
        EntityFactory::getRulesFromFiles(rules);
    }
    void installRules() {
        EntityFactory::installRules();
    }
    void installFactory(const std::string & parent,
                        const std::string & clss, FactoryBase * factory) {
        EntityFactory::installFactory(parent, clss, factory);
    }
    void populateFactory(const std::string & className,
                         FactoryBase * factory,
                         const MapType & classDesc) {
        EntityFactory::populateFactory(className, factory, classDesc);
    }
    FactoryBase * getNewFactory(const std::string & clss) {
        return EntityFactory::getNewFactory(clss);
    }
    int installEntityClass(const std::string & className,
                           const std::string & parent,
                           const MapType & classDesc) {
        return EntityFactory::installEntityClass(className, parent, classDesc);
    }
    int installOpDefinition(const std::string & opDefName,
                            const std::string & parent,
                            const MapType & opDefDesc) {
        return EntityFactory::installOpDefinition(opDefName, parent, opDefDesc);
    }

    const FactoryDict & factoryDict() const { return m_factories; }

};

int main(int argc, char ** argv)
{
    loadConfig(argc, argv);

    init_python_api();

    int ret;

    {
        World e("1", 1);
        TestWorld test_world(e);
        Anonymous attributes;

        EntityFactory::init(test_world);

        assert(EntityFactory::instance() != 0);

        assert(EntityFactory::instance()->newEntity("1", 1, "world", attributes) == 0);
        assert(EntityFactory::instance()->newEntity("1", 1, "nonexistant", attributes) == 0);
        assert(EntityFactory::instance()->newEntity("1", 1, "thing", attributes) != 0);

        EntityFactory::del();
        assert(EntityFactory::instance() == 0);
    }

    {
        World e("1", 1);
        TestWorld test_world(e);
        Anonymous attributes;
        Atlas::Message::Element val;

        EntityFactory::init(test_world);

        assert(EntityFactory::instance() != 0);

        Entity * test_ent = EntityFactory::instance()->newEntity("1", 1, "thing", attributes);
        assert(test_ent != 0);
        assert(!test_ent->get("funky", val));
        assert(val.isNone());

        attributes->setAttr("funky", "true");

        test_ent = EntityFactory::instance()->newEntity("1", 1, "thing", attributes);
        assert(test_ent != 0);
        assert(test_ent->get("funky", val));
        assert(val.isString());
        assert(val.String() == "true");

        EntityFactory::del();
        assert(EntityFactory::instance() == 0);
    }

    {
        // Create a test world.
        World e("1", 1);
        TestWorld test_world(e);
        Atlas::Message::Element val;

        // Instance of EntityFactory with all protected methods exposed
        // for testing
        ExposedEntityFactory entity_factory(test_world);

        // Attributes for test entities being created
        Anonymous attributes;

        // Create an entity which is an instance of one of the core classes
        Entity * test_ent = entity_factory.newEntity("1", 1, "thing", attributes);
        assert(test_ent != 0);
        // Check the created entity does not have the attribute values we
        // will be testing later
        assert(!test_ent->get("funky", val));
        assert(val.isNone());

        // Set a test attribute
        attributes->setAttr("funky", "true");

        // Create another entity, and check that it has picked up the new
        // attribute value
        test_ent = entity_factory.newEntity("1", 1, "thing", attributes);
        assert(test_ent != 0);
        assert(test_ent->get("funky", val));
        assert(val.isString());
        assert(val.String() == "true");

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

        // Set up a type description for a new type, and install it.
        {
            MapType custom_type_description;
            MapType attrs;
            MapType test_custom_type_attr;
            test_custom_type_attr["default"] = "test_value";
            test_custom_type_attr["visibility"] = "public";
            attrs["test_custom_type_attr"] = test_custom_type_attr;
            custom_type_description["attributes"] = attrs;

            ret = entity_factory.installEntityClass("custom_type", "thing", custom_type_description);

            assert(ret == 0);
        }

        // Check that the factory dictionary now contains a factory for
        // the custom type we just installed.
        FactoryDict::const_iterator I = factory_dict.find("custom_type");
        assert(I != factory_dict.end());
        FactoryBase * custom_type_factory = I->second;

        // Check the factory has the attributes we described on the custom
        // type.
        MapType::const_iterator J = custom_type_factory->m_attributes.find("test_custom_type_attr");
        assert(J != custom_type_factory->m_attributes.end());
        assert(J->second.isString());
        assert(J->second.String() == "test_value");

        // Create an instance of our custom type, ensuring that it works.
        test_ent = entity_factory.newEntity("1", 1, "custom_type", attributes);
        assert(test_ent != 0);

        // Reset val.
        val = Atlas::Message::Element();
        assert(val.isNone());

        // Check the custom type has the attribute we passed in when creating
        // the instance.
        assert(test_ent->get("funky", val));
        assert(val.isString());
        assert(val.String() == "true");

        // Reset val.
        val = Atlas::Message::Element();
        assert(val.isNone());

        // Check the custom type has the attribute described when the
        // custom type was installed.
        assert(test_ent->get("test_custom_type_attr", val));
        assert(val.isString());
        assert(val.String() == "test_value");

        // Check that creating an entity of a type we know we have not yet
        // installed results in a null pointer.
        assert(entity_factory.newEntity("1", 1, "custom_inherited_type", attributes) == 0);

        // Assert the dictionary does not contain the factory we know we have
        // have not yet installed.
        assert(factory_dict.find("custom_inherited_type") == factory_dict.end());

        // Set up a type description for a second new type which inherits
        // from the first, and install it.
        {
            MapType custom_inherited_type_description;
            MapType attrs;
            MapType test_custom_type_attr;
            test_custom_type_attr["default"] = "test_inherited_value";
            test_custom_type_attr["visibility"] = "public";
            attrs["test_custom_inherited_type_attr"] = test_custom_type_attr;
            custom_inherited_type_description["attributes"] = attrs;

            ret = entity_factory.installEntityClass("custom_inherited_type", "custom_type", custom_inherited_type_description);

            assert(ret == 0);
        }

        // Check that the factory dictionary does contain the factory for
        // the second newly installed type
        I = factory_dict.find("custom_inherited_type");
        assert(I != factory_dict.end());
        FactoryBase * custom_inherited_type_factory = I->second;
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
        test_ent = entity_factory.newEntity("1", 1, "custom_inherited_type", attributes);
        assert(test_ent != 0);

        // Reset val.
        val = Atlas::Message::Element();
        assert(val.isNone());

        // Check the custom type has the attribute we passed in when creating
        // the instance.
        assert(test_ent->get("funky", val));
        assert(val.isString());
        assert(val.String() == "true");

        // Reset val.
        val = Atlas::Message::Element();
        assert(val.isNone());

        // Check the instance of the second custom type has the attribute
        // described when the first custom type was installed.
        assert(test_ent->get("test_custom_type_attr", val));
        assert(val.isString());
        assert(val.String() == "test_value");

        // Reset val.
        val = Atlas::Message::Element();
        assert(val.isNone());

        // Check the custom type has the attribute described when the
        // second custom type was installed
        assert(test_ent->get("test_custom_inherited_type_attr", val));
        assert(val.isString());
        assert(val.String() == "test_inherited_value");

        // FIXME TODO Modify a type, and ensure attribute propagate to inherited types.

        // Make sure than attempting to modify a non-existant type fails
        {
            MapType nonexistant_description;
            MapType attrs;
            MapType test_custom_type_attr;

            test_custom_type_attr["default"] = "no_value";
            test_custom_type_attr["visibility"] = "public";
            attrs["no_custom_type_attr"] = test_custom_type_attr;

            nonexistant_description["attributes"] = attrs;

            ret = entity_factory.modifyRule("nonexistant", nonexistant_description);

            assert(ret != 0);
        }

        // Modify the second custom type removing its custom attribute
        {
            MapType new_custom_inherited_type_description;
            new_custom_inherited_type_description["attributes"] = MapType();

            ret = entity_factory.modifyRule("custom_inherited_type", new_custom_inherited_type_description);

            assert(ret == 0);
        }

        // Check that the factory dictionary does contain the factory for
        // the second newly installed type
        I = factory_dict.find("custom_inherited_type");
        assert(I != factory_dict.end());
        custom_inherited_type_factory = I->second;
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
        test_ent = entity_factory.newEntity("1", 1, "custom_inherited_type", attributes);
        assert(test_ent != 0);

        // Reset val.
        val = Atlas::Message::Element();
        assert(val.isNone());

        // Make sure test nonexistant attribute isn't present
        assert(!test_ent->get("nonexistant", val));
        // Make sure nonexistant attribute isn't present
        assert(!test_ent->get("nonexistant_attribute", val));

        // Reset val.
        val = Atlas::Message::Element();
        assert(val.isNone());

        // Check the custom type has the attribute we passed in when creating
        // the instance.
        assert(test_ent->get("funky", val));
        assert(val.isString());
        assert(val.String() == "true");

        // Reset val.
        val = Atlas::Message::Element();
        assert(val.isNone());

        // Check the instance of the second custom type has the attribute
        // described when the first custom type was installed.
        assert(test_ent->get("test_custom_type_attr", val));
        assert(val.isString());
        assert(val.String() == "test_value");

        // Reset val.
        val = Atlas::Message::Element();
        assert(val.isNone());

        // Check the custom type has the attribute described when the
        // second custom type was installed
        assert(!test_ent->get("test_custom_inherited_type_attr", val));

        // Modify the first custom type removing its custom attribute
        {
            MapType new_custom_type_description;
            new_custom_type_description["attributes"] = MapType();

            ret = entity_factory.modifyRule("custom_type", new_custom_type_description);

            assert(ret == 0);
        }

        // Check that the factory dictionary now contains a factory for
        // the custom type we just installed.
        I = factory_dict.find("custom_type");
        assert(I != factory_dict.end());
        custom_type_factory = I->second;

        // Check the factory has the attributes we described on the custom
        // type.
        J = custom_type_factory->m_attributes.find("test_custom_type_attr");
        assert(J == custom_type_factory->m_attributes.end());

        // Create an instance of our custom type, ensuring that it works.
        test_ent = entity_factory.newEntity("1", 1, "custom_type", attributes);
        assert(test_ent != 0);

        // Reset val.
        val = Atlas::Message::Element();
        assert(val.isNone());

        // Check the custom type has the attribute we passed in when creating
        // the instance.
        assert(test_ent->get("funky", val));
        assert(val.isString());
        assert(val.String() == "true");

        // Reset val.
        val = Atlas::Message::Element();
        assert(val.isNone());

        // Check the custom type no longer has the custom attribute
        assert(!test_ent->get("test_custom_type_attr", val));

        // Check that the factory dictionary does contain the factory for
        // the second newly installed type
        I = factory_dict.find("custom_inherited_type");
        assert(I != factory_dict.end());
        custom_inherited_type_factory = I->second;
        assert(custom_inherited_type_factory != 0);

        // Check that the factory no longer has inherited the attributes
        // from the first custom type which we removed
        J = custom_inherited_type_factory->m_attributes.find("test_custom_type_attr");
        assert(J == custom_inherited_type_factory->m_attributes.end());

        // Check that the factory no longer has the attributes we removed
        J = custom_inherited_type_factory->m_attributes.find("test_custom_inherited_type_attr");
        assert(J == custom_inherited_type_factory->m_attributes.end());

        // Creat an instance of the second custom type, ensuring it works.
        test_ent = entity_factory.newEntity("1", 1, "custom_inherited_type", attributes);
        assert(test_ent != 0);

        // Reset val.
        val = Atlas::Message::Element();
        assert(val.isNone());

        // Make sure test nonexistant attribute isn't present
        assert(!test_ent->get("nonexistant", val));
        // Make sure nonexistant attribute isn't present
        assert(!test_ent->get("nonexistant_attribute", val));

        // Reset val.
        val = Atlas::Message::Element();
        assert(val.isNone());

        // Check the custom type has the attribute we passed in when creating
        // the instance.
        assert(test_ent->get("funky", val));
        assert(val.isString());
        assert(val.String() == "true");

        // Reset val.
        val = Atlas::Message::Element();
        assert(val.isNone());

        // Check the instance of the second custom type has the attribute
        // described when the first custom type was installed.
        assert(!test_ent->get("test_custom_type_attr", val));

        // Reset val.
        val = Atlas::Message::Element();
        assert(val.isNone());

        // Check the custom type has the attribute described when the
        // second custom type was installed
        assert(!test_ent->get("test_custom_inherited_type_attr", val));

        // Add more custom attributes to the first type
        {
            MapType new_custom_type_description;
            MapType attrs;
            MapType test_custom_type_attr;

            test_custom_type_attr["default"] = "test_value";
            test_custom_type_attr["visibility"] = "public";
            attrs["test_custom_type_attr"] = test_custom_type_attr;

            MapType new_custom_type_attr;

            new_custom_type_attr["default"] = "new_value";
            new_custom_type_attr["visibility"] = "public";
            attrs["new_custom_type_attr"] = new_custom_type_attr;

            new_custom_type_description["attributes"] = attrs;

            ret = entity_factory.modifyRule("custom_type", new_custom_type_description);

            assert(ret == 0);
            
        }

        // Check that the factory dictionary now contains a factory for
        // the custom type we just installed.
        I = factory_dict.find("custom_type");
        assert(I != factory_dict.end());
        custom_type_factory = I->second;

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
        test_ent = entity_factory.newEntity("1", 1, "custom_type", attributes);
        assert(test_ent != 0);

        // Reset val.
        val = Atlas::Message::Element();
        assert(val.isNone());

        // Check the custom type has the attribute we passed in when creating
        // the instance.
        assert(test_ent->get("funky", val));
        assert(val.isString());
        assert(val.String() == "true");

        // Reset val.
        val = Atlas::Message::Element();
        assert(val.isNone());

        // Check the custom type now has the custom attributes
        assert(test_ent->get("test_custom_type_attr", val));
        assert(val.isString());
        assert(val.String() == "test_value");

        assert(test_ent->get("new_custom_type_attr", val));
        assert(val.isString());
        assert(val.String() == "new_value");

        // Check that the factory dictionary does contain the factory for
        // the second newly installed type
        I = factory_dict.find("custom_inherited_type");
        assert(I != factory_dict.end());
        custom_inherited_type_factory = I->second;
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
        test_ent = entity_factory.newEntity("1", 1, "custom_inherited_type", attributes);
        assert(test_ent != 0);

        // Reset val.
        val = Atlas::Message::Element();
        assert(val.isNone());

        // Make sure test nonexistant attribute isn't present
        assert(!test_ent->get("nonexistant", val));
        // Make sure nonexistant attribute isn't present
        assert(!test_ent->get("nonexistant_attribute", val));

        // Reset val.
        val = Atlas::Message::Element();
        assert(val.isNone());

        // Check the custom type has the attribute we passed in when creating
        // the instance.
        assert(test_ent->get("funky", val));
        assert(val.isString());
        assert(val.String() == "true");

        // Reset val.
        val = Atlas::Message::Element();
        assert(val.isNone());

        // Check the instance of the second custom type has the attribute
        // described when the first custom type was installed.
        assert(test_ent->get("test_custom_type_attr", val));
        assert(val.isString());
        assert(val.String() == "test_value");

        assert(test_ent->get("new_custom_type_attr", val));
        assert(val.isString());
        assert(val.String() == "new_value");

        // Reset val.
        val = Atlas::Message::Element();
        assert(val.isNone());

        // Check the custom type no longer has the attribute described when the
        // second custom type was installed
        assert(!test_ent->get("test_custom_inherited_type_attr", val));

    }
}

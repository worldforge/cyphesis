// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#include "TestWorld.h"

#include "rulesets/World.h"

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

int main()
{
    {
        World e("1", 1);
        TestWorld test_world(e);
        Anonymous attributes;

        EntityFactory::init(test_world);

        assert(EntityFactory::instance() != 0);

        assert(EntityFactory::instance()->newEntity("1", 1, "world", attributes) == 0);
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
        World e("1", 1);
        TestWorld test_world(e);
        Anonymous attributes;
        Atlas::Message::Element val;

        ExposedEntityFactory entity_factory(test_world);

        Entity * test_ent = entity_factory.newEntity("1", 1, "thing", attributes);
        assert(test_ent != 0);
        assert(!test_ent->get("funky", val));
        assert(val.isNone());

        attributes->setAttr("funky", "true");

        test_ent = entity_factory.newEntity("1", 1, "thing", attributes);
        assert(test_ent != 0);
        assert(test_ent->get("funky", val));
        assert(val.isString());
        assert(val.String() == "true");

        assert(entity_factory.newEntity("1", 1, "custom_type", attributes) == 0);

        const FactoryDict & factory_dict = entity_factory.factoryDict();

        assert(!factory_dict.empty());

        assert(factory_dict.find("custom_type") == factory_dict.end());

        {
            MapType custom_type_description;
            MapType attrs;
            MapType test_custom_type_attr;
            test_custom_type_attr["default"] = "test_value";
            test_custom_type_attr["visibility"] = "public";
            attrs["test_custom_type_attr"] = test_custom_type_attr;
            custom_type_description["attributes"] = attrs;

            int ret = entity_factory.installEntityClass("custom_type", "thing", custom_type_description);

            assert(ret == 0);
        }

        FactoryDict::const_iterator I = factory_dict.find("custom_type");
        assert(I != factory_dict.end());
        FactoryBase * custom_type_factory = I->second;

        MapType::const_iterator J = custom_type_factory->m_attributes.find("test_custom_type_attr");
        assert(J != custom_type_factory->m_attributes.end());
        assert(J->second.isString());
        assert(J->second.String() == "test_value");

        test_ent = entity_factory.newEntity("1", 1, "custom_type", attributes);
        assert(test_ent != 0);

        val = Atlas::Message::Element();
        assert(val.isNone());

        assert(test_ent->get("funky", val));
        assert(val.isString());
        assert(val.String() == "true");

        val = Atlas::Message::Element();
        assert(val.isNone());

        assert(test_ent->get("test_custom_type_attr", val));
        assert(val.isString());
        assert(val.String() == "test_value");

        assert(entity_factory.newEntity("1", 1, "custom_inherited_type", attributes) == 0);
        assert(factory_dict.find("custom_inherited_type") == factory_dict.end());

        {
            MapType custom_inherited_type_description;
            MapType attrs;
            MapType test_custom_type_attr;
            test_custom_type_attr["default"] = "test_inherited_value";
            test_custom_type_attr["visibility"] = "public";
            attrs["test_custom_inherited_type_attr"] = test_custom_type_attr;
            custom_inherited_type_description["attributes"] = attrs;

            int ret = entity_factory.installEntityClass("custom_inherited_type", "custom_type", custom_inherited_type_description);

            assert(ret == 0);
        }

        I = factory_dict.find("custom_inherited_type");
        assert(I != factory_dict.end());
        FactoryBase * custom_inherited_type_factory = I->second;
        assert(custom_inherited_type_factory != 0);

        J = custom_inherited_type_factory->m_attributes.find("test_custom_type_attr");
        assert(J != custom_inherited_type_factory->m_attributes.end());
        assert(J->second.isString());
        assert(J->second.String() == "test_value");

        J = custom_inherited_type_factory->m_attributes.find("test_custom_inherited_type_attr");
        assert(J != custom_inherited_type_factory->m_attributes.end());
        assert(J->second.isString());
        assert(J->second.String() == "test_inherited_value");

        test_ent = entity_factory.newEntity("1", 1, "custom_inherited_type", attributes);
        assert(test_ent != 0);

        val = Atlas::Message::Element();
        assert(val.isNone());

        assert(test_ent->get("funky", val));
        assert(val.isString());
        assert(val.String() == "true");

        val = Atlas::Message::Element();
        assert(val.isNone());

        assert(test_ent->get("test_custom_type_attr", val));
        assert(val.isString());
        assert(val.String() == "test_value");

        val = Atlas::Message::Element();
        assert(val.isNone());

        assert(test_ent->get("test_custom_inherited_type_attr", val));
        assert(val.isString());
        assert(val.String() == "test_inherited_value");

        // FIXME TODO Modify a type, and ensure attribute propagate to inherited types.

    }
}

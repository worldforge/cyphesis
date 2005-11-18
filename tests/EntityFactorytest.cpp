// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#include "TestWorld.h"

#include "rulesets/World.h"

#include "server/EntityFactory.h"

#include <Atlas/Objects/Anonymous.h>

#include <cassert>

using Atlas::Message::MapType;
using Atlas::Objects::Entity::Anonymous;

class ExposedEntityFactory : public EntityFactory {
  public:
    explicit ExposedEntityFactory(BaseWorld & w) : EntityFactory(w) { }

    void getRulesFromFiles(Atlas::Message::MapType & rules) {
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
                         const Atlas::Message::MapType & classDesc) {
        EntityFactory::populateFactory(className, factory, classDesc);
    }
    FactoryBase * getNewFactory(const std::string & clss) {
        return EntityFactory::getNewFactory(clss);
    }
    int installEntityClass(const std::string & className,
                           const std::string & parent,
                           const Atlas::Message::MapType & classDesc) {
        return EntityFactory::installEntityClass(className, parent, classDesc);
    }
    int installOpDefinition(const std::string & opDefName,
                            const std::string & parent,
                            const Atlas::Message::MapType & opDefDesc) {
        return EntityFactory::installOpDefinition(opDefName, parent, opDefDesc);
    }

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
}

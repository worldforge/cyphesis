/*
 Copyright (C) 2020 Erik Ogenvik

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include "../TestBase.h"
#include "../TestWorld.h"

#include "rules/simulation/Entity.h"

#include "common/Property.h"
#include "common/PropertyManager.h"
#include "common/TypeNode.h"

#include <cstdlib>

#include <cassert>
#include <server/EntityBuilder.h>
#include <server/ArchetypeFactory.h>
#include <server/EntityFactory.h>
#include <rules/simulation/World.h>
#include <common/Inheritance.h>


#include <Atlas/Objects/Decoder.h>
#include <Atlas/Codecs/XML.h>
#include <common/Monitors.h>
#include "rules/AtlasProperties.h"
#include <server/ArchetypeRuleHandler.h>
#include <server/EntityRuleHandler.h>

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Message::ListType;

template
class EntityFactory<Entity>;

#include "server/EntityFactory_impl.h"
#include "../DatabaseNull.h"
#include "../TestPropertyManager.h"

Atlas::Objects::Factories factories;

Atlas::Objects::Root fromXml(const std::string& xml)
{
    struct ObjectDecoder : Atlas::Objects::ObjectsDecoder
    {
        bool m_check;
        Atlas::Objects::Root m_obj;

        void objectArrived(Atlas::Objects::Root obj) override
        {
            m_check = true;
            m_obj = obj;
        }


        explicit ObjectDecoder(const Atlas::Objects::Factories& factories) :
                ObjectsDecoder(factories),
                m_check(false)
        {
        }

        bool check() const
        {
            return m_check;
        }

        const Atlas::Objects::Root& get()
        {
            m_check = false;
            return m_obj;
        }
    };


    std::stringstream ss(xml);
    ObjectDecoder decoder(factories);

    Atlas::Codecs::XML codec(ss, ss, decoder);
    codec.poll();

    assert(decoder.check());

    return decoder.get();

}

struct Tested : public Cyphesis::TestBase
{

    struct TestContext
    {
        DatabaseNull database;
        Ref<World> world;
        Inheritance inheritance;
        TestWorld testWorld;

        TestPropertyManager propertyManager;
        EntityBuilder eb;
        EntityRuleHandler entityRuleHandler;
        ArchetypeRuleHandler archetypeRuleHandler;


        TestContext() :
                world(new World()),
                inheritance(factories),
                testWorld(world),
                eb(),
                entityRuleHandler(eb, propertyManager),
                archetypeRuleHandler(eb, propertyManager)
        {
            eb.installBaseFactory("thing", "game_entity", std::make_unique<EntityFactory<Thing>>());
            // Set up a type description for a new type, and install it
            auto type1Factory = std::make_unique<EntityFactory<Entity>>();
            auto thing1 = R"("
<atlas>
    <map>
        <string name="id">thing1</string>
        <string name="objtype">class</string>
        <string name="parent">thing</string>
        <map name="attributes">
            <map name="property1">
                <int name="default">10</int>
            </map>
            <map name="property2">
                <int name="default">20</int>
            </map>
            <map name="propertyList1">
                <list name="default">
                    <int>1</int>
                </list>
            </map>
        </map>
    </map>
</atlas>
")";

            std::string dependents;
            std::string reasons;
            std::map<const TypeNode*, TypeNode::PropertiesUpdate> changes;
            entityRuleHandler.install("thing1", "thing", fromXml(thing1), dependents, reasons, changes);
        }

        ~TestContext()
        {
            testWorld.shutdown();
        }
    };


    Tested()
    {
        ADD_TEST(Tested::test_createFromArchetypeWithChildren);
        ADD_TEST(Tested::test_createFromArchetypeWithModifiers);
        ADD_TEST(Tested::test_createFromArchetype);
    }

    void setup() override
    {

    }

    void teardown() override
    {

    }

    void test_createFromArchetypeWithChildren()
    {
        TestContext context;


        auto archetype1 = R"("
<atlas>
    <map>
        <string name="id">archetype1</string>
        <string name="objtype">archetype</string>
        <string name="parent">archetype</string>
        <list name="entities">
            <map>
                <string name="id">main</string>
                <string name="parent">thing1</string>
                <list name="contains">
                    <string>child1</string>
                    <string>child2</string>
                </list>
            </map>
            <map>
                <string name="id">child1</string>
                <string name="parent">thing1</string>
                <string name="name">first child</string>
            </map>
            <map>
                <string name="id">child2</string>
                <string name="parent">thing1</string>
                <string name="name">second child</string>
            </map>
        </list>
    </map>
</atlas>
")";

        std::string diagnose;
        std::map<const TypeNode*, TypeNode::PropertiesUpdate> changes;

        context.archetypeRuleHandler.install("archetype1", "archetype", fromXml(archetype1), diagnose, diagnose, changes);

        auto obj = fromXml(R"("
<atlas>
    <map>
        <string name="parent">archetype1</string>
    </map>
</atlas>
")");
        auto entityDef = Atlas::Objects::smart_dynamic_cast<Atlas::Objects::Entity::RootEntity>(obj);

        auto entity = context.eb.newEntity(1, "archetype1", entityDef);
        ASSERT_NOT_NULL(entity.get());
        ASSERT_EQUAL(2UL, entity->m_contains->size());
        Atlas::Message::Element element;
        (*entity->m_contains->begin())->getAttr("name", element);
        ASSERT_EQUAL(element, "first child");
        (*(++entity->m_contains->begin()))->getAttr("name", element);
        ASSERT_EQUAL(element, "second child");

        auto contains = *entity->m_contains;
        for (auto child : contains) {
            child->destroy();
        }
        entity->destroy();

    }

    void test_createFromArchetypeWithModifiers()
    {
        TestContext context;


        auto archetype1 = R"("
<atlas>
    <map>
        <string name="id">archetype1</string>
        <string name="objtype">archetype</string>
        <string name="parent">archetype</string>
        <list name="entities">
            <map>
                <string name="id">main</string>
                <string name="parent">thing1</string>
                <int name="property1!append">20</int>
            </map>
        </list>
    </map>
</atlas>
")";

        std::string diagnose;
        std::map<const TypeNode*, TypeNode::PropertiesUpdate> changes;

        context.archetypeRuleHandler.install("archetype1", "archetype", fromXml(archetype1), diagnose, diagnose, changes);

        auto obj = fromXml(R"("
<atlas>
    <map>
        <string name="parent">archetype1</string>
        <string name="name">foo</string>
        <int name="property2!append">30</int>
    </map>
</atlas>
")");
        auto entityDef = Atlas::Objects::smart_dynamic_cast<Atlas::Objects::Entity::RootEntity>(obj);

        auto entity = context.eb.newEntity(1, "archetype1", entityDef);
        ASSERT_NOT_NULL(entity.get());
        Atlas::Message::Element element;

        entity->getAttr("property1", element);
        ASSERT_EQUAL(element, 30);
        entity->getAttr("property2", element);
        ASSERT_EQUAL(element, 50);

        entity->destroy();
    }

    void test_createFromArchetype()
    {
        TestContext context;


        auto archetype1 = R"("
<atlas>
    <map>
        <string name="id">archetype1</string>
        <string name="objtype">archetype</string>
        <string name="parent">archetype</string>
        <list name="entities">
            <map>
                <string name="id">main</string>
                <string name="parent">thing1</string>
                <int name="property2">20</int>
                <list name="propertyList1">
                    <int>1</int>
                </list>
            </map>
        </list>
    </map>
</atlas>
")";

        std::string diagnose;
        std::map<const TypeNode*, TypeNode::PropertiesUpdate> changes;

        context.archetypeRuleHandler.install("archetype1", "archetype", fromXml(archetype1), diagnose, diagnose, changes);

        auto obj = fromXml(R"("
<atlas>
    <map>
        <string name="parent">archetype1</string>
        <string name="name">foo</string>
        <int name="property3">30</int>
    </map>
</atlas>
")");
        auto entityDef = Atlas::Objects::smart_dynamic_cast<Atlas::Objects::Entity::RootEntity>(obj);

        auto entity = context.eb.newEntity(1, "archetype1", entityDef);
        ASSERT_NOT_NULL(entity.get());
        Atlas::Message::Element element;
        entity->getAttr("name", element);
        ASSERT_EQUAL(element, "foo");

        entity->getAttr("property1", element);
        ASSERT_EQUAL(element, 10);

        entity->getAttr("property2", element);
        ASSERT_EQUAL(element, 20);

        entity->getAttr("property3", element);
        ASSERT_EQUAL(element, 30);
        entity->destroy();

    }

};


int main()
{
    Monitors m;
    Tested t;

    return t.run();
}

// stubs

#include "rules/AtlasProperties.h"
#include "rules/Script.h"

#include "common/id.h"

#include "../stubs/server/stubTeleportProperty.h"
#include "../stubs/server/stubExternalMindsManager.h"
#include "../stubs/server/stubExternalMindsConnection.h"

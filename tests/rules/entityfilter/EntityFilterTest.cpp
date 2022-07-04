#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include "../../TestBaseWithContext.h"

#include "rules/entityfilter/Filter.h"

#include "rules/entityfilter/Providers.h"

#include "rules/entityfilter/ParserDefinitions.h"

#include "rules/simulation/EntityProperty.h"
#include "rules/Domain.h"
#include "rules/AtlasProperties.h"
#include "rules/BBoxProperty.h"
#include "rules/simulation/BaseWorld.h"
#include "rules/simulation/Entity.h"

#include "common/Property.h"
#include "common/log.h"
#include "common/Inheritance.h"
#include "common/TypeNode.h"

#include <wfmath/point.h>
#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/Factories.h>

#include <cassert>
#include <rules/simulation/ModeDataProperty.h>

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Objects::Entity::Anonymous;

using namespace EntityFilter;

static std::map<std::string, std::unique_ptr<TypeNode>>* s_types;

struct TestEntity : Entity
{
    explicit TestEntity(RouterId id) : Entity(id)
    {
    }


    std::unique_ptr<Domain> m_domain;

    void destroy() override
    {
        m_parent = nullptr;
        Entity::destroy();
    }

    void test_setDomain(std::unique_ptr<Domain> domain)
    {
        m_domain = std::move(domain);
    }

    Domain* getDomain() override
    {
        return m_domain.get();
    }

    Domain* getDomain() const override
    {
        return m_domain.get();
    }
};

struct TestDomain : Domain
{
    explicit TestDomain(LocatedEntity& e) : Domain(e)
    {}


    bool isEntityVisibleFor(const LocatedEntity& observingEntity, const LocatedEntity& observedEntity) const override
    {
        return false;
    }

    void addEntity(LocatedEntity& entity) override
    {}

    /**
     * Removes a child entity from this domain. The child entity is guaranteed to be a direct child of the entity to which the domain belongs, and to have addEntity(...) being called earlier.
     *
     * @param entity A child entity.
     */
    void removeEntity(LocatedEntity& entity) override
    {}

    /**
     * Fills the supplied list with all entities in the domain that the supplied entity can currently observe.
     * @param observingEntity The entity that is observing.
     * @param entityList A list of entities.
     */
    void getVisibleEntitiesFor(const LocatedEntity& observingEntity, std::list<LocatedEntity*>& entityList) const override
    {}

    bool isEntityReachable(const LocatedEntity& reachingEntity, float reach, const LocatedEntity& queriedEntity, const WFMath::Point<3>& positionOnQueriedEntity) const override
    {
        //For tests we'll return false if reach is zero
        if (reach > 0.f) {
            return false;
        }
        //There's a special property "only_reachable_with_pos" which allows us to test that positions are used when entity_location is used.
        if (queriedEntity.hasAttr("only_reachable_with_pos")) {
            return positionOnQueriedEntity.isValid();
        }
        //Only allow reaching if parent or child
        return reachingEntity.m_parent == &queriedEntity || queriedEntity.m_parent == &reachingEntity;
    }

    boost::optional<std::function<void()>> observeCloseness(LocatedEntity& reacher, LocatedEntity& target, double reach, std::function<void()> callback) override
    {
        return boost::none;
    }

};

struct TestContext
{
    Atlas::Objects::Factories factories;
    Inheritance m_inheritance;
    std::map<std::string, std::unique_ptr<TypeNode>> types;

    ProviderFactory m_factory;

    //Entities for testing

    //Barrels
    Ref<TestEntity> m_b1;
    Ref<Entity> m_b2;
    Ref<Entity> m_b3;

    //Boulder
    Ref<Entity> m_bl1;

    Ref<Entity> m_ch1; //Character entity
    //Outfit for the character
    Ref<Entity> m_glovesEntity; //Gloves for the character entity's outfit
    Ref<Entity> m_bootsEntity;
    Ref<Entity> m_cloth; //Cloth for gloves' outfit
    Ref<Entity> m_leather;

    Ref<Entity> m_entityOnlyReachableWithPosition; //An entity which can only be reachable if position is specified.

    std::map<std::string, Ref<LocatedEntity>> m_entities;

    std::map<std::string, Atlas::Message::MapType> m_memory;

    TestContext() : m_inheritance(factories)
    {
        s_types = &types;
//Set up testing environment for Type/Soft properties
        m_b1 = new TestEntity(1);
        add_entity(m_b1);

        types["thing"] = std::make_unique<TypeNode>("thing");;

        types["barrel"] = std::make_unique<TypeNode>("barrel");;
        types["barrel"]->setParent(types["thing"].get());
        m_b1->setType(types["barrel"].get());
        m_b1->setProperty("mass", std::make_unique<SoftProperty>(30));
        m_b1->setProperty("burn_speed", std::make_unique<SoftProperty>(0.3));
        m_b1->setProperty("isVisible", std::make_unique<SoftProperty>(true));

        m_b2 = new TestEntity(2);
        add_entity(m_b2);
        m_b2->setProperty("mass", std::make_unique<SoftProperty>(20));
        m_b2->setProperty("burn_speed", std::make_unique<SoftProperty>(0.25));
        m_b2->setType(types["barrel"].get());
        m_b2->setProperty("isVisible", std::make_unique<SoftProperty>(false));

        m_b3 = new TestEntity(3);
        add_entity(m_b3);
        m_b3->setProperty("mass", std::make_unique<SoftProperty>(25));
        m_b3->setProperty("burn_speed", std::make_unique<SoftProperty>(0.25));
        m_b3->setType(types["barrel"].get());

        types["boulder"] = std::make_unique<TypeNode>("boulder");
        m_bl1 = new TestEntity(4);
        add_entity(m_bl1);
        m_bl1->setProperty("mass", std::make_unique<SoftProperty>(25));
        m_bl1->setType(types["boulder"].get());

        m_bl1->setProperty("float_list", std::make_unique<SoftProperty>(std::vector<Element>{25.0, 20.0}));

        m_bl1->setProperty("string_list", std::make_unique<SoftProperty>(std::vector<Element>{"foo", "bar"}));

// Create an entity-related memory map
        Atlas::Message::MapType entity_memory_map{{"disposition", 25}};

        m_memory.emplace("1", entity_memory_map);

//b1 contains bl1 which contains b3
        m_b1->m_contains = std::make_unique<LocatedEntitySet>();
        m_b1->m_contains->insert(m_bl1);
        m_bl1->m_parent = m_b1.get();
        m_b1->test_setDomain(std::make_unique<TestDomain>(*m_b1));

        m_bl1->m_contains = std::make_unique<LocatedEntitySet>();
        m_bl1->m_contains->insert(m_b3);
        m_b3->m_parent = m_bl1.get();


//Set up testing environment for Outfit property
        types["gloves"] = std::make_unique<TypeNode>("gloves");
        types["boots"] = std::make_unique<TypeNode>("boots");
        types["character"] = std::make_unique<TypeNode>("character");
        types["cloth"] = std::make_unique<TypeNode>("cloth");
        types["leather"] = std::make_unique<TypeNode>("leather");

        m_glovesEntity = new TestEntity(5);
        add_entity(m_glovesEntity);
        m_glovesEntity->setType(types["gloves"].get());
        m_glovesEntity->setProperty("color", std::make_unique<SoftProperty>("brown"));
        m_glovesEntity->setProperty("mass", std::make_unique<SoftProperty>(5));
        //Mark it with a "reach" so we can use it in the "can_reach ... with" tests
        auto reachProp = new Property<double>();
        reachProp->data() = 10.0f;
        m_glovesEntity->setProperty("reach", std::unique_ptr<PropertyBase>(reachProp));

        m_bootsEntity = new TestEntity(6);
        add_entity(m_bootsEntity);
        m_bootsEntity->setType(types["boots"].get());
        m_bootsEntity->setProperty("color", std::make_unique<SoftProperty>("black"));
        m_bootsEntity->setProperty("mass", std::make_unique<SoftProperty>(10));


        m_cloth = new TestEntity(8);
        add_entity(m_cloth);
        m_cloth->setType(types["cloth"].get());
        m_cloth->setProperty("color", std::make_unique<SoftProperty>("green"));

        m_leather = new TestEntity(9);
        add_entity(m_leather);
        m_leather->setType(types["leather"].get());
        m_leather->setProperty("color", std::make_unique<SoftProperty>("pink"));

        //The m_cloth entity is attached to the gloves by the "thumb" attachment
        {
            auto attachedProp = std::make_unique<SoftProperty>();
            attachedProp->data() = Atlas::Message::MapType{{"$eid", m_cloth->getId()}};
            m_glovesEntity->setProperty("attached_thumb", std::move(attachedProp));

            auto modeDataProp = std::make_unique<ModeDataProperty>();
            modeDataProp->setPlantedData({m_glovesEntity->getIntId()});
            m_cloth->setProperty(ModeDataProperty::property_name, std::move(modeDataProp));
            m_glovesEntity->makeContainer();
            m_glovesEntity->addChild(*m_cloth);
        }

        m_ch1 = new TestEntity(7);
        add_entity(m_ch1);
        m_ch1->setType(types["character"].get());
        m_ch1->makeContainer();
        m_ch1->addChild(*m_glovesEntity);

        //The m_glovesEntity entity is attached to the m_ch1 by the "hand_primary" attachment
        {
            auto attachedHandPrimaryProp = std::make_unique<SoftProperty>();
            attachedHandPrimaryProp->data() = Atlas::Message::MapType{{"$eid", m_glovesEntity->getId()}};
            m_ch1->setProperty("attached_hand_primary", std::move(attachedHandPrimaryProp));
        }

        {
            auto modeDataProp = std::make_unique<ModeDataProperty>();
            modeDataProp->setPlantedData({m_ch1->getIntId()});
            m_glovesEntity->setProperty(ModeDataProperty::property_name, std::move(modeDataProp));
        }

        auto bbox1 = std::make_unique<BBoxProperty>();
        bbox1->set((std::vector<Element>{-1, -3, -2, 1, 3, 2}));
        m_b1->setProperty("bbox", std::unique_ptr<PropertyBase>(bbox1->copy()));

        auto bbox2 = std::make_unique<BBoxProperty>();
        bbox2->set(std::vector<Element>{-3, -2, -1, 1, 3, 2});
        m_bl1->setProperty("bbox", std::unique_ptr<PropertyBase>(bbox2->copy()));

        m_cloth->setProperty("bbox", std::unique_ptr<PropertyBase>(bbox1->copy()));

        //The m_entityOnlyReachableWithPosition is a child of b1
        m_entityOnlyReachableWithPosition = new TestEntity(10);
        add_entity(m_entityOnlyReachableWithPosition);
        m_entityOnlyReachableWithPosition->setProperty("only_reachable_with_pos", std::make_unique<SoftProperty>(Element(1)));
        m_entityOnlyReachableWithPosition->setType(types["barrel"].get());
        m_b1->m_contains->emplace(m_entityOnlyReachableWithPosition);
        m_entityOnlyReachableWithPosition->m_parent = m_b1.get();
    }

    ~TestContext()
    {
        for (auto entry: m_entities) {
            entry.second->destroy();
        }
    };

    QueryContext makeContext(const Ref<Entity>& entity)
    {
        QueryContext queryContext{*entity};
        queryContext.entity_lookup_fn = [&](const std::string& id) { return find_entity(id); };
        queryContext.type_lookup_fn = [](const std::string& id) { return Inheritance::instance().getType(id); };
        queryContext.memory_lookup_fn = [&](const std::string& id) -> const Atlas::Message::MapType& {
            auto I = m_memory.find(id);
            if (I != m_memory.end()) {
                return I->second;
            }
            static Atlas::Message::MapType empty;
            return empty;
        };
        return queryContext;
    }

    Ref<LocatedEntity> find_entity(const std::string& id)
    {
        auto I = m_entities.find(id);
        if (I != m_entities.end()) {
            return I->second;
        }
        return {};
    }

    void add_entity(Ref<LocatedEntity> entity)
    {
        m_entities.emplace(entity->getId(), entity);
    }


};


struct Tested : public Cyphesis::TestBaseWithContext<TestContext>
{


    //\brief a tester function for entity filter. Accepts a query and lists of entities that
    // are supposed to pass or fail the test for a given query
    void TestQuery(TestContext& testContext,
                   const std::string& query,
                   std::initializer_list<Ref<Entity>> entitiesToPass,
                   std::initializer_list<Ref<Entity>> entitiesToFail)
    {
        TestQuery(testContext, query, entitiesToPass, entitiesToFail, EntityFilter::ProviderFactory());
    }

    void TestQuery(TestContext& testContext,
                   const std::string& query,
                   std::initializer_list<Ref<Entity>> entitiesToPass,
                   std::initializer_list<Ref<Entity>> entitiesToFail,
                   const EntityFilter::ProviderFactory& factory)
    {
        EntityFilter::Filter f(query, factory);
        for (const auto& entity : entitiesToPass) {
            QueryContext queryContext = testContext.makeContext(entity);
            assert(f.match(queryContext));
        }
        for (const auto& entity : entitiesToFail) {
            QueryContext queryContext = testContext.makeContext(entity);
            assert(!f.match(queryContext));
        }
    }

    void TestContextQuery(const std::string& query,
                          std::initializer_list<QueryContext> contextsToPass,
                          std::initializer_list<QueryContext> contextsToFail)
    {
        TestContextQuery(query, contextsToPass, contextsToFail, EntityFilter::ProviderFactory());
    }

    void TestContextQuery(const std::string& query,
                          std::initializer_list<QueryContext> contextsToPass,
                          std::initializer_list<QueryContext> contextsToFail,
                          const EntityFilter::ProviderFactory& factory)
    {
        EntityFilter::Filter f(query, factory);
        for (auto& context : contextsToPass) {
            assert(f.match(context));
        }
        for (auto& context : contextsToFail) {
            assert(!f.match(context));
        }
    }

    void test_literals(TestContext& context)
    {
//        TestQuery(context, "1=1", {context.m_b1}, {});
//        TestQuery(context, "true=true", {context.m_b1}, {});
//        TestQuery(context, "false=false", {context.m_b1}, {});
//        TestQuery(context, "false!=true", {context.m_b1}, {});
//        TestQuery(context, "'string'='string'", {context.m_b1}, {});
//        TestQuery(context, R"("string"='string')", {context.m_b1}, {});
//        TestQuery(context, R"("string"="string")", {context.m_b1}, {});
//        TestQuery(context, R"("string with space"="string with space")", {context.m_b1}, {});
//        TestQuery(context, R"("string with space"!="stringwithspace")", {context.m_b1}, {});
    }

    void test_describe(TestContext& context)
    {
        TestQuery(context, "describe('Should be barrel', entity.type=types.barrel)", {context.m_b1}, {context.m_bl1});
        TestQuery(context, "describe(\"Should be barrel\", entity.type=types.barrel)", {context.m_b1}, {context.m_bl1});
        TestQuery(context, "describe('Should burn.', entity.burn_speed != none)", {context.m_b1, context.m_b2}, {});
        TestQuery(context, "describe('Should burn.', entity.burn_speed != none and entity.burn_speed != none) and describe('Should burn.', entity.burn_speed != none)", {context.m_b1, context.m_b2},
                  {});

        {
            //Test a tricky filter we had some issues with.
            EntityFilter::Filter f(
                    "describe('Must be able to reach the bloomery.', actor can_reach tool) and describe('There must be charcoal or lumber in the bloomery.', contains(tool.contains, child.type=types.charcoal) or contains(tool.contains, child.type=types.lumber)) and describe('There must be hematite in the bloomery.', contains(tool.contains, child.type=types.hematite))",
                    EntityFilter::ProviderFactory());
        }
        {
            EntityFilter::Filter f("describe('Should burn.', entity.burn_speed != none)", EntityFilter::ProviderFactory());
            QueryContext queryContext = context.makeContext(context.m_bl1);
            std::vector<std::string> errors;
            queryContext.report_error_fn = [&](const std::string& error) { errors.push_back(error); };
            f.match(queryContext);
            ASSERT_FALSE(errors.empty());
            ASSERT_EQUAL("Should burn.", errors.front());
        }
    }

    void test_Memory(TestContext& context)
    {
        TestQuery(context, "memory.disposition = 25", {context.m_b1}, {context.m_bl1});
    }

    void test_Types(TestContext& context)
    {
        TestQuery(context, "entity.type=types.barrel", {context.m_b1}, {context.m_bl1});
        TestQuery(context, "entity instance_of types.barrel", {context.m_b1}, {context.m_bl1});
        TestQuery(context, "entity.type=types.does_not_exist", {}, {context.m_bl1, context.m_b1});
        TestQuery(context, "entity instance_of types.does_not_exist", {}, {context.m_bl1, context.m_b1});

    }

    void test_CanReach(TestContext& context)
    {
        //TestDomain prevents context.m_b1 from reaching context.m_b2
        {
            QueryContext c{*context.m_b1};
            c.actor = context.m_b2.get();
            TestContextQuery("actor can_reach entity", {}, {c});
            TestContextQuery("entity can_reach actor", {}, {c});
        }

        //TestDomain allows context.m_b1 from reaching context.m_bl1
        {
            QueryContext c{*context.m_b1};
            c.actor = context.m_bl1.get();
            TestContextQuery("actor can_reach entity", {c}, {});
            TestContextQuery("entity can_reach actor", {c}, {});
        }

        //Test "with". context.m_glovesEntity has "reach" set, and our TestDomain will return false for any "reach" values that aren't zero
        {
            QueryContext c{*context.m_b1};
            c.actor = context.m_bl1.get();
            c.tool = context.m_glovesEntity.get();
            TestContextQuery("actor can_reach entity with tool", {}, {c});
            TestContextQuery("entity can_reach actor with tool", {}, {c});
        }

        //context.m_bootsEntity has no "reach" property so should work
        {
            QueryContext c{*context.m_b1};
            c.actor = context.m_bl1.get();
            c.tool = context.m_bootsEntity.get();
            TestContextQuery("actor can_reach entity with tool", {c}, {});
            TestContextQuery("entity can_reach actor with tool", {c}, {});
        }

        //context.m_entityOnlyReachableWithPosition can only be reached if the position is sent along, which happens if we use "entity_location"
        {
            QueryContext c{*context.m_entityOnlyReachableWithPosition};
            WFMath::Point<3> pos(1, 1, 1);
            c.entityLoc.pos = &pos;
            c.actor = context.m_bl1.get();
            TestContextQuery("actor can_reach entity", {}, {c});
            TestContextQuery("actor can_reach entity_location", {c}, {});
        }
    }


    //General tests for soft properties. Also includes a few misc. tests
    void test_SoftProperty(TestContext& context)
    {
        TestQuery(context, "entity.burn_speed != none", {context.m_b1, context.m_b2}, {});
        TestQuery(context, "entity.non_existing = none", {context.m_b1, context.m_b2}, {});
        TestQuery(context, "entity.burn_speed=0.3", {context.m_b1}, {context.m_b2});

        TestQuery(context, "entity.burn_speed>0.3", {}, {context.m_b1, context.m_bl1});
        TestQuery(context, "entity.burn_speed>=0.3", {context.m_b1}, {context.m_bl1});

        TestQuery(context, "entity.burn_speed<0.3", {context.m_b2}, {context.m_b1});
        TestQuery(context, "entity.burn_speed<=0.3", {context.m_b2, context.m_b1}, {});

        //test bool values
        TestQuery(context, "entity.isVisible = True", {context.m_b1}, {context.m_b2});
        TestQuery(context, "entity.isVisible = true", {context.m_b1}, {context.m_b2});

        TestQuery(context, "entity.isVisible = False", {context.m_b2}, {context.m_b1});
        TestQuery(context, "entity.isVisible = false", {context.m_b2}, {context.m_b1});

        //test entity ID matching
        TestQuery(context, "entity.id=1", {context.m_b1}, {context.m_b2});

        //test list match using "includes" operator

        TestQuery(context, "entity.float_list includes 20.0", {context.m_bl1}, {});

        TestQuery(context, "entity.string_list includes 'foo'", {context.m_bl1}, {});

        TestQuery(context, "entity.float_list includes 95.0", {}, {context.m_bl1});

        //test list match using "in" operator
        TestQuery(context, "20.0 in entity.float_list", {context.m_bl1}, {});

        //test queries with [] list notation
        TestQuery(context, "25 in [25, 30]", {context.m_bl1}, {});

        TestQuery(context, "'foo' in ['bar']", {}, {context.m_bl1});

        TestQuery(context, "entity.mass in [25, 30]", {context.m_b1}, {context.m_b2});

        TestQuery(context, "entity.burn_speed in [0.30, 0.40, 0.10]", {context.m_b1}, {context.m_b2});

        //test query with several criteria

        TestQuery(context, "entity.type=types.barrel&&entity.burn_speed=0.3", {context.m_b1}, {
                context.m_b2, context.m_bl1});
        //Test with fudged syntax
        TestQuery(context, "entity.type = types.barrel && entity.burn_speed = 0.3", {context.m_b1},
                  {context.m_b2, context.m_bl1});
        TestQuery(context, "entity.type  =  types.barrel  &&  entity.burn_speed  =  0.3", {
                          context.m_b1},
                  {context.m_b2, context.m_bl1});

        TestQuery(context, "entity instance_of types.barrel", {context.m_b1}, {context.m_bl1});

        //Check inheritence
        TestQuery(context, "entity instance_of types.thing", {context.m_b1}, {context.m_bl1});

        //test query with spaces
        TestQuery(context, "  entity.type = types.barrel   ", {context.m_b1}, {context.m_bl1});

        TestQuery(context, "foobar", {}, {context.m_b1, context.m_bl1});

        //test invalid syntax
        try {
            TestQuery(context, "entity instance_of | types.foo", {}, {});
            assert(false);
        } catch (std::invalid_argument& e) {
        }
        try {
            TestQuery(context, "entity,type = types.barrel", {}, {});
            assert(false);
        } catch (std::invalid_argument& e) {
        }
    }

    //Test logical operators and precedence
    void test_LogicalOperators(TestContext& context)
    {
        //test operators "and", "or"
        TestQuery(context, "(entity.type=types.barrel and entity.burn_speed=0.3)", {context.m_b1},
                  {context.m_bl1});

        TestQuery(context, "entity instance_of types.barrel   or   entity.mass=25", {
                          context.m_b1, context.m_b2, context.m_bl1},
                  {});

        //test not operator
        TestQuery(context, "!entity.type = types.barrel", {context.m_bl1}, {context.m_b1, context.m_b2});

        TestQuery(context, "not entity.burn_speed = 0.3", {context.m_bl1}, {context.m_b1});

        //test multiple types for instance_of operator
        TestQuery(context, "entity instance_of types.barrel|types.boulder", {context.m_b1, context.m_bl1}, {});

        TestQuery(context, "entity.type=types.barrel|types.boulder|types.thing", {context.m_b1, context.m_bl1}, {});
        //test and operator without spaces
        try {
            TestQuery(context, "entity.type=types.barrelandentity.burn_speed=0.3", {}, {});
            assert(false);
        } catch (std::invalid_argument& e) {
        }

        //test logical operators and precedence

        TestQuery(context, "entity.type=types.barrel||entity.type=types.boulder", {context.m_b1,
                                                                                   context.m_bl1},
                  {});

        TestQuery(context,
                  "entity.type=types.boulder||entity.type=types.barrel&&entity.burn_speed=0.3",
                  {context.m_b1, context.m_bl1}, {});

        //Test not operator precedence. It should be applied to burn_speed comparison, and
        //not the whole expression
        TestQuery(context, "not entity.burn_speed = 0.3 && entity.type=types.barrel", {context.m_b2,
                                                                                       context.m_b3},
                  {context.m_b1, context.m_bl1});

    }

    //Test queries with parentheses
    void test_Parentheses(TestContext& context)
    {

        //test query with parenthesis
        TestQuery(context, "(entity.type=types.boulder)", {context.m_bl1}, {context.m_b1});

        TestQuery(context, "(entity.type=types.boulder)&&(entity.mass=25)", {context.m_bl1},
                  {context.m_b1});

        //test query with nested parentheses
        TestQuery(context,
                  "(entity.type=types.barrel&&(entity.mass=25||entity.mass=30)||entity.type=types.boulder)",
                  {context.m_b1, context.m_b3, context.m_bl1}, {context.m_b2});
        //Test with fudged syntax
        TestQuery(context,
                  "(entity.type = types.barrel && ( entity.mass = 25 || entity.mass = 30 ) || entity.type = types.boulder )",
                  {context.m_b1, context.m_b3, context.m_bl1}, {context.m_b2});

        TestQuery(context,
                  "(entity.type=types.barrel&&(entity.mass=25&&(entity.burn_speed=0.25||entity.mass=30))||entity.type=types.boulder)",
                  {context.m_bl1}, {context.m_b1});

        //override precedence rules with parentheses
        TestQuery(context,
                  "(entity.type=types.boulder||entity.type=types.barrel)&&entity.burn_speed=0.3",
                  {context.m_b1}, {context.m_bl1});
        TestQuery(context, "not (entity.burn_speed = 0.3 && entity.type=types.barrel)", {
                          context.m_bl1, context.m_b3, context.m_b2},
                  {context.m_b1});
    }

    //Test outfit queries
    void test_Outfit(TestContext& context)
    {
        //Test soft property of outfit
        TestQuery(context, "get_entity(entity.attached_hand_primary).color='brown'", {context.m_ch1}, {});
        //Test type of outfit
        TestQuery(context, "get_entity(entity.attached_hand_primary).type=types.gloves", {context.m_ch1}, {});
        //Test an entity without outfit
        TestQuery(context, "get_entity(entity.attached_hand_primary).type=types.gloves", {}, {context.m_bl1});
        //Test outfit that doesn't have the specified part
        TestQuery(context, "get_entity(entity.attached_chest_primary).color='red'", {}, {context.m_ch1});
        //Test outfit with another criterion
        TestQuery(context, "entity.type=types.character&&get_entity(entity.attached_hand_primary).color='brown'",
                  {context.m_ch1}, {context.m_b1});
        //Test nested outfit
        TestQuery(context, "get_entity(get_entity(entity.attached_hand_primary).attached_thumb).color='green'", {context.m_ch1}, {});

        TestQuery(context, "get_entity(get_entity(entity.attached_hand_primary).attached_thumb).type=types.cloth", {context.m_ch1}, {});

        TestQuery(context, "entity instance_of types.barrel|types.boulder|types.gloves",
                  {context.m_b1, context.m_bl1, context.m_glovesEntity}, {});

        {
            QueryContext queryContext{*context.m_ch1};
            queryContext.entity_lookup_fn = [&](const std::string& id) { return context.find_entity(id); };
            queryContext.type_lookup_fn = [](const std::string& id) { return Inheritance::instance().getType(id); };
            queryContext.tool = context.m_glovesEntity.get();
            TestContextQuery("get_entity(entity.attached_hand_primary) = tool", {queryContext}, {});
        }
        {
            QueryContext queryContext{*context.m_b1};
            queryContext.entity_lookup_fn = [&](const std::string& id) { return context.find_entity(id); };
            queryContext.type_lookup_fn = [](const std::string& id) { return Inheritance::instance().getType(id); };

            TestContextQuery("get_entity(entity.attached_hand_primary) = none", {queryContext}, {});
        }

        {
            QueryContext queryContext{*context.m_ch1};
            queryContext.entity_lookup_fn = [&](const std::string& id) { return context.find_entity(id); };
            queryContext.type_lookup_fn = [](const std::string& id) { return Inheritance::instance().getType(id); };

            TestContextQuery("get_entity(entity.attached_not_exists) = none", {queryContext}, {});
        }
    }

    //Test BBox queries (such as volume and other dimensions)
    void test_BBox(TestContext& context)
    {
        //Test BBox volume
        TestQuery(context, "entity.bbox.volume=48.0", {context.m_b1}, {context.m_bl1});
        //Test BBox height
        TestQuery(context, "entity.bbox.height=6.0", {context.m_b1}, {context.m_bl1});
        //Test BBox length
        TestQuery(context, "entity.bbox.depth=4.0", {context.m_b1}, {context.m_bl1});
        //Test BBox width
        TestQuery(context, "entity.bbox.width=2.0", {context.m_b1}, {context.m_bl1});
        //Test BBox area
        TestQuery(context, "entity.bbox.area=8.0", {context.m_b1}, {context.m_bl1});
        //Test BBox with another criterion
        TestQuery(context, "entity.type=types.barrel&&entity.bbox.height>0.0", {context.m_b1}, {
                context.m_b2, context.m_bl1});
        //Test BBox of an outfit
//    TestQuery(context, "entity.outfit.hands.outfit.thumb.bbox.volume=48.0", {context.m_ch1},
//              {});
    }

    //Test contains_recursive function
    void test_ContainsRecursive(TestContext& context)
    {
        //Test contains_recursive function
        TestQuery(context,
                  "contains_recursive(entity.contains, child.type=types.boulder)",
                  {context.m_b1}, {context.m_b2});

        TestQuery(context,
                  "contains_recursive(entity.contains, child.type=types.barrel)",
                  {context.m_b1, context.m_bl1}, {context.m_b2});

        TestQuery(context,
                  "contains_recursive(entity.contains, child.type=types.barrel or child.mass = 25) = true",
                  {context.m_b1, context.m_bl1}, {context.m_b2});


        TestContextQuery(
                "contains_recursive(entity.contains, child = tool)",
                {{*context.m_b1, nullptr, context.m_bl1.get()}}, {});

        //The cloth is a child of the gloves which is a child of the character. Should be found recursively.
        TestQuery(context,
                  "contains_recursive(entity.contains, child.type = types.cloth)",
                  {context.m_ch1}, {});
    }

    //Test contains function
    void test_Contains(TestContext& context)
    {
        //Test contains function
        TestQuery(context,
                  "contains(entity.contains, child.type=types.boulder)",
                  {context.m_b1}, {context.m_b2});

        TestQuery(context,
                  "contains(entity.contains, child.type=types.barrel)",
                  {context.m_b1, context.m_bl1}, {context.m_b2});

        TestQuery(context,
                  "contains(entity.contains, child.type=types.barrel or child.mass = 25) = true",
                  {context.m_b1, context.m_bl1}, {context.m_b2});


        TestContextQuery(
                "contains(entity.contains, child = tool)",
                {{*context.m_b1, nullptr, context.m_bl1.get()}}, {});

        //The cloth is a child of the gloves which is a child of the character. Should not be found since it's not recursive.
        TestQuery(context,
                  "contains(entity.contains, child.type = types.cloth)",
                  {}, {context.m_ch1});
    }

    Tested()
    {
        ADD_TEST(Tested::test_literals);
        ADD_TEST(Tested::test_describe);
        ADD_TEST(Tested::test_Memory);
        ADD_TEST(Tested::test_Types);
        ADD_TEST(Tested::test_CanReach);
        ADD_TEST(Tested::test_SoftProperty);
        ADD_TEST(Tested::test_LogicalOperators);
        ADD_TEST(Tested::test_Parentheses);
        ADD_TEST(Tested::test_Outfit);
        ADD_TEST(Tested::test_BBox);
        ADD_TEST(Tested::test_ContainsRecursive)
        ADD_TEST(Tested::test_Contains)

    }
};


int main()
{
    Tested t;

    return t.run();;

    //TODO: Move remaining old tests
// START of Soft property and general filtering tests
    /*{
     // test entity.attribute case with various operators

     // END of soft property and general tests

     {
     ProviderFactory factory;

     //self.type
     segments.clear();
     segments.push_back(ProviderFactory::Segment { "", "self" });
     segments.push_back(ProviderFactory::Segment { ".", "mass" });
     auto lhs_provider5 = factory.createProviders(segments);
     QueryContext qc { bl1 };
     qc.self_entity = &b1;

     lhs_provider5->value(value, qc);
     assert(value == Element(30));


     //entity.type.name
     segments.clear();
     segments.push_back(ProviderFactory::Segment { "", "entity" });
     segments.push_back(ProviderFactory::Segment { ".", "type" });
     segments.push_back(ProviderFactory::Segment { ".", "name" });
     auto lhs_provider6 = mind_factory.createProviders(segments);
     ComparePredicate compPred16(lhs_provider6,
     new FixedElementProvider("barrel"),
     ComparePredicate::Comparator::EQUALS);
     assert(compPred15.isMatch(QueryContext { b1, memory }));

     */

}


//Stubs

#include "../../stubs/common/stubVariable.h"
#include "../../stubs/common/stubMonitors.h"
#include "../../stubs/common/stubLink.h"
#include "../../stubs/rules/simulation/stubDomainProperty.h"
#include "../../stubs/rules/stubAtlasProperties.h"
#include "../../stubs/rules/simulation/stubDensityProperty.h"
#include "../../stubs/rules/stubScaleProperty.h"
#include "../../stubs/rules/simulation/stubModeProperty.h"


#include "../../stubs/common/stubcustom.h"
#include "../../stubs/common/stubRouter.h"

#include "../../stubs/rules/simulation/stubBaseWorld.h"
#include "../../stubs/rules/stubLocation.h"
#include "../../stubs/rules/stubDomain.h"

#define STUB_Inheritance_getType

const TypeNode* Inheritance::getType(const std::string& parent) const
{
    auto I = s_types->find(parent);
    if (I == s_types->end()) {
        return 0;
    }
    return I->second.get();
}

#include "../../stubs/common/stubInheritance.h"
#include "../../stubs/common/stublog.h"
#include "../../stubs/rules/stubModifier.h"
#include "../../stubs/rules/stubPhysicalProperties.h"

#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

//TODO: Check for unnecessary includes/links
#include "TestBase.h"

#include "rulesets/entityfilter/Filter.h"

#include "rulesets/entityfilter/Providers.h"

#include "rulesets/entityfilter/ParserDefinitions.h"

#include "rulesets/EntityProperty.h"
#include "rulesets/Domain.h"
#include "rulesets/AtlasProperties.h"
#include "rulesets/BBoxProperty.h"
#include "rulesets/PlantedOnProperty.h"
#include "rulesets/BaseWorld.h"
#include "rulesets/Entity.h"

#include "common/Property.h"
#include "common/log.h"
#include "common/Inheritance.h"
#include "common/TypeNode.h"

#include <wfmath/point.h>
#include <Atlas/Objects/Anonymous.h>

#include <cassert>

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Objects::Entity::Anonymous;

using namespace EntityFilter;

static std::map<std::string, TypeNode*> types;

struct TestEntity : Entity
{
    explicit TestEntity(const std::string& id, long intId) : Entity(id, intId)
    {

    }

    Domain* m_domain = nullptr;

    void test_setDomain(Domain* domain)
    {
        m_domain = domain;
    }

    Domain* getDomain() override
    {
        return m_domain;
    }

    Domain* getDomain() const override
    {
        return m_domain;
    }
};

struct TestDomain : Domain
{
    explicit TestDomain(LocatedEntity& e) : Domain(e)
    {}

    void tick(double t, OpVector& res) override
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
        //Only allow reaching if parent or child
        if (reachingEntity.m_location.m_parent == &queriedEntity || queriedEntity.m_location.m_parent == &reachingEntity) {
            return true;
        }
        return false;
    }
};

struct EntityFilterTest : public Cyphesis::TestBase
{

    ProviderFactory m_factory;

    //Entities for testing

    //Barrels
    Ref<TestEntity> m_b1;
    Ref<Entity> m_b2;
    Ref<Entity> m_b3;
    LocatedEntitySet* m_b1_container; //Container property for b1

    //Boulder
    Ref<Entity> m_bl1;
    LocatedEntitySet* m_bl1_container;

    Ref<Entity> m_ch1; //Character entity
    //Outfit for the character
    Ref<Entity> m_glovesEntity; //Gloves for the character entity's outfit
    Ref<Entity> m_bootsEntity;
    Ref<Entity> m_cloth; //Cloth for gloves' outfit
    Ref<Entity> m_leather;

    std::map<std::string, Ref<LocatedEntity>> m_entities;

    //Types for testing
    TypeNode* m_thingType;
    TypeNode* m_barrelType;
    TypeNode* m_boulderType;
    TypeNode* m_characterType;
    TypeNode* m_clothType;
    TypeNode* m_glovesType;
    TypeNode* m_bootsType;

    //\brief a tester function for entity filter. Accepts a query and lists of entities that
    // are supposed to pass or fail the test for a given query
    void TestQuery(const std::string& query,
                   std::initializer_list<Ref<Entity>> entitiesToPass,
                   std::initializer_list<Ref<Entity>> entitiesToFail);

    void TestContextQuery(const std::string& query,
                          std::initializer_list<QueryContext> contextsToPass,
                          std::initializer_list<QueryContext> contextsToFail);

    EntityFilterTest();

    ///\Initialize private variables for testing before each test.
    void setup() override;

    ///\Free allocated space after every test
    void teardown() override;

    void test_CanReach();

    //General tests for soft properties. Also includes a few misc. tests
    void test_SoftProperty();

    //Test logical operators and precedence
    void test_LogicalOperators();

    //Test queries with parentheses
    void test_Parentheses();

    //Test outfit queries
    void test_Outfit();

    //Test BBox queries (such as volume and other dimensions)
    void test_BBox();

    //Test contains_recursive function
    void test_ContainsRecursive();

    Inheritance* m_inheritance;

    Ref<LocatedEntity> find_entity(const std::string& id)
    {
        auto I = m_entities.find(id);
        if (I != m_entities.end()) {
            return I->second;
        }
        return nullptr;
    }

    void add_entity(Ref<LocatedEntity> entity)
    {
        m_entities.emplace(entity->getId(), entity);
    }
};

void EntityFilterTest::test_CanReach()
{
    //TestDomain prevents m_b1 from reaching m_b2
    {
        QueryContext c{*m_b1};
        c.actor = m_b2.get();
        TestContextQuery("actor can_reach entity", {}, {c});
        TestContextQuery("entity can_reach actor", {}, {c});
    }

    //TestDomain allows m_b1 from reaching m_bl1
    {
        QueryContext c{*m_b1};
        c.actor = m_bl1.get();
        TestContextQuery("actor can_reach entity", {c}, {});
        TestContextQuery("entity can_reach actor", {c}, {});
    }

    //Test "with". m_glovesEntity has "reach" set, and our TestDomain will return false for any "reach" values that aren't zero
    {
        QueryContext c{*m_b1};
        c.actor = m_bl1.get();
        c.tool = m_glovesEntity.get();
        TestContextQuery("actor can_reach entity with tool", {}, {c});
        TestContextQuery("entity can_reach actor with tool", {}, {c});
    }

    //m_bootsEntity has no "reach" property so should work
    {
        QueryContext c{*m_b1};
        c.actor = m_bl1.get();
        c.tool = m_bootsEntity.get();
        TestContextQuery("actor can_reach entity with tool", {c}, {});
        TestContextQuery("entity can_reach actor with tool", {c}, {});
    }
}


void EntityFilterTest::test_SoftProperty()
{
    TestQuery("entity.burn_speed != none", {m_b1, m_b2}, {});
    TestQuery("entity.non_existing = none", {m_b1, m_b2}, {});
    TestQuery("entity.burn_speed=0.3", {m_b1}, {m_b2});

    TestQuery("entity.burn_speed>0.3", {}, {m_b1, m_bl1});
    TestQuery("entity.burn_speed>=0.3", {m_b1}, {m_bl1});

    TestQuery("entity.burn_speed<0.3", {m_b2}, {m_b1});
    TestQuery("entity.burn_speed<=0.3", {m_b2, m_b1}, {});

    //test bool values
    TestQuery("entity.isVisible = True", {m_b1}, {m_b2});
    TestQuery("entity.isVisible = true", {m_b1}, {m_b2});

    TestQuery("entity.isVisible = False", {m_b2}, {m_b1});
    TestQuery("entity.isVisible = false", {m_b2}, {m_b1});

    //test entity ID matching
    TestQuery("entity.id=1", {m_b1}, {m_b2});

    //test list match using "contains" operator

    TestQuery("entity.float_list contains 20.0", {m_bl1}, {});

    TestQuery("entity.string_list contains 'foo'", {m_bl1}, {});

    TestQuery("entity.float_list contains 95.0", {}, {m_bl1});

    //test list match using "in" operator
    TestQuery("20.0 in entity.float_list", {m_bl1}, {});

    //test queries with [] list notation
    TestQuery("25 in [25, 30]", {m_bl1}, {});

    TestQuery("'foo' in ['bar']", {}, {m_bl1});

    TestQuery("entity.mass in [25, 30]", {m_b1}, {m_b2});

    TestQuery("entity.burn_speed in [0.30, 0.40, 0.10]", {m_b1}, {m_b2});

    //test query with several criteria

    TestQuery("entity.type=types.barrel&&entity.burn_speed=0.3", {m_b1}, {
        m_b2, m_bl1});
    //Test with fudged syntax
    TestQuery("entity.type = types.barrel && entity.burn_speed = 0.3", {m_b1},
              {m_b2, m_bl1});
    TestQuery("entity.type  =  types.barrel  &&  entity.burn_speed  =  0.3", {
                  m_b1},
              {m_b2, m_bl1});

    TestQuery("entity instance_of types.barrel", {m_b1}, {m_bl1});

    //Check inheritence
    TestQuery("entity instance_of types.thing", {m_b1}, {m_bl1});

    //test query with spaces
    TestQuery("  entity.type = types.barrel   ", {m_b1}, {m_bl1});

    TestQuery("foobar", {}, {m_b1, m_bl1});

    //test a non-existing type
    try {
        TestQuery("entity instance_of types.foo", {}, {});
        assert(false);
    } catch (std::invalid_argument& e) {
    }

    //test invalid syntax
    try {
        TestQuery("entity instance_of | types.foo", {}, {});
        assert(false);
    } catch (std::invalid_argument& e) {
    }
    try {
        TestQuery("entity,type = types.barrel", {}, {});
        assert(false);
    } catch (std::invalid_argument& e) {
    }
}

void EntityFilterTest::test_LogicalOperators()
{
    //test operators "and", "or"
    TestQuery("(entity.type=types.barrel and entity.burn_speed=0.3)", {m_b1},
              {m_bl1});

    TestQuery("entity instance_of types.barrel   or   entity.mass=25", {
                  m_b1, m_b2, m_bl1},
              {});

    //test not operator
    TestQuery("!entity.type = types.barrel", {m_bl1}, {m_b1, m_b2});

    TestQuery("not entity.burn_speed = 0.3", {m_bl1}, {m_b1});

    //test multiple types for instance_of operator
    TestQuery("entity instance_of types.barrel|types.boulder", {m_b1,
                                                                m_bl1},
              {});

    //test and operator without spaces
    try {
        TestQuery("entity.type=types.barrelandentity.burn_speed=0.3", {}, {});
        assert(false);
    } catch (std::invalid_argument& e) {
    }

    //test logical operators and precedence

    TestQuery("entity.type=types.barrel||entity.type=types.boulder", {m_b1,
                                                                      m_bl1},
              {});

    TestQuery(
        "entity.type=types.boulder||entity.type=types.barrel&&entity.burn_speed=0.3",
        {m_b1, m_bl1}, {});

    //Test not operator precedence. It should be applied to burn_speed comparison, and
    //not the whole expression
    TestQuery("not entity.burn_speed = 0.3 && entity.type=types.barrel", {m_b2,
                                                                          m_b3},
              {m_b1, m_bl1});

}

void EntityFilterTest::test_Parentheses()
{

    //test query with parenthesis
    TestQuery("(entity.type=types.boulder)", {m_bl1}, {m_b1});

    TestQuery("(entity.type=types.boulder)&&(entity.mass=25)", {m_bl1},
              {m_b1});

    //test query with nested parentheses
    TestQuery(
        "(entity.type=types.barrel&&(entity.mass=25||entity.mass=30)||entity.type=types.boulder)",
        {m_b1, m_b3, m_bl1}, {m_b2});
    //Test with fudged syntax
    TestQuery(
        "(entity.type = types.barrel && ( entity.mass = 25 || entity.mass = 30 ) || entity.type = types.boulder )",
        {m_b1, m_b3, m_bl1}, {m_b2});

    TestQuery(
        "(entity.type=types.barrel&&(entity.mass=25&&(entity.burn_speed=0.25||entity.mass=30))||entity.type=types.boulder)",
        {m_bl1}, {m_b1});

    //override precedence rules with parentheses
    TestQuery(
        "(entity.type=types.boulder||entity.type=types.barrel)&&entity.burn_speed=0.3",
        {m_b1}, {m_bl1});
    TestQuery("not (entity.burn_speed = 0.3 && entity.type=types.barrel)", {
                  m_bl1, m_b3, m_b2},
              {m_b1});
}

void EntityFilterTest::test_Outfit()
{
    //Test soft property of outfit
    TestQuery("get_entity(entity.attached_hand_primary).color='brown'", {m_ch1}, {});
    //Test type of outfit
    TestQuery("get_entity(entity.attached_hand_primary).type=types.gloves", {m_ch1}, {});
    //Test an entity without outfit
    TestQuery("get_entity(entity.attached_hand_primary).type=types.gloves", {}, {m_bl1});
    //Test outfit that doesn't have the specified part
    TestQuery("get_entity(entity.attached_chest_primary).color='red'", {}, {m_ch1});
    //Test outfit with another criterion
    TestQuery("entity.type=types.character&&get_entity(entity.attached_hand_primary).color='brown'",
              {m_ch1}, {m_b1});
    //Test nested outfit
    TestQuery("get_entity(get_entity(entity.attached_hand_primary).attached_thumb).color='green'", {m_ch1}, {});

    TestQuery("get_entity(get_entity(entity.attached_hand_primary).attached_thumb).type=types.cloth", {m_ch1}, {});

    TestQuery("entity instance_of types.barrel|types.boulder|types.gloves",
              {m_b1, m_bl1, m_glovesEntity}, {});

    {
        QueryContext queryContext{*m_ch1};
        queryContext.entity_lookup_fn = [&](const std::string& id) { return find_entity(id); };
        queryContext.tool = m_glovesEntity.get();
        TestContextQuery("get_entity(entity.attached_hand_primary) = tool", {queryContext}, {});
    }
    {
        QueryContext queryContext{*m_b1};
        queryContext.entity_lookup_fn = [&](const std::string& id) { return find_entity(id); };

        TestContextQuery("get_entity(entity.attached_hand_primary) = none", {queryContext}, {});
    }

    {
        QueryContext queryContext{*m_ch1};
        queryContext.entity_lookup_fn = [&](const std::string& id) { return find_entity(id); };

        TestContextQuery("get_entity(entity.attached_not_exists) = none", {queryContext}, {});
    }
}

void EntityFilterTest::test_BBox()
{
    //Test BBox volume
    TestQuery("entity.bbox.volume=48.0", {m_b1}, {m_bl1});
    //Test BBox height
    TestQuery("entity.bbox.height=6.0", {m_b1}, {m_bl1});
    //Test BBox length
    TestQuery("entity.bbox.depth=4.0", {m_b1}, {m_bl1});
    //Test BBox width
    TestQuery("entity.bbox.width=2.0", {m_b1}, {m_bl1});
    //Test BBox area
    TestQuery("entity.bbox.area=8.0", {m_b1}, {m_bl1});
    //Test BBox with another criterion
    TestQuery("entity.type=types.barrel&&entity.bbox.height>0.0", {m_b1}, {
        m_b2, m_bl1});
    //Test BBox of an outfit
//    TestQuery("entity.outfit.hands.outfit.thumb.bbox.volume=48.0", {m_ch1},
//              {});
}

void EntityFilterTest::test_ContainsRecursive()
{
    //Test contains_recursive function
    TestQuery(
        "contains_recursive(entity.contains, child.type=types.boulder)",
        {m_b1}, {m_b2});

    TestQuery(
        "contains_recursive(entity.contains, child.type=types.barrel)",
        {m_b1, m_bl1}, {m_b2});

    TestQuery(
        "contains_recursive(entity.contains, child.type=types.barrel or child.mass = 25) = true",
        {m_b1, m_bl1}, {m_b2});


    TestContextQuery(
        "contains_recursive(entity.contains, child = tool)",
        {{*m_b1, nullptr, m_bl1.get()}}, {});
}

void EntityFilterTest::setup()
{
    m_inheritance = new Inheritance();
//Set up testing environment for Type/Soft properties
    m_b1 = new TestEntity("1", 1);
    add_entity(m_b1);

    m_thingType = new TypeNode("thing");
    types["thing"] = m_thingType;

    m_barrelType = new TypeNode("barrel");
    m_barrelType->setParent(m_thingType);
    types["barrel"] = m_barrelType;
    m_b1->setType(m_barrelType);
    m_b1->setProperty("mass", new SoftProperty(Element(30)));
    m_b1->setProperty("burn_speed", new SoftProperty(Element(0.3)));
    m_b1->setProperty("isVisible", new SoftProperty(Element(true)));

    m_b2 = new Entity("2", 2);
    add_entity(m_b2);
    m_b2->setProperty("mass", new SoftProperty(Element(20)));
    m_b2->setProperty("burn_speed", new SoftProperty(0.25));
    m_b2->setType(m_barrelType);
    m_b2->setProperty("isVisible", new SoftProperty(Element(false)));

    m_b3 = new Entity("3", 3);
    add_entity(m_b3);
    m_b3->setProperty("mass", new SoftProperty(Element(25)));
    m_b3->setProperty("burn_speed", new SoftProperty(Element(0.25)));
    m_b3->setType(m_barrelType);

    m_boulderType = new TypeNode("boulder");
    types["boulder"] = m_boulderType;
    m_bl1 = new Entity("4", 4);
    add_entity(m_bl1);
    m_bl1->setProperty("mass", new SoftProperty(Element(25)));
    m_bl1->setType(m_boulderType);

    SoftProperty* prop1 = new SoftProperty();
    prop1->set(std::vector<Element>{25.0, 20.0});
    m_bl1->setProperty("float_list", prop1);

    SoftProperty* list_prop2 = new SoftProperty();
    list_prop2->set(std::vector<Element>{"foo", "bar"});
    m_bl1->setProperty("string_list", list_prop2);

// Create an entity-related memory map
    std::map<std::string, Element> entity_memory_map;
    entity_memory_map.insert(std::make_pair("disposition", Element(25)));
    Element memory_map_element(entity_memory_map);

    std::map<std::string, Element> memory;
    memory.insert(std::make_pair("1", memory_map_element));

//b1 contains bl1 which contains b3
    m_b1_container = new LocatedEntitySet;
    m_b1_container->insert(m_bl1);
    m_bl1->m_location.m_parent = m_b1;
    m_b1->m_contains = m_b1_container;
    m_b1->test_setDomain(new TestDomain(*m_b1));

    m_bl1_container = new LocatedEntitySet;
    m_bl1_container->insert(m_b3);
    m_b3->m_location.m_parent = m_bl1;

    m_bl1->m_contains = m_bl1_container;

//Set up testing environment for Outfit property
    m_glovesType = new TypeNode("gloves");
    types["gloves"] = m_glovesType;
    m_bootsType = new TypeNode("boots");
    m_characterType = new TypeNode("character");
    types["character"] = m_characterType;
    TypeNode* m_clothType = new TypeNode("cloth");
    types["cloth"] = m_clothType;
    TypeNode* m_leatherType = new TypeNode("leather");

    m_glovesEntity = new Entity("5", 5);
    add_entity(m_glovesEntity);
    m_glovesEntity->setType(m_glovesType);
    m_glovesEntity->setProperty("color", new SoftProperty("brown"));
    m_glovesEntity->setProperty("mass", new SoftProperty(5));
    //Mark it with a "reach" so we can use it in the "can_reach ... with" tests
    auto reachProp = new Property<double>();
    reachProp->data() = 10.0f;
    m_glovesEntity->setProperty("reach", reachProp);

    m_bootsEntity = new Entity("6", 6);
    add_entity(m_bootsEntity);
    m_bootsEntity->setType(m_bootsType);
    m_bootsEntity->setProperty("color", new SoftProperty("black"));
    m_bootsEntity->setProperty("mass", new SoftProperty(10));


    m_cloth = new Entity("8", 8);
    add_entity(m_cloth);
    m_cloth->setType(m_clothType);
    m_cloth->setProperty("color", new SoftProperty("green"));

    m_leather = new Entity("9", 9);
    add_entity(m_leather);
    m_leather->setType(m_leatherType);
    m_leather->setProperty("color", new SoftProperty("pink"));

    //The m_cloth entity is attached to the gloves by the "thumb" attachment
    {
        auto attachedProp = new SoftProperty();
        attachedProp->data() = Atlas::Message::MapType{{"$eid", m_cloth->getId()}};
        m_glovesEntity->setProperty("attached_thumb", attachedProp);

        auto plantedOnProp = new PlantedOnProperty();
        plantedOnProp->data().entity = WeakEntityRef(m_glovesEntity.get());
        m_cloth->setProperty("planted_on", plantedOnProp);
    }

    m_ch1 = new Entity("7", 7);
    add_entity(m_ch1);
    m_ch1->setType(m_characterType);

    //The m_glovesEntity entity is attached to the m_ch1 by the "hand_primary" attachment
    {
        auto attachedHandPrimaryProp = new SoftProperty();
        attachedHandPrimaryProp->data() = Atlas::Message::MapType{{"$eid", m_glovesEntity->getId()}};
        m_ch1->setProperty("attached_hand_primary", attachedHandPrimaryProp);
    }

    {
        auto plantedOnProp = new PlantedOnProperty();
        plantedOnProp->data().entity = WeakEntityRef(m_ch1.get());
        m_glovesEntity->setProperty("planted_on", plantedOnProp);
    }

    BBoxProperty* bbox1 = new BBoxProperty;
    bbox1->set((std::vector<Element>{-1, -3, -2, 1, 3, 2}));
    m_b1->setProperty("bbox", bbox1);

    BBoxProperty* bbox2 = new BBoxProperty;
    bbox2->set(std::vector<Element>{-3, -2, -1, 1, 3, 2});
    m_bl1->setProperty("bbox", bbox2);

    m_cloth->setProperty("bbox", bbox1->copy());
}

EntityFilterTest::EntityFilterTest()
{
    ADD_TEST(EntityFilterTest::test_CanReach);
    ADD_TEST(EntityFilterTest::test_SoftProperty);
    ADD_TEST(EntityFilterTest::test_LogicalOperators);
    ADD_TEST(EntityFilterTest::test_Parentheses);
    ADD_TEST(EntityFilterTest::test_Outfit);
    ADD_TEST(EntityFilterTest::test_BBox);
    ADD_TEST(EntityFilterTest::test_ContainsRecursive);
}

int main()
{
    EntityFilterTest t;

    return t.run();

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

     //MindProviderFactory tests
     MindProviderFactory mind_factory;

     //memory.disposition
     segments.clear();
     segments.push_back( { "", "memory" });
     segments.push_back( { ".", "disposition" });
     provider = mind_factory.createProviders(segments);
     provider->value(value, QueryContext { b1, memory });
     assert(value.Int() == 25);

     //entity.memory
     segments.clear();
     segments.push_back( { "", "memory" });
     segments.push_back( { ".", "disposition" });
     lhs_provider5 = mind_factory.createProviders(segments);

     ComparePredicate compPred15(lhs_provider5, new FixedElementProvider(25),
     ComparePredicate::Comparator::EQUALS);
     assert(compPred15.isMatch(QueryContext { b1, memory }));

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

void EntityFilterTest::TestQuery(const std::string& query,
                                 std::initializer_list<Ref<Entity>> entitiesToPass,
                                 std::initializer_list<Ref<Entity>> entitiesToFail)
{
    EntityFilter::ProviderFactory factory;
    EntityFilter::Filter f(query, factory);
    for (const auto& entity : entitiesToPass) {
        QueryContext queryContext{*entity};
        queryContext.entity_lookup_fn = [&](const std::string& id) { return find_entity(id); };
        assert(f.match(queryContext));
    }
    for (const auto& entity : entitiesToFail) {
        QueryContext queryContext{*entity};
        queryContext.entity_lookup_fn = [&](const std::string& id) { return find_entity(id); };
        assert(!f.match(queryContext));
    }
}

void EntityFilterTest::TestContextQuery(const std::string& query,
                                        std::initializer_list<QueryContext> contextsToPass,
                                        std::initializer_list<QueryContext> contextsToFail)
{
    EntityFilter::ProviderFactory factory;
    EntityFilter::Filter f(query, factory);
    for (auto& context : contextsToPass) {
        assert(f.match(context));
    }
    for (auto& context : contextsToFail) {
        assert(!f.match(context));
    }
}

void EntityFilterTest::teardown()
{
    m_entities.clear();

    delete m_inheritance;
//    Clean up

    delete m_barrelType;
    delete m_boulderType;
    delete m_glovesType;
    delete m_bootsType;
    delete m_characterType;

}

//Stubs

#include "stubs/common/stubVariable.h"
#include "stubs/common/stubMonitors.h"
#include "stubs/common/stubLink.h"
#include "stubs/rulesets/stubDomainProperty.h"
#include "stubs/rulesets/stubIdProperty.h"
#include "stubs/rulesets/stubDensityProperty.h"
#include "stubs/rulesets/stubScaleProperty.h"

ContainsProperty::ContainsProperty(LocatedEntitySet& data) :
    PropertyBase(per_ephem), m_data(data)
{
}

int ContainsProperty::get(Atlas::Message::Element& e) const
{
    return 0;
}

void ContainsProperty::set(const Atlas::Message::Element& e)
{
}

void ContainsProperty::add(const std::string& s,
                           const Atlas::Objects::Entity::RootEntity& ent) const
{
}

ContainsProperty* ContainsProperty::copy() const
{
    return 0;
}


#include "stubs/common/stubCustom.h"
#include "stubs/common/stubRouter.h"

#include "stubs/rulesets/stubBaseWorld.h"
#include "stubs/rulesets/stubLocation.h"
#include "stubs/rulesets/stubDomain.h"

#define STUB_Inheritance_getType

const TypeNode* Inheritance::getType(const std::string& parent)
{
    auto I = types.find(parent);
    if (I == types.end()) {
        return 0;
    }
    return I->second;
}

#include "stubs/common/stubInheritance.h"

void log(LogLevel lvl, const std::string& msg)
{
}


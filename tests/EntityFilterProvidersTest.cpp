#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include "TestBase.h"

#include "rulesets/entityfilter/Filter.h"
#include "rulesets/entityfilter/ParserDefinitions.h"
#include "rulesets/entityfilter/Providers.h"

#include "rulesets/EntityProperty.h"
#include "rulesets/Domain.h"
#include "rulesets/AtlasProperties.h"
#include "rulesets/BBoxProperty.h"
#include "common/Property.h"
#include "rulesets/BaseWorld.h"
#include "common/log.h"
#include "common/Inheritance.h"

#include "rulesets/Entity.h"
#include "common/TypeNode.h"

#include <wfmath/point.h>
#include <Atlas/Objects/Anonymous.h>

#include <cassert>
#include <rulesets/PlantedOnProperty.h>

using namespace EntityFilter;
using Atlas::Message::Element;

static std::map<std::string, TypeNode*> types;

class ProvidersTest : public Cyphesis::TestBase {
    private:
        ProviderFactory m_factory;

        //Entities for testing
        Ref<Entity> m_b1;
        LocatedEntitySet *m_b1_container; //Container property for b1
        Ref<Entity> m_b2;
        LocatedEntitySet *m_b2_container; //Container for b2

        Ref<Entity> m_ch1; //Character type entity
        Ref<Entity> m_glovesEntity; //Gloves for the character entity's outfit
        Ref<Entity> m_cloth; //Cloth for gloves' outfit

        //Types for testing
        TypeNode *m_thingType;
        TypeNode *m_barrelType;
        TypeNode *m_characterType;
        TypeNode *m_clothType;

        ///\A helper to create providers. Accepts a list of tokens and assumes that
        ///the delimiter for all but first token is "." (a dot)
        /// for example, to make entity.type provider, use {"Entity", "type"} argument
        Consumer<QueryContext>* CreateProvider(std::initializer_list<std::string> tokens);
    public:
        ProvidersTest();

        ///\Initialize private variables for testing before each test.
        void setup();
        ///\Free allocated space after every test
        void teardown();
        ///\Test basic property providers (soft properties, type, id)
        void test_EntityProperty();
        ///\Test BBox providers (bbox volume, height, area etc)
        void test_BBoxProviders();
        ///\Test Outfit providers
        void test_GetEntityProviders();
        ///\Test comparator and logical predicates
        void test_ComparePredicates();
        ///\Test comparators that work on lists
        void test_ListComparators();
        ///\Test contains_recursive function provider
        ///\contains_recursive(container, condition) checks if there is an entity
        ///\that matches condition within the container
        void test_ContainsRecursive();
        ///\Test instance_of operator
        ///\In particular, cases of checking for parent type
        void test_InstanceOf();

        Inheritance* m_inheritance;

        std::map<std::string, Ref<LocatedEntity>> m_entities;

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

void ProvidersTest::test_EntityProperty()
{

    Atlas::Message::Element value;

    auto provider = CreateProvider( { "entity" });

    provider->value(value, QueryContext { *m_b1 });
    assert(value.Ptr() == m_b1.get());

    //entity.type
    provider = CreateProvider( { "entity", "type" });
    provider->value(value, QueryContext { *m_b1 });
    assert(value.Ptr() == m_barrelType);

    //entity.id
    provider = CreateProvider( { "entity", "id" });
    provider->value(value, QueryContext { *m_b1 });
    assert(value.Int() == 1);

    //entity.mass
    provider = CreateProvider( { "entity", "mass" });
    provider->value(value, QueryContext { *m_b1 });
    assert(value.Int() == 30);

    //entity.burn_speed
    provider = CreateProvider( { "entity", "burn_speed" });
    provider->value(value, QueryContext { *m_b1 });
    assert(value.Float() == 0.3);
}

void ProvidersTest::test_BBoxProviders()
{
    Atlas::Message::Element value;
    //entity.bbox.volume
    auto provider = CreateProvider( { "entity", "bbox", "volume" });
    provider->value(value, QueryContext { *m_b1 });
    assert(value.Float() == 48.0);

    //entity.bbox.height
    provider = CreateProvider( { "entity", "bbox", "height" });
    provider->value(value, QueryContext { *m_b1 });
    assert(value.Float() == 6.0);

    //entity.bbox.width
    provider = CreateProvider( { "entity", "bbox", "width" });
    provider->value(value, QueryContext { *m_b1 });
    assert(value.Float() == 2.0);

    //entity.bbox.depth
    provider = CreateProvider( { "entity", "bbox", "depth" });
    provider->value(value, QueryContext { *m_b1 });
    assert(value.Float() == 4.0);

    //entity.bbox.area
    provider = CreateProvider( { "entity", "bbox", "area" });
    provider->value(value, QueryContext { *m_b1 });
    assert(value.Float() == 8.0);
}

void ProvidersTest::test_GetEntityProviders()
{
    auto entity_provider = CreateProvider( { "entity", "attached_hand_primary" });

    GetEntityFunctionProvider getEntityProvider(entity_provider, nullptr);

    Atlas::Message::Element value;
    QueryContext queryContext{*m_ch1};
    queryContext.entity_lookup_fn = [&](const std::string& id) { return find_entity(id); };
    queryContext.type_lookup_fn = [](const std::string& id) { return Inheritance::instance().getType(id); };

    getEntityProvider.value(value, queryContext);

    assert(value.Ptr() == m_glovesEntity.get());
//
//    //Check if we get the right entity in outfit query
//    auto provider = CreateProvider( { "entity", "outfit", "hands" });
//    provider->value(value, QueryContext { *m_ch1 });
//    assert(value.Ptr() == m_glovesEntity.get());
//
//    //Check for outfit's property query
//    provider = CreateProvider( { "entity", "outfit", "hands", "color" });
//    provider->value(value, QueryContext { *m_ch1 });
//    assert(value.String() == "brown");
//
//    //Check if we get the right entity in nested outfit query
//    provider = CreateProvider(
//            { "entity", "outfit", "hands", "outfit", "thumb" });
//    provider->value(value, QueryContext { *m_ch1 });
//    assert(value.Ptr() == m_cloth.get());
//
//    //Check for nested outfit's property
//    provider = CreateProvider( { "entity", "outfit", "hands", "outfit", "thumb",
//            "color" });
//    provider->value(value, QueryContext { *m_ch1 });
//    assert(value.String() == "green");
}

void ProvidersTest::test_ComparePredicates()
{
    //entity.type = types.barrel
    auto lhs_provider1 = CreateProvider( { "entity", "type" });
    auto rhs_provider1 = CreateProvider( { "types", "barrel" });

    ComparePredicate compPred1(lhs_provider1, rhs_provider1,
                               ComparePredicate::Comparator::EQUALS);

    //entity.bbox.volume
    auto lhs_provider2 = CreateProvider( { "entity", "bbox", "volume" });

    //entity.bbox.volume = 48
    ComparePredicate compPred2(lhs_provider2, new FixedElementProvider(48.0f),
                               ComparePredicate::Comparator::EQUALS);
    assert(compPred2.isMatch(QueryContext { *m_b1 }));

    //entity.bbox.volume = 1
    ComparePredicate compPred3(lhs_provider2, new FixedElementProvider(1.0f),
                               ComparePredicate::Comparator::EQUALS);
    assert(!compPred3.isMatch(QueryContext { *m_b1 }));

    //entity.bbox.volume != 1
    ComparePredicate compPred4(lhs_provider2, new FixedElementProvider(1.0f),
                               ComparePredicate::Comparator::NOT_EQUALS);
    assert(compPred4.isMatch(QueryContext { *m_b1 }));

    //entity.bbox.volume > 0
    ComparePredicate compPred5(lhs_provider2, new FixedElementProvider(0.0f),
                               ComparePredicate::Comparator::GREATER);
    assert(compPred5.isMatch(QueryContext { *m_b1 }));

    //entity.bbox.volume >= 1
    ComparePredicate compPred6(lhs_provider2, new FixedElementProvider(1.0f),
                               ComparePredicate::Comparator::GREATER_EQUAL);
    assert(compPred6.isMatch(QueryContext { *m_b1 }));

    //entity.bbox.volume < 5
    ComparePredicate compPred7(lhs_provider2, new FixedElementProvider(5.0f),
                               ComparePredicate::Comparator::LESS);
    assert(!compPred7.isMatch(QueryContext { *m_b1 }));

    //entity.bbox.volume <= 48
    ComparePredicate compPred8(lhs_provider2, new FixedElementProvider(48.0f),
                               ComparePredicate::Comparator::LESS_EQUAL);
    assert(compPred8.isMatch(QueryContext { *m_b1 }));

    //entity.type = types.barrel && entity.bbox.volume = 48
    AndPredicate andPred1(&compPred1, &compPred2);
    assert(andPred1.isMatch(QueryContext { *m_b1 }));

    //entity.type = types.barrel && entity.bbox.volume = 1
    AndPredicate andPred2(&compPred1, &compPred3);
    assert(!andPred2.isMatch(QueryContext { *m_b1 }));

    //entity.type = types.barrel || entity.bbox.volume = 1
    OrPredicate orPred1(&compPred1, &compPred3);
    assert(orPred1.isMatch(QueryContext { *m_b1 }));

    //not entity.type = types.barrel
    NotPredicate notPred1(&compPred1);
    assert(orPred1.isMatch((QueryContext { *m_b1 })));
}

void ProvidersTest::test_ListComparators()
{

    //entity.float_list
    auto lhs_provider3 = CreateProvider( { "entity", "float_list" });

    //entity.float_list contains 20.0
    ComparePredicate compPred9(lhs_provider3, new FixedElementProvider(20.0),
                               ComparePredicate::Comparator::CONTAINS);
    assert(compPred9.isMatch(QueryContext { *m_b1 }));

    //20.0 in entity.float_list
    ComparePredicate compPred13(new FixedElementProvider(20.0), lhs_provider3,
                                ComparePredicate::Comparator::IN);
    assert(compPred13.isMatch(QueryContext { *m_b1 }));

    //entity.float_list contains 100.0
    ComparePredicate compPred10(lhs_provider3, new FixedElementProvider(100.0),
                                ComparePredicate::Comparator::CONTAINS);
    assert(!compPred10.isMatch(QueryContext { *m_b1 }));

    //100.0 in entity.float_list
    ComparePredicate compPred14(new FixedElementProvider(100.0), lhs_provider3,
                                ComparePredicate::Comparator::IN);
    assert(!compPred14.isMatch(QueryContext { *m_b1 }));

    //entity.string_list
    auto lhs_provider4 = CreateProvider( { "entity", "string_list" });

    //entity.string_list contains "foo"
    ComparePredicate compPred11(lhs_provider4, new FixedElementProvider("foo"),
                                ComparePredicate::Comparator::CONTAINS);
    assert(compPred11.isMatch(QueryContext { *m_b1 }));

    //entity.string_list contains "foobar"
    ComparePredicate compPred12(lhs_provider4,
                                new FixedElementProvider("foobar"),
                                ComparePredicate::Comparator::CONTAINS);
    assert(!compPred12.isMatch(QueryContext { *m_b1 }));
}

void ProvidersTest::test_ContainsRecursive()
{
    Element value;

    //entity.mass
    auto lhs_provider1 = CreateProvider( { "child", "mass" });
    //entity.contains
    auto lhs_provider2 = CreateProvider( { "entity", "contains" });

    //entity.mass = 30
    ComparePredicate compPred17(lhs_provider1, new FixedElementProvider(20),
                                ComparePredicate::Comparator::EQUALS);

    //contains_recursive(entity.contains, entity.mass = 30)
    //Check that container has something with mass 30 inside
    ContainsRecursiveFunctionProvider contains_recursive(lhs_provider2,
                                                         &compPred17);
    contains_recursive.value(value, QueryContext { *m_b1 });
    ASSERT_EQUAL(value.Int(), 1);

    contains_recursive.value(value, QueryContext { *m_b2 });
    ASSERT_EQUAL(value.Int(), 0);

    //entity.type
    auto lhs_provider3 = CreateProvider( { "child", "type" });
    //types.character
    auto rhs_provider1 = CreateProvider( { "types", "character" });

    //entity.type = types.character
    ComparePredicate compPred18(lhs_provider3, rhs_provider1,
                                ComparePredicate::Comparator::EQUALS);

    //contains_recursive(entity.contains, entity.type = types.character)
    //Check that the container has a character inside
    ContainsRecursiveFunctionProvider contains_recursive2(lhs_provider2,
                                                          &compPred18);

    //Should be true for both barrels since character is in b2, while b2 is in b1
    contains_recursive2.value(value, QueryContext { *m_b1 });
    ASSERT_EQUAL(1, value.Int());

    contains_recursive2.value(value, QueryContext { *m_b2 });
    ASSERT_EQUAL(value.Int(), 1);

    contains_recursive2.value(value, QueryContext { *m_ch1 });
    ASSERT_EQUAL(value.Int(), 0);
}

void ProvidersTest::test_InstanceOf()
{
    //Thing for testing instance_of
    Entity thingEntity("123", 123);
    thingEntity.setType(m_thingType);

    //Barrel is also thing but thing is not a barrel

    //entity.type = types.barrel
    auto lhs_provider1 = CreateProvider( { "entity" });
    auto rhs_provider1 = CreateProvider( { "types", "barrel" });

    ComparePredicate compPred1(lhs_provider1, rhs_provider1,
                               ComparePredicate::Comparator::INSTANCE_OF);
    ASSERT_TRUE(compPred1.isMatch(QueryContext { *m_b1 }));
    ASSERT_TRUE(!compPred1.isMatch(QueryContext { thingEntity }));

    auto rhs_provider2 = CreateProvider( { "types", "thing" });

    ComparePredicate compPred2(lhs_provider1, rhs_provider2,
                               ComparePredicate::Comparator::INSTANCE_OF);
    ASSERT_TRUE(compPred2.isMatch(QueryContext { *m_b1 }));
    ASSERT_TRUE(compPred2.isMatch(QueryContext { thingEntity }));
}

ProvidersTest::ProvidersTest()
{
    ADD_TEST(ProvidersTest::test_EntityProperty);
    ADD_TEST(ProvidersTest::test_BBoxProviders);
    ADD_TEST(ProvidersTest::test_GetEntityProviders);
    ADD_TEST(ProvidersTest::test_ComparePredicates);
    ADD_TEST(ProvidersTest::test_ListComparators);
    ADD_TEST(ProvidersTest::test_InstanceOf);
    ADD_TEST(ProvidersTest::test_ContainsRecursive);

}

void ProvidersTest::setup()
{
    m_inheritance = new Inheritance();
    //Thing is a parent type for all types except character
    m_thingType = new TypeNode("thing");
    types["thing"] = m_thingType;

    //Make a barrel with mass and burn speed properties
    m_b1 = new Entity("1", 1);
    add_entity(m_b1);
    m_barrelType = new TypeNode("barrel");
    m_barrelType->setParent(m_thingType);
    types["barrel"] = m_barrelType;
    m_b1->setType(m_barrelType);
    m_b1->setProperty("mass", new SoftProperty(Element(30)));
    m_b1->setProperty("burn_speed", new SoftProperty(Element(0.3)));
    m_b1->setProperty("isVisible", new SoftProperty(Element(true)));

    //List properties for testing list operators
    SoftProperty* prop1 = new SoftProperty();
    prop1->set(std::vector<Element> { 25.0, 20.0 });
    m_b1->setProperty("float_list", prop1);

    SoftProperty* list_prop2 = new SoftProperty();
    list_prop2->set(std::vector<Element> { "foo", "bar" });
    m_b1->setProperty("string_list", list_prop2);

    //Make a second barrel
    m_b2 = new Entity("2", 2);
    add_entity(m_b2);
    m_b2->setProperty("mass", new SoftProperty(Element(20)));
    m_b2->setProperty("burn_speed", new SoftProperty(0.25));
    m_b2->setType(m_barrelType);
    m_b2->setProperty("isVisible", new SoftProperty(Element(false)));

    //Make first barrel contain the second barrel
    m_b1_container = new LocatedEntitySet;
    m_b1_container->insert(m_b2);
    m_b1->m_contains = m_b1_container;

    //Set bounding box properties for barrels
    BBoxProperty* bbox1 = new BBoxProperty;
    //Specify two corners of bbox in form of x, y, z coordinates
    bbox1->set((std::vector<Element> { -1, -3, -2, 1, 3, 2 }));
    m_b1->setProperty("bbox", bbox1);

    BBoxProperty* bbox2 = new BBoxProperty;
    bbox2->set(std::vector<Element> { -3, -2, -1, 1, 3, 2 });
    m_b2->setProperty("bbox", bbox2);

    ///Set up outfit testing

    //Green Cloth serves as outfit for gloves
    m_clothType = new TypeNode("cloth");
    m_clothType->setParent(m_thingType);
    types["cloth"] = m_clothType;

    m_cloth = new Entity("3", 3);
    add_entity(m_cloth);
    m_cloth->setType(m_clothType);
    m_cloth->setProperty("color", new SoftProperty("green"));

    m_glovesEntity = new Entity("4", 4);
    add_entity(m_glovesEntity);
    m_glovesEntity->setProperty("color", new SoftProperty("brown"));


    //The m_cloth entity is attached to the gloves by the "thumb" attachment
    {
        auto attachedProp = new SoftProperty();
        attachedProp->data() = Atlas::Message::MapType{{"$eid", m_cloth->getId()}};
        m_glovesEntity->setProperty("attached_thumb", attachedProp);

        auto plantedOnProp = new PlantedOnProperty();
        plantedOnProp->data().entity = WeakEntityRef(m_glovesEntity.get());
        m_cloth->setProperty("planted_on", plantedOnProp);
    }

    //Create the character for testing
    m_characterType = new TypeNode("character");
    types["character"] = m_characterType;
    m_ch1 = new Entity("5", 5);
    add_entity(m_ch1);
    m_ch1->setType(m_characterType);

    //The m_glovesEntity entity is attached to the m_ch1 by the "hand_primary" attachment
    {
        auto attachedHandPrimaryProp = new SoftProperty();
        attachedHandPrimaryProp->data() = Atlas::Message::MapType{{"$eid", m_glovesEntity->getId()}};
        m_ch1->setProperty("attached_hand_primary", attachedHandPrimaryProp);

        auto plantedOnProp = new PlantedOnProperty();
        plantedOnProp->data().entity = WeakEntityRef(m_ch1.get());
        m_glovesEntity->setProperty("planted_on", plantedOnProp);
    }

    //Make second barrel contain the character
    m_b2_container = new LocatedEntitySet;
    m_b2_container->insert(m_ch1);
    m_b2->m_contains = m_b2_container;
}

void ProvidersTest::teardown()
{
    delete m_inheritance;
    delete m_barrelType;
    delete m_characterType;
    delete m_clothType;
    delete m_thingType;

    m_entities.clear();
}

Consumer<QueryContext>* ProvidersTest::CreateProvider(std::initializer_list<
        std::string> tokens)
{
    ProviderFactory::SegmentsList segments;
    auto iter = tokens.begin();

    //First token doesn't have a delimiter, so just add it.
    segments.push_back(ProviderFactory::Segment { "", *iter++ });

    //Starting from the second token, add them to the list of segments with "." delimiter
    for (; iter != tokens.end(); iter++) {
        segments.push_back(ProviderFactory::Segment { ".", *iter });
    }
    return m_factory.createProviders(segments);
}

int main()
{
    ProvidersTest t;

    return t.run();
}

//Stubs

#include "stubs/common/stubVariable.h"
#include "stubs/common/stubMonitors.h"
#include "stubs/common/stubLink.h"
#include "stubs/rulesets/stubDomainProperty.h"
#include "stubs/rulesets/stubDensityProperty.h"
#include "stubs/rulesets/stubScaleProperty.h"
#include "stubs/rulesets/stubAtlasProperties.h"
#include "stubs/common/stubCustom.h"
#include "stubs/common/stubRouter.h"
#include "stubs/rulesets/stubBaseWorld.h"
#include "stubs/rulesets/stubLocation.h"

#define STUB_Inheritance_getType
const TypeNode* Inheritance::getType(const std::string & parent)
{
    auto I = types.find(parent);
    if (I == types.end()) {
        return 0;
    }
    return I->second;
}

#include "stubs/common/stubInheritance.h"


void log(LogLevel lvl, const std::string & msg)
{
}

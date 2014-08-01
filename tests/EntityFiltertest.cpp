#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

//TODO: Check for unnecessary includes/links
#include "rulesets/entityfilter/Filter.h"

#include "rulesets/entityfilter/Providers.h"

#include "rulesets/entityfilter/ParserDefinitions.h"

#include "rulesets/EntityProperty.h"
#include "rulesets/Domain.h"
#include "rulesets/AtlasProperties.h"
#include "rulesets/OutfitProperty.h"
#include "rulesets/BBoxProperty.h"
#include "common/Property.h"
#include "common/BaseWorld.h"
#include "common/log.h"
#include "common/Inheritance.h"

#include "rulesets/Entity.h"
#include "common/TypeNode.h"

#include <wfmath/point.h>
#include <Atlas/Objects/Anonymous.h>

#include <cassert>

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Objects::Entity::Anonymous;

static std::map<std::string, TypeNode*> types;


//\brief a tester function for entity filter. Accepts a query and lists of entities that
// are supposed to pass or fail the test for a given query
void TestQuery(const std::string& query,
               std::initializer_list<Entity*> entitiesToPass,
               std::initializer_list<Entity*> entitiesToFail)
{
    EntityFilter::Filter f(query);
    for (auto iter = entitiesToPass.begin(); iter != entitiesToPass.end();
            ++iter) {
        assert(f.match(**iter));
    }
    for (auto iter = entitiesToFail.begin(); iter != entitiesToFail.end();
            ++iter) {
        assert(!f.match(**iter));
    }
}
int main()
{
    using namespace EntityFilter;

    //Set up testing environment for Type/Soft properties
    Entity b1("1", 1);
    TypeNode* barrelType = new TypeNode("barrel");
    types["barrel"] = barrelType;
    b1.setType(barrelType);
    b1.setProperty("mass", new SoftProperty(Element(30)));
    b1.setProperty("burn_speed", new SoftProperty(Element(0.3)));

    Entity b2("2", 2);
    b2.setProperty("mass", new SoftProperty(Element(20)));
    b2.setProperty("burn_speed", new SoftProperty(0.25));
    b2.setType(barrelType);

    Entity b3("3", 3);
    b3.setProperty("mass", new SoftProperty(Element(25)));
    b3.setProperty("burn_speed", new SoftProperty(Element(0.25)));
    b3.setType(barrelType);

    TypeNode* boulderType = new TypeNode("boulder");
    types["boulder"] = boulderType;
    Entity bl1("4", 4);
    bl1.setProperty("mass", new SoftProperty(Element(25)));
    bl1.setType(boulderType);

    SoftProperty* prop1 = new SoftProperty();
    prop1->set(std::vector<Element> {25.0, 20.0});
    bl1.setProperty("float_list", prop1);

    SoftProperty* list_prop2 = new SoftProperty();
    list_prop2->set(std::vector<Element> {"foo", "bar"});
    bl1.setProperty("string_list", list_prop2);


// START of Soft property and general filtering tests
    {
        TestQuery("entity.type is_instance types.barrel", { &b1 }, { &bl1 });
        // test entity.attribute case with various operators
        TestQuery("entity.burn_speed=0.3", { &b1 }, { &b2 });

        TestQuery("entity.burn_speed>0.3", { }, { &b1, &bl1 });

        TestQuery("entity.burn_speed<0.3", { &b2 }, { &b1 });

        //test list of floats comparison
        //TestQuery("entity.float_list=[25]", { &bl1 }, { &b1 });

        //TestQuery("entity.float_list==[25, 20]", { &bl1 }, { &b1 });

        //test empty lists
        //TestQuery("entity.float_list==[]", { }, { &bl1 });

        //TestQuery("entity.float_list=[]", { }, { &bl1 });

        //test list of strings
        //TestQuery("entity.string_list=[bar]", { &bl1 }, { &b1 });

        //TestQuery("entity.string_list=[foo, bar]", { &bl1 }, { &b1 });

        //test query with several criteria

        TestQuery("entity.type=types.barrel&entity.burn_speed=0.3", { &b1 }, { &b2,
                          &bl1 });

        //test logical operators and precedence

        TestQuery("entity.type=types.barrel|entity.type=types.boulder", { &b1, &bl1 }, { });

        TestQuery(
                "entity.type=types.boulder|entity.type=types.barrel&entity.burn_speed=0.3",
                { &b1, &bl1 }, { });

        //test query with parenthesis
        TestQuery("(entity.type=types.boulder)", {&bl1}, {&b1});

        TestQuery("(entity.type=types.boulder)&(entity.mass=25)", {&bl1}, {&b1});

        //test query with nested parentheses
        TestQuery("(entity.type=types.barrel&(entity.mass=25|entity.mass=30)|entity.type=types.boulder)", {&b1, &b3, &bl1}, {&b2});

        TestQuery("(entity.type=types.barrel&(entity.mass=25&(entity.burn_speed=0.25|entity.mass=30))|entity.type=types.boulder)", {&bl1}, {&b1});

        //override precedence rules with parentheses
        TestQuery("(entity.type=types.boulder|entity.type=types.barrel)&entity.burn_speed=0.3", {&b1}, {&bl1});

        //test operators "and", "or"
        TestQuery("(entity.type=types.barrel and entity.burn_speed=0.3)", {&b1}, {&bl1});

        TestQuery("entity.type is_instance types.barrel   or   entity.mass=25", {&b1, &b2, &bl1}, {});

        try {
            TestQuery("entity.type=types.barrelandentity.burn_speed=0.3", { }, { });
            assert(false);
        } catch (EntityFilter::InvalidQueryException& e) {
        }
        //test query with spaces
        TestQuery("  entity.type = types.barrel   ", { &b1 }, { &bl1 });

        try {
            TestQuery("foobar", { }, { &b1, &bl1 });
            assert(false);
        } catch (EntityFilter::InvalidQueryException& e) {
        }
    }

    // END of soft property and general tests

    //Set up testing environment for Outfit property
    TypeNode* glovesType = new TypeNode("gloves");
    types["gloves"] = glovesType;
    TypeNode* bootsType = new TypeNode("boots");
    TypeNode* characterType = new TypeNode("character");
    types["character"] = characterType;
    TypeNode* clothType = new TypeNode("cloth");
    types["cloth"] = clothType;
    TypeNode* leatherType = new TypeNode("leather");

    Entity glovesEntity("5", 5);
    glovesEntity.setType(glovesType);
    glovesEntity.setProperty("color", new SoftProperty("brown"));
    glovesEntity.setProperty("mass", new SoftProperty(5));

    Entity bootsEntity("6", 6);
    bootsEntity.setType(bootsType);
    bootsEntity.setProperty("color", new SoftProperty("black"));
    bootsEntity.setProperty("mass", new SoftProperty(10));

    std::map<std::string, Element> outfitMap;
    outfitMap.insert(std::make_pair("feet", Element(&bootsEntity)));
    outfitMap.insert(std::make_pair("hands", Element(&glovesEntity)));
    OutfitProperty* outfit1 = new OutfitProperty;
    outfit1->set(outfitMap);

    Entity cloth("8", 8);
    cloth.setType(clothType);
    cloth.setProperty("color", new SoftProperty("green"));

    Entity leather("9", 9);
    leather.setType(leatherType);
    leather.setProperty("color", new SoftProperty("pink"));

    std::map<std::string, Element> outfitMap1;
    outfitMap1.insert(std::make_pair("thumb", Element(&cloth)));
    OutfitProperty* outfit2 = new OutfitProperty;
    outfit2->set(outfitMap1);
    glovesEntity.setProperty("outfit", outfit2);

    Entity ch1("7", 7);
    ch1.setType(characterType);
    ch1.setProperty("outfit", outfit1);

    //START of outfit case test
    {
        //Test soft property of outfit
        TestQuery("entity.outfit.hands.color='brown'", { &ch1 }, { });
        //Test type of outfit
        TestQuery("entity.outfit.hands.type=types.gloves", { &ch1 }, { });
        //Test an entity without outfit
        TestQuery("entity.outfit.hands.type=types.gloves", { }, { &bl1 });
        //Test outfit that doesn't have the specified part
        TestQuery("entity.outfit.chest.color='red'", { }, { &ch1 });
        //Test outfit with another criterion
        TestQuery("entity.type=types.character&entity.outfit.hands.color='brown'", { &ch1 },
                  { &b1 });
        //Test nested outfit
        TestQuery("entity.outfit.hands.outfit.thumb.color='green'", {&ch1}, {});

        TestQuery("entity.outfit.hands.outfit.thumb.type=types.cloth", {&ch1}, {});
    }
    //END of outfit case test

//Set up bbox testing environment

    BBoxProperty* bbox1 = new BBoxProperty;
    bbox1->set((std::vector<Element> { -1, -2, -3, 1, 2, 3 }));
    b1.setProperty("bbox", bbox1);

    BBoxProperty* bbox2 = new BBoxProperty;
    bbox2->set(std::vector<Element> { -3, -1, -2, 1, 2, 3 });
    bl1.setProperty("bbox", bbox2);

    cloth.setProperty("bbox", bbox1->copy());
    //START of BBox tests
    {
        //Test BBox volume
        TestQuery("entity.bbox.volume=48.0", { &b1 }, { &bl1 });
        //Test BBox height
        TestQuery("entity.bbox.height=6.0", { &b1 }, { &bl1 });
        //Test BBox length
        TestQuery("entity.bbox.depth=4.0", { &b1 }, { &bl1 });
        //Test BBox width
        TestQuery("entity.bbox.width=2.0", { &b1 }, { &bl1 });
        //Test BBox area
        TestQuery("entity.bbox.area=8.0", { &b1 }, { &bl1 });
        //Test BBox with another criterion
        TestQuery("entity.type=types.barrel&entity.bbox.height>0.0", { &b1 }, { &b2,
                          &bl1 });
        //Test BBox of an outfit
        TestQuery("entity.outfit.hands.outfit.thumb.bbox.volume=48.0", {&ch1}, {});
    }
    //END of BBox tests


    {
        ProviderFactory factory;

        //entity
        Atlas::Message::Element value;
        ProviderFactory::SegmentsList segments;
        segments.push_back(ProviderFactory::Segment{"", "entity"});
        auto provider = factory.createProviders(segments);
        provider->value(value, QueryContext{b1});
        assert(value.Ptr() == &b1);

        //entity.type
        segments.clear();
        segments.push_back(ProviderFactory::Segment{"", "entity"});
        segments.push_back(ProviderFactory::Segment{".", "type"});
        provider = factory.createProviders(segments);
        provider->value(value, QueryContext{b1});
        assert(value.Ptr() == barrelType);


        //entity.mass
        segments.clear();
        segments.push_back(ProviderFactory::Segment{"", "entity"});
        segments.push_back(ProviderFactory::Segment{".", "mass"});
        provider = factory.createProviders(segments);
        provider->value(value, QueryContext{b1});
        assert(value.Int() == 30);




        //entity.outfit.hands.color
        segments.clear();
        segments.push_back(ProviderFactory::Segment{"", "entity"});
        segments.push_back(ProviderFactory::Segment{".", "outfit"});
        segments.push_back(ProviderFactory::Segment{".", "hands"});
        segments.push_back(ProviderFactory::Segment{".", "color"});
        provider = factory.createProviders(segments);
        provider->value(value, QueryContext{ch1});
        assert(value.String() == "brown");



        //entity.bbox.volume
        segments.clear();
        segments.push_back(ProviderFactory::Segment{"", "entity"});
        segments.push_back(ProviderFactory::Segment{".", "bbox"});
        segments.push_back(ProviderFactory::Segment{".", "volume"});
        provider = factory.createProviders(segments);
        provider->value(value, QueryContext{b1});
        assert(value.asNum() == 48);



        //entity.right_hand_wield.color
        EntityProperty* wield_prop = new EntityProperty();
        wield_prop->data() = EntityRef(&glovesEntity);
        ch1.setProperty("right_hand_wield", wield_prop);

        segments.clear();
        segments.push_back(ProviderFactory::Segment{"", "entity"});
        segments.push_back(ProviderFactory::Segment{".", "right_hand_wield"});
        segments.push_back(ProviderFactory::Segment{".", "color"});
        provider = factory.createProviders(segments);
        provider->value(value, QueryContext{ch1});
        assert(value.String() == "brown");



        //types.barrel
        segments.clear();
        segments.push_back(ProviderFactory::Segment{"", "types"});
        segments.push_back(ProviderFactory::Segment{".", "barrel"});
        provider = factory.createProviders(segments);
        provider->value(value, QueryContext{ch1});
        assert(value.Ptr() == barrelType);


        //types.barrel.name
        segments.clear();
        segments.push_back(ProviderFactory::Segment{"", "types"});
        segments.push_back(ProviderFactory::Segment{".", "barrel"});
        segments.push_back(ProviderFactory::Segment{".", "name"});
        provider = factory.createProviders(segments);
        provider->value(value, QueryContext{ch1});
        assert(value.String() == "barrel");














        //entity.type
        segments.clear();
        segments.push_back(ProviderFactory::Segment{"", "entity"});
        segments.push_back(ProviderFactory::Segment{".", "type"});
        auto lhs_provider1 = factory.createProviders(segments);


        //types.barrel
        segments.clear();
        segments.push_back(ProviderFactory::Segment{"", "types"});
        segments.push_back(ProviderFactory::Segment{".", "barrel"});
        auto rhs_provider1 = factory.createProviders(segments);

        //entity.type = types.barrel
        ComparePredicate compPred1(lhs_provider1, rhs_provider1, ComparePredicate::Comparator::INSTANCE_OF);
        assert(compPred1.isMatch(QueryContext{b1}));





        //entity.bbox.volume
        segments.clear();
        segments.push_back(ProviderFactory::Segment{"", "entity"});
        segments.push_back(ProviderFactory::Segment{".", "bbox"});
        segments.push_back(ProviderFactory::Segment{".", "volume"});
        auto lhs_provider2 = factory.createProviders(segments);

        //entity.bbox.volume = 48
        ComparePredicate compPred2(lhs_provider2, new FixedElementProvider(48.0f), ComparePredicate::Comparator::EQUALS);
        assert(compPred2.isMatch(QueryContext{b1}));

        //entity.bbox.volume = 1
        ComparePredicate compPred3(lhs_provider2, new FixedElementProvider(1.0f), ComparePredicate::Comparator::EQUALS);
        assert(!compPred3.isMatch(QueryContext{b1}));

        //entity.bbox.volume != 1
        ComparePredicate compPred4(lhs_provider2, new FixedElementProvider(1.0f), ComparePredicate::Comparator::NOT_EQUALS);
        assert(compPred4.isMatch(QueryContext{b1}));

        //entity.bbox.volume > 0
        ComparePredicate compPred5(lhs_provider2, new FixedElementProvider(0.0f), ComparePredicate::Comparator::GREATER);
        assert(compPred5.isMatch(QueryContext{b1}));

        //entity.bbox.volume >= 1
        ComparePredicate compPred6(lhs_provider2, new FixedElementProvider(1.0f), ComparePredicate::Comparator::GREATER_EQUAL);
        assert(compPred6.isMatch(QueryContext{b1}));

        //entity.bbox.volume < 5
        ComparePredicate compPred7(lhs_provider2, new FixedElementProvider(5.0f), ComparePredicate::Comparator::LESS);
        assert(!compPred7.isMatch(QueryContext{b1}));

        //entity.bbox.volume <= 48
        ComparePredicate compPred8(lhs_provider2, new FixedElementProvider(48.0f), ComparePredicate::Comparator::LESS_EQUAL);
        assert(compPred8.isMatch(QueryContext{b1}));

        //entity.type = types.barrel && entity.bbox.volume = 48
        AndPredicate andPred1(&compPred1, &compPred2);
        assert(andPred1.isMatch(QueryContext{b1}));

        //entity.type = types.barrel && entity.bbox.volume = 1
        AndPredicate andPred2(&compPred1, &compPred3);
        assert(!andPred2.isMatch(QueryContext{b1}));

        //entity.type = types.barrel || entity.bbox.volume = 1
        OrPredicate orPred1(&compPred1, &compPred3);
        assert(orPred1.isMatch(QueryContext{b1}));

        //entity.float_list
        segments.clear();
        segments.push_back(ProviderFactory::Segment { "", "entity" });
        segments.push_back(ProviderFactory::Segment { ".", "float_list" });
        auto lhs_provider3 = factory.createProviders(segments);

        //entity.float_list contains 20.0
        ComparePredicate compPred9(lhs_provider3, new FixedElementProvider(20.0), ComparePredicate::Comparator::CONTAINS);
        assert(compPred9.isMatch(QueryContext{bl1}));

        //entity.float_list contains 100.0
        ComparePredicate compPred10(lhs_provider3, new FixedElementProvider(100.0), ComparePredicate::Comparator::CONTAINS);
        assert(!compPred10.isMatch(QueryContext{bl1}));

        //entity.string_list
        segments.clear();
        segments.push_back(ProviderFactory::Segment { "", "entity" });
        segments.push_back(ProviderFactory::Segment { ".", "string_list" });
        auto lhs_provider4 = factory.createProviders(segments);

        //entity.string_list contains "foo"
        ComparePredicate compPred11(lhs_provider4, new FixedElementProvider("foo"), ComparePredicate::Comparator::CONTAINS);
        assert(compPred11.isMatch(QueryContext{bl1}));

        //entity.string_list contains "foobar"
        ComparePredicate compPred12(lhs_provider4, new FixedElementProvider("foobar"), ComparePredicate::Comparator::CONTAINS);
        assert(!compPred12.isMatch(QueryContext{bl1}));
    }

//    Clean up
    delete barrelType;
    delete boulderType;
    delete glovesType;
    delete bootsType;
    delete characterType;
}

//Stubs
Domain * Domain::m_instance = new Domain();

Domain::Domain() :
        m_refCount(0)
{
}

Domain::~Domain()
{
}

float Domain::constrainHeight(LocatedEntity * parent,
                              const Point3D & pos,
                              const std::string & mode)
{
    return 0.f;
}

void Domain::tick(double t)
{
}
IdProperty::IdProperty(const std::string & data) :
        PropertyBase(per_ephem), m_data(data)
{
}

int IdProperty::get(Atlas::Message::Element & e) const
{
    e = m_data;
    return 0;
}

void IdProperty::set(const Atlas::Message::Element & e)
{
}

void IdProperty::add(const std::string & key,
                     Atlas::Message::MapType & ent) const
{
}

void IdProperty::add(const std::string & key,
                     const Atlas::Objects::Entity::RootEntity & ent) const
{
}

IdProperty * IdProperty::copy() const
{
    return 0;
}

ContainsProperty::ContainsProperty(LocatedEntitySet & data) :
        PropertyBase(per_ephem), m_data(data)
{
}

int ContainsProperty::get(Atlas::Message::Element & e) const
{
    return 0;
}

void ContainsProperty::set(const Atlas::Message::Element & e)
{
}

void ContainsProperty::add(const std::string & s,
                           const Atlas::Objects::Entity::RootEntity & ent) const
{
}

ContainsProperty * ContainsProperty::copy() const
{
    return 0;
}

namespace Atlas
{
namespace Objects
{
namespace Operation
{
int ACTUATE_NO = -1;
int ATTACK_NO = -1;
int EAT_NO = -1;
int NOURISH_NO = -1;
int SETUP_NO = -1;
int TICK_NO = -1;
int UPDATE_NO = -1;
int RELAY_NO = -1;
}
}
}
Router::Router(const std::string & id, long intId) :
        m_id(id), m_intId(intId)
{
}

Router::~Router()
{
}

void Router::addToMessage(Atlas::Message::MapType & omap) const
{
}

void Router::addToEntity(const Atlas::Objects::Entity::RootEntity & ent) const
{
}
BaseWorld*BaseWorld::m_instance = 0;
BaseWorld::BaseWorld(LocatedEntity & gw) :
        m_gameWorld(gw)
{
    m_instance = this;
}

BaseWorld::~BaseWorld()
{
    m_instance = 0;
}

LocatedEntity * BaseWorld::getEntity(const std::string & id) const
{
    return 0;
}

void Location::addToMessage(MapType & omap) const
{
}

Location::Location() :
        m_loc(0)
{
}

void Location::addToEntity(const Atlas::Objects::Entity::RootEntity & ent) const
{
}
void Location::modifyBBox()
{
}


Inheritance::Inheritance() {}

Inheritance & Inheritance::instance()
{
    return *(new Inheritance());
}

const TypeNode * Inheritance::getType(const std::string & parent)
{
    auto I = types.find(parent);
    if (I == types.end()) {
        return 0;
    }
    return I->second;
}

void log(LogLevel lvl, const std::string & msg)
{
}


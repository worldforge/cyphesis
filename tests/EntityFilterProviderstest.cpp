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

using namespace EntityFilter;
using Atlas::Message::Element;

static std::map<std::string, TypeNode*> types;

class ProvidersTest : public Cyphesis::TestBase {
    private:
        ProviderFactory m_factory;

        //Entities for testing
        Entity *m_b1;
        LocatedEntitySet *m_b1_container; //Container property for b1
        Entity *m_b2;

        //Types for testing
        TypeNode *m_barrelType;

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

};

void ProvidersTest::test_EntityProperty()
{

    Atlas::Message::Element value;

    auto provider = CreateProvider( { "entity" });

    provider->value(value, QueryContext { *m_b1 });
    assert(value.Ptr() == m_b1);

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
    auto provider = CreateProvider( { "entity", "BBox", "Volume" });
    provider->value(value, QueryContext { *m_b1 });
    ASSERT_TRUE(value.Float() == 48.0);

    //entity.bbox.height
    provider = CreateProvider( { "entity", "BBox", "Height" });
    provider->value(value, QueryContext { *m_b1 });
    ASSERT_TRUE(value.Float() == 6.0);

    //entity.bbox.width
    provider = CreateProvider( { "entity", "BBox", "Width" });
    provider->value(value, QueryContext { *m_b1 });
    ASSERT_TRUE(value.Float() == 2.0);

    //entity.bbox.depth
    provider = CreateProvider( { "entity", "BBox", "Depth" });
    provider->value(value, QueryContext { *m_b1 });
    ASSERT_TRUE(value.Float() == 4.0);

    //entity.bbox.area
    provider = CreateProvider( { "entity", "BBox", "Area" });
    provider->value(value, QueryContext { *m_b1 });
    ASSERT_TRUE(value.Float() == 8.0);
}

ProvidersTest::ProvidersTest()
{
    ADD_TEST(ProvidersTest::test_EntityProperty);
    ADD_TEST(ProvidersTest::test_BBoxProviders);
}

void ProvidersTest::setup()
{
    //Make a barrel with mass and burn speed properties
    m_b1 = new Entity("1", 1);
    m_barrelType = new TypeNode("barrel");
    types["barrel"] = m_barrelType;
    m_b1->setType(m_barrelType);
    m_b1->setProperty("mass", new SoftProperty(Element(30)));
    m_b1->setProperty("burn_speed", new SoftProperty(Element(0.3)));
    m_b1->setProperty("isVisible", new SoftProperty(Element(true)));

    //Make a second barrel
    m_b2 = new Entity("2", 2);
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
    bbox1->set((std::vector<Element> { -1, -2, -3, 1, 2, 3 }));
    m_b1->setProperty("bbox", bbox1);

    BBoxProperty* bbox2 = new BBoxProperty;
    bbox2->set(std::vector<Element> { -3, -1, -2, 1, 2, 3 });
    m_b1->setProperty("bbox", bbox2);
}

void ProvidersTest::teardown()
{

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

    t.run();
}

//Stubs

#include "stubs/common/stubVariable.h"
#include "stubs/common/stubMonitors.h"
#include "stubs/rulesets/stubDomainProperty.h"
#include "stubs/rulesets/stubIdProperty.h"

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

Inheritance::Inheritance()
{
}

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

#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

//TODO: Check for unnecessary includes/links
#include "rulesets/entityfilter/Filter.h"
#include "rulesets/EntityProperty.h"
#include "rulesets/Domain.h"
#include "rulesets/AtlasProperties.h"
#include "rulesets/OutfitProperty.h"
#include "rulesets/BBoxProperty.h"
#include "common/Property.h"
#include "common/BaseWorld.h"
#include "common/log.h"

#include "rulesets/Entity.h"
#include "common/TypeNode.h"

#include <wfmath/point.h>
#include <Atlas/Objects/Anonymous.h>

#include <cassert>

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Objects::Entity::Anonymous;

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
    Entity bl1("4", 4);
    bl1.setProperty("mass", new SoftProperty(Element(25)));
    bl1.setType(boulderType);

// START of Soft property and general filtering tests
    {
        TestQuery("entity.type=barrel", { &b1 }, { &bl1 });

        // test entity.attribute case with various operators
        TestQuery("entity.burn_speed=0.3", { &b1 }, { &b2 });

        TestQuery("entity.burn_speed>0.3", { }, { &b1, &bl1 });

        TestQuery("entity.burn_speed<0.3", { &b2 }, { &b1 });

        //test query with several criteria

        TestQuery("entity.type=barrel&entity.burn_speed=0.3", { &b1 }, { &b2,
                          &bl1 });

        //test logical operators and precedence

        TestQuery("entity.type=barrel|entity.type=boulder", { &b1, &bl1 }, { });
        TestQuery(
                "entity.type=boulder|entity.type=barrel&entity.burn_speed=0.3",
                { &b1, &bl1 }, { });

        //test query with spaces
        TestQuery("  entity.type = barrel   ", { &b1 }, { &bl1 });
        //TODO: Test invalid query.
        try {
            TestQuery("foobar", { }, { &b1, &bl1 });
        } catch (EntityFilter::InvalidQueryException& e) {
            log(WARNING, e.what());
        }
    }
    // END of soft property and general tests

    //Set up testing environment for Outfit property
    TypeNode* glovesType = new TypeNode("gloves");
    TypeNode* bootsType = new TypeNode("boots");
    TypeNode* characterType = new TypeNode("character");

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

    Entity ch1("7", 7);
    ch1.setType(characterType);
    ch1.setProperty("outfit", outfit1);

    //START of outfit case test
    {
        //Test soft property of outfit
        TestQuery("entity.outfit.hands.color=brown", { &ch1 }, { });
        //Test type of outfit
        TestQuery("entity.outfit.hands.type=gloves", { &ch1 }, { });
        //Test an entity without outfit
        TestQuery("entity.outfit.hands.type=gloves", { }, { &bl1 });
        //Test outfit that doesn't have the specified part
        TestQuery("entity.outfit.chest.color=red", { }, { &ch1 });
        //Test outfit with another criterion
        TestQuery("entity.type=character&entity.outfit.hands.color=brown", {
                          &ch1 },
                  { &b1 });

    }
    //END of outfit case test

//Set up bbox testing environment

    BBoxProperty* bbox1 = new BBoxProperty;
    bbox1->set((std::vector<Element> { -1, -2, -3, 1, 2, 3 }));
    b1.setProperty("bbox", bbox1);

    BBoxProperty* bbox2 = new BBoxProperty;
    bbox2->set(std::vector<Element> { -3, -1, -2, 1, 2, 3 });
    bl1.setProperty("bbox", bbox2);

    //START of BBox tests
    {
        //Test BBox volume
        TestQuery("entity.bbox.volume=48", { &b1 }, { &bl1 });
        //Test BBox height
        TestQuery("entity.bbox.height=6", { &b1 }, { &bl1 });
        //Test BBox length
        TestQuery("entity.bbox.length=4", { &b1 }, { &bl1 });
        //Test BBox width
        TestQuery("entity.bbox.width=2", { &b1 }, { &bl1 });
        //Test BBox area
        TestQuery("entity.bbox.area=8", { &b1 }, { &bl1 });
        //Test BBox with another criterion
        TestQuery("entity.type=barrel&entity.bbox.height>0", { &b1 }, { &b2,
                          &bl1 });
    }
    //END of BBox tests

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
void log(LogLevel lvl, const std::string & msg)
{
}


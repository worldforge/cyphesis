#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include "rulesets/EntityProperty.h"

#include "rulesets/Entity.h"

#include "../rulesets/entityfilter/Filter.h"

#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/RootOperation.h>

#include <cassert>

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Objects::Entity::Anonymous;

int main(){
    using namespace EntityFilter;
    //Set up testing environment
    Entity b1("1", 1);
    Atlas::Message::Element attr;
    b1.setAttr("mass", Element(15));
    b1.setAttr("burn_speed", Element(0.3));
    TypeNode* barrelType = new TypeNode("barrel");
    b1.setType(barrelType);

    Entity b2("2", 2);
    b2.setAttr("mass", Element(20));
    b2.setAttr("burn_speed", Element(0.25));
    b1.setType(barrelType);

    Entity b3("3", 3);
    b3.setAttr("mass", Element(25));
    b3.setAttr("burn_speed", Element(0.25));
    b3.setType(barrelType);

    TypeNode* boulderType = new TypeNode("boulder");
    Entity bl1("4", 3);
    bl1.setAttr("mass", Element(25));
    bl1.setType(boulderType);


    //test entity.type case
    {
        Filter f("entity.type=barrel");
        assert(f.search(b1));
        assert(!f.search(bl1));
    }

    // test entity.attribute case with various operators
    {
        Filter f("entity.burn_speed=0.3");
        assert(f.search(b1));
        assert(!f.search(b2));
    }
    {
        Filter f("entity.burn_speed>0.3");
        assert (!f.search(b1));
    }
    {
        Filter f("entity.burn_speed<0.3");
        assert(f.search(b2));
        assert(!f.search(b1));
    }

    //test query with several criteria
    {
        Filter f("entity.type=barrel&entity.burn_speed=0.3");
        assert(f.search(b1));
        assert(!f.search(b2));
        assert(!f.search(bl1));
    }

    //test logical operators and precedence
    {
        Filter f("entity.type=barrel|entity.type=boulder");
        assert(f.search(b1));
        assert(f.search(bl1));
    }
    {
        Filter f("entity.type=boulder|entity.type=barrel&entity.burn_speed=0.3");
        assert(f.search(b1));
        assert(f.search(bl1));
    }


    //Clean up
    delete barrelType;
    delete boulderType;
}

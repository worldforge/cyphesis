#include <Atlas/Message/Object.h>
#include <Atlas/Objects/Operation/Login.h>

#include "Thing.h"
#include "ThingFactory.h"
#include "Python_API.h"

#include <server/WorldRouter.h>

#include <common/const.h>

#include "Character.h"
#include "Creator.h"

static int debug_thingf = 0;

using Atlas::Message::Object;

void init_python_api();

ThingFactory thing_factory;

ThingFactory::ThingFactory()
{
    init_python_api();
}

void ThingFactory::readRuleset(const string & setname)
{
    global_conf->readFromFile(setname+".vconf");
}

Thing * ThingFactory::newThing(const string & type,const Object & ent, Routing * svr)
{
    if (!ent.IsMap()) {
         debug_thingf && cout << "Entity is not a map" << endl << flush;
    }
    Object::MapType entmap = ent.AsMap();
    Thing * thing;
    string py_package;
    if (type.size() == 0) {
        thing = new Thing();
    } else if (type == "creator") {
        thing = new Creator();
    } else if (global_conf->findItem("characters", type)) {
        thing = new Character();
        py_package = global_conf->getItem("characters", type);
    } else if (global_conf->findItem("things", type)) {
        thing = new Thing();
        py_package = global_conf->getItem("things", type);
    } else {
        thing = new Thing();
    }
    debug_thingf && cout << "[" << type << " " << thing->name << "]" << endl << flush;
    thing->type = type;
    // Sort out python object
    if (py_package.size() != 0) {
        Create_PyThing(thing, py_package, type);
    }
    if (entmap.find("name") != entmap.end() && entmap["name"].IsString()) {
        thing->name = entmap["name"].AsString();
    } else {
        debug_thingf && cout << "Got no name" << endl << flush;
    }
    thing->merge(entmap);
    // Get location from entity, if it is present
    thing->getLocation(entmap, svr->fobjects);
    return(thing);
}

// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#include <Atlas/Message/Object.h>
#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/Operation/Login.h>
#include <Atlas/Objects/Operation/Create.h>
#include <Atlas/Objects/Operation/Sight.h>
#include <Atlas/Objects/Operation/Set.h>
#include <Atlas/Objects/Operation/Delete.h>
#include <Atlas/Objects/Operation/Move.h>
#include <Atlas/Objects/Operation/Sound.h>
#include <Atlas/Objects/Operation/Touch.h>
#include <Atlas/Objects/Operation/Look.h>
#include <Atlas/Objects/Operation/Appearance.h>
#include <Atlas/Objects/Operation/Disappearance.h>

#include <common/Setup.h>
#include <common/Tick.h>
#include <common/Chop.h>
#include <common/Cut.h>
#include <common/Eat.h>
#include <common/Nourish.h>
#include <common/Fire.h>

#include "Entity.h"
#include "Script.h"

#include <common/const.h>
#include <common/debug.h>

static const bool debug_flag = false;

using Atlas::Message::Object;

Entity::Entity() : script(new Script), status(1),
                 type("thing"), isCharacter(false), weight(-1)
{
    inGame = true;
    name = string("Foo");
    attributes["mode"] = Object("birth");
}

Entity::~Entity() { }

const Object & Entity::operator[](const string & aname)
{
    if (aname == "status") {
        attributes[aname] = Object(status);
    } else if (aname == "id") {
        attributes[aname] = Object(fullid);
    } else if (aname == "name") {
        attributes[aname] = Object(name);
    } else if (aname == "weight") {
        attributes[aname] = Object(weight);
    } else if (aname == "contains") {
        Object::ListType contlist;
        for(list_t::const_iterator I=contains.begin();I!=contains.end();I++) {
            contlist.push_back(*I);
        }
        attributes[aname] = Object(contlist);
    } else if (attributes.find(aname) == attributes.end()) {
        attributes[aname] = Object();
    }
    return(attributes[aname]);
}

void Entity::set(const string & aname, const Object & attr)
{
    if ((aname == "status") && attr.IsFloat()) {
        status = attr.AsFloat();
    } else if ((aname == "name") && attr.IsString()) {
        name = attr.AsString();
    } else if ((aname == "weight") && attr.IsFloat()) {
        weight = attr.AsFloat();
    } else {
        attributes[aname] = attr;
    }
}

int Entity::setScript(Script * scrpt) {
    script = scrpt;
    return(scrpt == NULL ? -1 : 0);
}

MemMap * Entity::getMap() {
    return NULL;
}

void Entity::addToObject(Object * obj) const
{
    Object::MapType & omap = obj->AsMap();
    omap["name"] = Object(name);
    omap["type"] = Object(type);
    omap["parents"] = Object(Object::ListType(1,Object(type)));
    // We need to have a list of keys to pull from attributes.
    location.addToObject(obj);
    BaseEntity::addToObject(obj);
}

void Entity::merge(const Object::MapType & entmap)
{
    Object::MapType::const_iterator I;
    for (I=entmap.begin(); I!=entmap.end(); I++) {
        const string & key = I->first;
        if ((key == "name") || (key == "id") || (key == "parents")) continue;
        if ((key == "pos") || (key == "loc") || (key == "velocity")) continue;
        if ((key == "face") || (key == "contains")) continue;
        attributes[key] = I->second;
    }
}

void Entity::getLocation(Object::MapType & entmap, dict_t & objects)
{
    debug( cout << "Thing::getLocation" << endl << flush;);
    if (entmap.find("loc") != entmap.end()) {
        debug( cout << "Thing::getLocation, getting it" << endl << flush;);
        try {
            const string & ref_id = entmap["loc"].AsString();
            if (objects.find(ref_id) == objects.end()) {
                debug( cout << "ERROR: Can't get ref from objects dictionary" << endl << flush;);
                return;
            }
                
            location.ref = objects[ref_id];
            if (entmap.find("pos") != entmap.end()) {
                location.coords = Vector3D(entmap["pos"].AsList());
            }
            if (entmap.find("velocity") != entmap.end()) {
                location.velocity = Vector3D(entmap["velocity"].AsList());
            }
            if (entmap.find("face") != entmap.end()) {
                location.face = Vector3D(entmap["face"].AsList());
            } else if (!location.face) {
                location.face = Vector3D(1, 0, 0);
            }
            if (entmap.find("bbox") != entmap.end()) {
                location.bbox = Vector3D(entmap["bbox"].AsList());
            }
            if (entmap.find("bmedian") != entmap.end()) {
                location.bmedian = Vector3D(entmap["bmedian"].AsList());
            }
        }
        catch (Atlas::Message::WrongTypeException) {
            cerr << "ERROR: Create operation has bad location" << endl << flush;
        }
    }
}

oplist Entity::Operation(const Setup & op)
{
    oplist res;
    script->Operation("setup", op, res);
    return res;
}

oplist Entity::Operation(const Tick & op)
{
    oplist res;
    script->Operation("tick", op, res);
    return res;
}

oplist Entity::Operation(const Chop & op)
{
    oplist res;
    script->Operation("chop", op, res);
    return res;
}

oplist Entity::Operation(const Create & op)
{
    oplist res;
    script->Operation("create", op, res);
    return res;
}

oplist Entity::Operation(const Cut & op)
{
    oplist res;
    script->Operation("cut", op, res);
    return res;
}

oplist Entity::Operation(const Delete & op)
{
    oplist res;
    script->Operation("delete", op, res);
    return res;
}

oplist Entity::Operation(const Eat & op)
{
    oplist res;
    script->Operation("eat", op, res);
    return res;
}

oplist Entity::Operation(const Fire & op)
{
    oplist res;
    script->Operation("fire", op, res);
    return res;
}

oplist Entity::Operation(const Move & op)
{
    oplist res;
    script->Operation("move", op, res);
    return res;
}

oplist Entity::Operation(const Nourish & op)
{
    oplist res;
    script->Operation("nourish", op, res);
    return res;
}

oplist Entity::Operation(const Set & op)
{
    oplist res;
    script->Operation("set", op, res);
    return res;
}

oplist Entity::Operation(const Sight & op)
{
    oplist res;
    script->Operation("sight", op, res);
    return(res);
}

oplist Entity::Operation(const Sound & op)
{
    oplist res;
    script->Operation("sound", op, res);
    return(res);
}

oplist Entity::Operation(const Touch & op)
{
    oplist res;
    script->Operation("touch", op, res);
    return(res);
}

oplist Entity::Operation(const Look & op)
{
    oplist res;
    script->Operation("look", op, res);
    return res;
}

oplist Entity::Operation(const Appearance & op)
{
    oplist res;
    script->Operation("appearance", op, res);
    return(res);
}

oplist Entity::Operation(const Disappearance & op)
{
    oplist res;
    script->Operation("disappearance", op, res);
    return(res);
}

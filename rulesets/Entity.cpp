// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

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

Entity::Entity() : script(new Script), world(NULL), seq(0), status(1),
                   type("thing"), weight(-1),
                   isCharacter(false), deleted(false), omnipresent(false)
{
    inGame = true;
}

Entity::~Entity()
{
    if (script != NULL) {
        delete script;
    }
}

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
        for(elist_t::const_iterator I=contains.begin();I!=contains.end();I++) {
            contlist.push_back(*I);
        }
        attributes[aname] = Object(contlist);
    } else {
        Object::MapType::iterator I = attributes.find(aname);
        if (I == attributes.end()) {
            attributes[aname] = Object();
        }
    }
    return attributes[aname];
}

void Entity::set(const string & aname, const Object & attr)
{
    if ((aname == "status") && attr.IsNum()) {
        status = attr.AsNum();
    } else if (aname == "id") {
        return;
    } else if ((aname == "name") && attr.IsString()) {
        name = attr.AsString();
    } else if ((aname == "weight") && attr.IsNum()) {
        weight = attr.AsNum();
    } else {
        attributes[aname] = attr;
    }
}

void Entity::setScript(Script * scrpt)
{
    if (script != NULL) {
        delete script;
    }
    script = scrpt;
    return;
}

MemMap * Entity::getMap() {
    return NULL;
}

void Entity::destroy()
{
    if (deleted == true) {
        return;
    }
    for(elist_t::const_iterator I=contains.begin(); I != contains.end(); I++) {
        Entity * obj = *I;
        if (obj->deleted == false) {
            obj->location.ref = location.ref;
            obj->location.coords = location.coords + obj->location.coords;
        }
    }
    if (location) {
        location.ref->contains.remove(this);
    }
}

void Entity::addToObject(Object & obj) const
{
    Object::MapType & omap = obj.AsMap();
    // We need to have a list of keys to pull from attributes.
    Object::MapType::const_iterator I = attributes.begin();
    for (; I != attributes.end(); I++) {
        omap[I->first] = I->second;
    }
    if (!name.empty()) {
        omap["name"] = name;
    }
    omap["type"] = type;
    omap["weight"] = weight;
    omap["status"] = status;
    omap["stamp"] = (double)seq;
    omap["parents"] = Object(Object::ListType(1,Object(type)));
    location.addToObject(obj);
    Object::ListType contlist;
    for(elist_t::const_iterator I = contains.begin(); I!=contains.end(); I++) {
        contlist.push_back(Object((*I)->fullid));
    }
    if (contlist.size() != 0) {
        omap["contains"] = Object(contlist);
    }
    BaseEntity::addToObject(obj);
}

void Entity::merge(const Object::MapType & entmap)
{
    for (Object::MapType::const_iterator I=entmap.begin();I!=entmap.end();I++) {
        const string & key = I->first;
        if ((key == "parents")||(key == "bbox") || (key == "bmedian")) continue;
        if ((key == "pos") || (key == "loc") || (key == "velocity")) continue;
        if ((key == "face") || (key == "contains")) continue;
        set(key, I->second);
    }
}

void Entity::getLocation(const Object::MapType & entmap, edict_t & eobjects)
{
    debug( cout << "Thing::getLocation" << endl << flush;);
    Object::MapType::const_iterator I = entmap.find("loc");
    if (I == entmap.end()) { return; }
    debug( cout << "Thing::getLocation, getting it" << endl << flush;);
    try {
        const string & ref_id = I->second.AsString();
        edict_t::const_iterator J = eobjects.find(ref_id);
        if (J == eobjects.end()) {
            debug( cout << "ERROR: Can't get ref from objects dictionary" << endl << flush;);
            return;
        }
            
        location.ref = J->second;
        I = entmap.find("pos");
        if (I != entmap.end()) {
            location.coords = Vector3D(I->second.AsList());
        }
        I = entmap.find("velocity");
        if (I != entmap.end()) {
            location.velocity = Vector3D(I->second.AsList());
        }
        I = entmap.find("face");
        if (I != entmap.end()) {
            location.face = Vector3D(I->second.AsList());
        } else if (!location.face) {
            location.face = Vector3D(1, 0, 0);
        }
        I = entmap.find("bbox");
        if (I != entmap.end()) {
            location.bbox = Vector3D(I->second.AsList());
        }
        I = entmap.find("bmedian");
        if (I != entmap.end()) {
            location.bmedian = Vector3D(I->second.AsList());
        }
    }
    catch (Atlas::Message::WrongTypeException) {
        cerr << "ERROR: Create operation has bad location" << endl << flush;
    }
}

Vector3D Entity::getXyz() const
{
    //Location l=location;
    if (!location) {
        static Vector3D ret(0.0,0.0,0.0);
        return ret;
    }
    if (location.ref) {
        return location.coords+location.ref->getXyz();
    } else {
        return location.coords;
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

oplist Entity::Operation(const Action & op)
{
    oplist res;
    script->Operation("action", op, res);
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
    return res;
}

oplist Entity::Operation(const Sound & op)
{
    oplist res;
    script->Operation("sound", op, res);
    return res;
}

oplist Entity::Operation(const Touch & op)
{
    oplist res;
    script->Operation("touch", op, res);
    return res;
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
    return res;
}

oplist Entity::Operation(const Disappearance & op)
{
    oplist res;
    script->Operation("disappearance", op, res);
    return res;
}

oplist Entity::Operation(const RootOperation & op)
{
    const string & op_type = op.GetParents().front().AsString();
    oplist res;
    cout << "Entity " << fullid << " got custom " << op_type << " op" << endl << flush;
    script->Operation(op_type, op, res);
    return res;
}

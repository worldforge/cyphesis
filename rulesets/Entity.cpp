// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include <Atlas/Objects/Operation/Create.h>
#include <Atlas/Objects/Operation/Sight.h>
#include <Atlas/Objects/Operation/Set.h>
#include <Atlas/Objects/Operation/Delete.h>
#include <Atlas/Objects/Operation/Imaginary.h>
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

#include <common/debug.h>

static const bool debug_flag = false;

using Atlas::Message::Object;

std::set<std::string> Entity::m_immutable;

const std::set<std::string> & Entity::immutables()
{
    if (m_immutable.empty()) {
        m_immutable.insert("parents");
        m_immutable.insert("pos");
        m_immutable.insert("loc");
        m_immutable.insert("velocity");
        m_immutable.insert("orientation");
        m_immutable.insert("contains");
    }
    return m_immutable;
}

Entity::Entity() : script(new Script), seq(0), status(1),
                   type("thing"), mass(-1),
                   deleted(false), omnipresent(false), perceptive(false),
                   world(NULL)
{
}

Entity::~Entity()
{
    if (script != NULL) {
        delete script;
    }
}

const Object & Entity::operator[](const std::string & aname)
{
    if (aname == "status") {
        attributes[aname] = Object(status);
    } else if (aname == "id") {
        attributes[aname] = Object(getId());
    } else if (aname == "name") {
        attributes[aname] = Object(name);
    } else if (aname == "mass") {
        attributes[aname] = Object(mass);
    } else if (aname == "bbox") {
        attributes[aname] = location.bBox.asList();
    } else if (aname == "contains") {
        Object::ListType contlist;
        for(EntitySet::const_iterator I=contains.begin();I!=contains.end();I++){
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

void Entity::set(const std::string & aname, const Object & attr)
{
    if ((aname == "status") && attr.IsNum()) {
        status = attr.AsNum();
    } else if (aname == "id") {
        return;
    } else if ((aname == "name") && attr.IsString()) {
        name = attr.AsString();
    } else if ((aname == "mass") && attr.IsNum()) {
        mass = attr.AsNum();
    } else if ((aname=="bbox") && attr.IsList() && (attr.AsList().size()>2)) {
        location.bBox = BBox(attr.AsList());
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

void Entity::destroy()
{
    assert(!deleted);
    if (deleted) {
        return;
    }
    assert(location.ref != NULL);
    EntitySet & refContains = location.ref->contains;
    for(EntitySet::const_iterator I=contains.begin(); I != contains.end(); I++){
        Entity * obj = *I;
        if (!obj->deleted) {
            obj->location.ref = location.ref;
            obj->location.coords += location.coords;
            refContains.insert(obj);
        }
    }
    refContains.erase(this);
    deleted = true;
}

void Entity::addToObject(Object::MapType & omap) const
{
    // We need to have a list of keys to pull from attributes.
    Object::MapType::const_iterator I = attributes.begin();
    for (; I != attributes.end(); I++) {
        omap[I->first] = I->second;
    }
    if (!name.empty()) {
        omap["name"] = name;
    }
    omap["type"] = type;
    omap["mass"] = mass;
    omap["status"] = status;
    omap["stamp"] = (double)seq;
    omap["parents"] = Object(Object::ListType(1,Object(type)));
    location.addToObject(omap);
    Object::ListType contlist;
    for(EntitySet::const_iterator I = contains.begin(); I!=contains.end(); I++){
        contlist.push_back(Object((*I)->getId()));
    }
    if (!contlist.empty()) {
        omap["contains"] = Object(contlist);
    }
    BaseEntity::addToObject(omap);
}

void Entity::merge(const Object::MapType & entmap)
{
    const std::set<std::string> & imm = immutables();
    for (Object::MapType::const_iterator I=entmap.begin();I!=entmap.end();I++) {
        const std::string & key = I->first;
        if (imm.find(key) != imm.end()) continue;
        set(key, I->second);
    }
}

void Entity::getLocation(const Object::MapType & entmap,
                         const EntityDict & eobjects)
{
    debug( std::cout << "Thing::getLocation" << std::endl << std::flush;);
    Object::MapType::const_iterator I = entmap.find("loc");
    if ((I == entmap.end()) || !I->second.IsString()) {
        debug( std::cout << getId() << ".. has no loc" << std::endl << std::flush;);
        return;
    }
    try {
        const std::string & ref_id = I->second.AsString();
        EntityDict::const_iterator J = eobjects.find(ref_id);
        if (J == eobjects.end()) {
            debug( std::cout << "ERROR: Can't get ref from objects dictionary" << std::endl << std::flush;);
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
        I = entmap.find("orientation");
        if (I != entmap.end()) {
            location.orientation = Quaternion(I->second.AsList());
        }
        I = entmap.find("bbox");
        if (I != entmap.end()) {
            location.bBox = BBox(I->second.AsList());
        }
    }
    catch (Atlas::Message::WrongTypeException) {
        std::cerr << "ERROR: Bad location data" << std::endl << std::flush;
    }
}

Vector3D Entity::getXyz() const
{
    //Location l=location;
    if (!location.isValid()) {
        static Vector3D ret(0.0,0.0,0.0);
        return ret;
    }
    if (location.ref) {
        return Vector3D(location.coords) += location.ref->getXyz();
    } else {
        return location.coords;
    }
}

OpVector Entity::SetupOperation(const Setup & op)
{
    OpVector res;
    script->Operation("setup", op, res);
    return res;
}

OpVector Entity::TickOperation(const Tick & op)
{
    OpVector res;
    script->Operation("tick", op, res);
    return res;
}

OpVector Entity::ActionOperation(const Action & op)
{
    OpVector res;
    script->Operation("action", op, res);
    return res;
}

OpVector Entity::ChopOperation(const Chop & op)
{
    OpVector res;
    script->Operation("chop", op, res);
    return res;
}

OpVector Entity::CreateOperation(const Create & op)
{
    OpVector res;
    script->Operation("create", op, res);
    return res;
}

OpVector Entity::CutOperation(const Cut & op)
{
    OpVector res;
    script->Operation("cut", op, res);
    return res;
}

OpVector Entity::DeleteOperation(const Delete & op)
{
    OpVector res;
    script->Operation("delete", op, res);
    return res;
}

OpVector Entity::EatOperation(const Eat & op)
{
    OpVector res;
    script->Operation("eat", op, res);
    return res;
}

OpVector Entity::FireOperation(const Fire & op)
{
    OpVector res;
    script->Operation("fire", op, res);
    return res;
}

OpVector Entity::ImaginaryOperation(const Imaginary & op)
{
    OpVector res;
    script->Operation("imaginary", op, res);
    return res;
}

OpVector Entity::MoveOperation(const Move & op)
{
    OpVector res;
    script->Operation("move", op, res);
    return res;
}

OpVector Entity::NourishOperation(const Nourish & op)
{
    OpVector res;
    script->Operation("nourish", op, res);
    return res;
}

OpVector Entity::SetOperation(const Set & op)
{
    OpVector res;
    script->Operation("set", op, res);
    return res;
}

OpVector Entity::SightOperation(const Sight & op)
{
    OpVector res;
    script->Operation("sight", op, res);
    return res;
}

OpVector Entity::SoundOperation(const Sound & op)
{
    OpVector res;
    script->Operation("sound", op, res);
    return res;
}

OpVector Entity::TouchOperation(const Touch & op)
{
    OpVector res;
    script->Operation("touch", op, res);
    return res;
}

OpVector Entity::LookOperation(const Look & op)
{
    OpVector res;
    script->Operation("look", op, res);
    return res;
}

OpVector Entity::AppearanceOperation(const Appearance & op)
{
    OpVector res;
    script->Operation("appearance", op, res);
    return res;
}

OpVector Entity::DisappearanceOperation(const Disappearance & op)
{
    OpVector res;
    script->Operation("disappearance", op, res);
    return res;
}

OpVector Entity::OtherOperation(const RootOperation & op)
{
    const std::string & op_type = op.GetParents().front().AsString();
    OpVector res;
    debug(std::cout << "Entity " << getId() << " got custom " << op_type << " op"
               << std::endl << std::flush;);
    script->Operation(op_type, op, res);
    return res;
}

// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "MemEntity.h"

#include "common/log.h"
#include "common/debug.h"
#include "common/types.h"
#include "common/inheritance.h"

#include <wfmath/atlasconv.h>

#include <cassert>

static const bool debug_flag = false;

MemEntity::MemEntity(const std::string & id) : m_id(id)
{
}

MemEntity::~MemEntity()
{
}

bool MemEntity::get(const std::string & aname, Element & attr) const
{
    if (aname == "id") {
        attr = getId();
        return true;
    } else if (aname == "type") {
        attr = m_type;
        return true;
    } else if (aname == "bbox") {
        attr = m_location.m_bBox.toAtlas();
        return true;
    } else if (aname == "contains") {
        attr = Element::ListType();
        Element::ListType & contlist = attr.asList();
        for(MemEntitySet::const_iterator I = m_contains.begin();
            I != m_contains.end(); I++) {
            contlist.push_back(*I);
        }
        return true;
    } else {
        Element::MapType::const_iterator I = m_attributes.find(aname);
        if (I != m_attributes.end()) {
            attr = I->second;
            return true;
        } else {
            return false;
        }
    }
}

void MemEntity::set(const std::string & aname, const Element & attr)
{
    if (aname == "id") {
        return;
    } else if ((aname == "type") && attr.isString()) {
        m_type = attr.asString();
    } else if ((aname == "bbox") && attr.isList() &&
               (attr.asList().size() > 2)) {
        m_location.m_bBox.fromAtlas(attr.asList());
    } else {
        m_attributes[aname] = attr;
    }
}

void MemEntity::addToObject(Element::MapType & omap) const
{
    // We need to have a list of keys to pull from attributes.
    Element::MapType::const_iterator I = m_attributes.begin();
    for (; I != m_attributes.end(); I++) {
        omap[I->first] = I->second;
    }
    omap["type"] = m_type;
    m_location.addToObject(omap);
    Element::ListType contlist;
    MemEntitySet::const_iterator J = m_contains.begin();
    for(; J != m_contains.end(); J++) {
        contlist.push_back((*J)->getId());
    }
    if (!contlist.empty()) {
        omap["contains"] = contlist;
    }
    omap["objtype"] = "obj";
    omap["id"] = getId();
}

void MemEntity::merge(const Element::MapType & ent)
{
    for(Element::MapType::const_iterator I = ent.begin(); I != ent.end(); I++){
        const std::string & key = I->first;
        set(key, I->second);
    }
}

bool MemEntity::getLocation(const Element::MapType & entmap,
                         const MemEntityDict & eobjects)
{
    debug( std::cout << "MemEntity::getLocation" << std::endl << std::flush;);
    Element::MapType::const_iterator I = entmap.find("loc");
    if ((I == entmap.end()) || !I->second.isString()) {
        debug( std::cout << getId() << ".. has no loc" << std::endl << std::flush;);
        return true;
    }
    try {
        const std::string & ref_id = I->second.asString();
        MemEntityDict::const_iterator J = eobjects.find(ref_id);
        if (J == eobjects.end()) {
            debug( std::cout << "ERROR: Can't get ref from objects dictionary" << std::endl << std::flush;);
            return true;
        }
            
        m_location.m_loc = J->second;
        I = entmap.find("pos");
        if (I != entmap.end()) {
            m_location.m_pos.fromAtlas(I->second.asList());
        }
        I = entmap.find("velocity");
        if (I != entmap.end()) {
            m_location.m_velocity.fromAtlas(I->second.asList());
        }
        I = entmap.find("orientation");
        if (I != entmap.end()) {
            m_location.m_orientation.fromAtlas(I->second.asList());
        }
        I = entmap.find("bbox");
        if (I != entmap.end()) {
            m_location.m_bBox.fromAtlas(I->second.asList());
        }
    }
    catch (Atlas::Message::WrongTypeException) {
        log(ERROR, "getLocation: Bad location data");
        return true;
    }
    return false;
}

Vector3D MemEntity::getXyz() const
{
    //Location l=location;
    if (!m_location.isValid()) {
        static Vector3D ret(0.0,0.0,0.0);
        return ret;
    }
    if (m_location.m_loc) {
        return Vector3D(m_location.m_pos) + m_location.m_loc->getXyz();
    } else {
        return m_location.m_pos;
    }
}

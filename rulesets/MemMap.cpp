// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "Entity.h"
#include "MemMap.h"
#include "Script.h"

#include "modules/Location.h"

#include "common/debug.h"

#include <Atlas/Objects/Operation/Look.h>

static const bool debug_flag = false;

Entity * MemMap::addObject(Entity * object)
{
    debug(std::cout << "MemMap::addObject " << object << " " << object->getId()
                    << std::endl << std::flush;);
    m_entities[object->getId()] = object;

    debug( std::cout << this << std::endl << std::flush;);
    std::vector<std::string>::const_iterator I;
    for(I = m_addHooks.begin(); I != m_addHooks.end(); I++) {
        m_script->hook(*I, object);
    }
    return object;
}

RootOperation * MemMap::lookId()
{
    debug( std::cout << "MemMap::lookId" << std::endl << std::flush;);
    if (!m_additionsById.empty()) {
        const std::string & id = m_additionsById.front();
        Look * l = new Look();
        Atlas::Message::Element::MapType m;
        m["id"] = Atlas::Message::Element(id);
        l->setArgs(Atlas::Message::Element::ListType(1, m));
        l->setTo(id);
        m_additionsById.pop_front();
        return l;
    }
    return NULL;
}

Entity * MemMap::addId(const std::string & id)
{
    debug( std::cout << "MemMap::add_id" << std::endl << std::flush;);
    assert(m_entities.find(id) == m_entities.end());
    m_additionsById.push_back(id);
    Entity * entity = new Entity(id);
    return addObject(entity);
    // Atlas::Message::Element::MapType m;
    // m["id"] = Atlas::Message::Element(id);
    // return add(m);
}

void MemMap::del(const std::string & id)
{
    EntityDict::iterator I = m_entities.find(id);
    if (I != m_entities.end()) {
        Entity * obj = I->second;
        m_entities.erase(I);
        std::vector<std::string>::const_iterator J;
        for(J = m_deleteHooks.begin(); J != m_deleteHooks.end(); J++) {
            m_script->hook(*J, obj);
        }
        delete obj;
    }
}

Entity * MemMap::get(const std::string & id)
{
    debug( std::cout << "MemMap::get" << std::endl << std::flush;);
    if (id.empty()) { return NULL; }
    EntityDict::const_iterator I = m_entities.find(id);
    if (I != m_entities.end()) {
        return I->second;
    }
    return NULL;
}

Entity * MemMap::getAdd(const std::string & id)
{
    debug( std::cout << "MemMap::getAdd(" << id << ")" << std::endl << std::flush;);
    if (id.empty()) { return NULL; }
    EntityDict::const_iterator I = m_entities.find(id);
    if (I != m_entities.end()) {
        return I->second;
    }
    return addId(id);
}

void MemMap::addContents(const Element::MapType & entmap)
{
    Element::MapType::const_iterator I = entmap.find("contains");
    if ((I == entmap.end()) || (!I->second.isList())) {
        return;
    }
    const Element::ListType & contlist = I->second.asList();
    Element::ListType::const_iterator J = contlist.begin();
    for(;J != contlist.end(); J++) {
        if (!J->isString()) {
            continue;
        }
        getAdd(J->asString());
    }
}

Entity * MemMap::add(const Element::MapType & entmap)
{
    debug( std::cout << "MemMap::add" << std::endl << std::flush;);
    Element::MapType::const_iterator I = entmap.find("id");
    if ((I == entmap.end()) || (I->second.asString().empty())) {
        return NULL;
    }
    const std::string & id = I->second.asString();
    if (find(id)) {
        return update(entmap);
    }
    Entity * entity = new Entity(id);
    // entity->setId(id);
    I = entmap.find("name");
    if ((I != entmap.end()) && I->second.isString()) {
        entity->setName(I->second.asString());
    }
    I = entmap.find("type");
    if ((I != entmap.end()) && I->second.isString()) {
        entity->setType(I->second.asString());
    }
    entity->merge(entmap);
    I = entmap.find("loc");
    if ((I != entmap.end()) && I->second.isString()) {
        getAdd(I->second.asString());
    }
    entity->getLocation(entmap, m_entities);
    addContents(entmap);
    return addObject(entity);
}

Entity * MemMap::update(const Element::MapType & entmap)
{
    debug( std::cout << "MemMap::update" << std::endl << std::flush;);
    Element::MapType::const_iterator I = entmap.find("id");
    if ((I == entmap.end()) || !I->second.isString()) {
        return NULL;
    }
    const std::string & id = I->second.asString();
    if (id.empty()) {
        return NULL;
    }
    debug( std::cout << " updating " << id << std::endl << std::flush;);
    EntityDict::const_iterator J = m_entities.find(id);
    if (J == m_entities.end()) {
        return add(entmap);
    }
    debug( std::cout << " " << id << " has already been spotted" << std::endl << std::flush;);
    Entity * entity = J->second;
    debug( std::cout << " got " << entity << std::endl << std::flush;);
    I = entmap.find("name");
    if (I != entmap.end() && I->second.isString()) {
        entity->setName(I->second.asString());
    }
    I = entmap.find("type");
    if (I != entmap.end() && I->second.isString()) {
        entity->setType(I->second.asString());
    }
    // It is important that the entity is not mutated here. Ie, an update
    // should not affect its type, contain or id, and location and
    // stamp should be updated with accurate information
    entity->merge(entmap);
    I = entmap.find("loc");
    if ((I != entmap.end()) && I->second.isString()) {
        getAdd(I->second.asString());
    }
    entity->getLocation(entmap,m_entities);
    addContents(entmap);
    std::vector<std::string>::const_iterator K;
    for(K = m_updateHooks.begin(); K != m_updateHooks.end(); K++) {
        m_script->hook(*K, entity);
    }
    return entity;
}

EntityVector MemMap::findByType(const std::string & what)
{
    EntityVector res;
    EntityDict::const_iterator I;
    for(I = m_entities.begin(); I != m_entities.end(); I++) {
        Entity * item = I->second;
        debug( std::cout << "F" << what << ":" << item->getType() << ":" << item->getId() << std::endl << std::flush;);
        if (item->getType() == what) {
            res.push_back(I->second);
        }
    }
    return res;
}

EntityVector MemMap::findByLocation(const Location & loc, double radius)
{
    EntityVector res;
    EntityDict::const_iterator I;
    for(I = m_entities.begin(); I != m_entities.end(); I++) {
        const Location & oloc = I->second->m_location;
        if (!loc.isValid() || !oloc.isValid()) {
            continue;
        }
        if ((oloc.m_loc->getId() == loc.m_loc->getId()) &&
            (squareDistance(loc.m_pos, oloc.m_pos) < (radius * radius))) {
            res.push_back(I->second);
        }
    }
    return res;
}

const Element MemMap::asObject()
{
    Element::MapType omap;
    EntityDict::const_iterator I = m_entities.begin();
    for(;I != m_entities.end(); I++) {
        I->second->addToObject((omap[I->first] = Element(Element::MapType())).asMap());
    }
    return Element(omap);
}

void MemMap::flushMap()
{
    EntityDict::const_iterator I = m_entities.begin();
    for (; I != m_entities.end(); I++) {
        delete I->second;
    }
}

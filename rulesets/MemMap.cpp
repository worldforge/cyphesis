// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "MemEntity.h"
#include "MemMap.h"
#include "Script.h"

#include "common/log.h"
#include "common/debug.h"

#include <Atlas/Objects/Operation/Look.h>

#include <sstream>

static const bool debug_flag = false;

// This is a template which requires debug flag to be declared.
#include "Entity_getLocation.h"

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Message::ListType;
using Atlas::Objects::Operation::Look;

MemEntity * MemMap::addEntity(MemEntity * entity)
{
    assert(entity != 0);
    assert(!entity->getId().empty());

    debug(std::cout << "MemMap::addEntity " << entity << " " << entity->getId()
                    << std::endl << std::flush;);
    std::string next;
    if (m_checkIterator != m_entities.end()) {
        next = m_checkIterator->first;
    }
    m_entities[entity->getId()] = entity;
    m_checkIterator = m_entities.find(next);

    debug( std::cout << this << std::endl << std::flush;);
    std::vector<std::string>::const_iterator I = m_addHooks.begin();
    std::vector<std::string>::const_iterator Iend = m_addHooks.end();
    for (; I != Iend; ++I) {
        m_script->hook(*I, entity);
    }
    return entity;
}

void MemMap::readEntity(MemEntity * entity, const MapType & entmap)
// Read the contents of an Atlas message into an entity
{
    MapType::const_iterator I = entmap.find("name");
    if (I != entmap.end() && I->second.isString()) {
        entity->setName(I->second.asString());
    }
    I = entmap.find("parents");
    if (I != entmap.end() && I->second.isList()) {
        const ListType & parents = I->second.asList();
        if (!parents.empty() && parents.front().isString()) {
            entity->setType(parents.front().asString());
        }
    }
    entity->merge(entmap);
    I = entmap.find("loc");
    if ((I != entmap.end()) && I->second.isString()) {
        getAdd(I->second.asString());
    }
    Entity * old_loc = entity->m_location.m_loc;
    entity->getLocation(entmap, m_entities);
    if (old_loc != entity->m_location.m_loc) {
        if (old_loc != 0) {
            old_loc->m_contains.erase(entity);
        }
        entity->m_location.m_loc->m_contains.insert(entity);
    }
    addContents(entmap);
}

void MemMap::updateEntity(MemEntity * entity, const MapType & entmap)
// Update contents of entity an Atlas message.
{
    assert(entity != 0);

    debug( std::cout << " got " << entity << std::endl << std::flush;);

    readEntity(entity, entmap);

    std::vector<std::string>::const_iterator K = m_updateHooks.begin();
    std::vector<std::string>::const_iterator Kend = m_updateHooks.end();
    for (; K != Kend; ++K) {
        m_script->hook(*K, entity);
    }
}

MemEntity * MemMap::newEntity(const std::string & id,
                              const MapType & entmap)
// Create a new entity from an Atlas message.
{
    assert(!id.empty());
    assert(m_entities.find(id) == m_entities.end());

    MemEntity * entity = new MemEntity(id);

    readEntity(entity, entmap);

    return addEntity(entity);
}

MemMap::MemMap(Script *& s) : m_checkIterator(m_entities.begin()), m_script(s)
{
}

RootOperation * MemMap::lookId()
// Generate a look operation to look at an entity we are interested in
{
    debug( std::cout << "MemMap::lookId" << std::endl << std::flush;);
    if (!m_additionsById.empty()) {
        const std::string & id = m_additionsById.front();
        Look * l = new Look();
        MapType m;
        m["id"] = id;
        l->setArgs(ListType(1, m));
        m_additionsById.pop_front();
        return l;
    }
    return NULL;
}

MemEntity * MemMap::addId(const std::string & id)
// Queue the ID of an entity we are interested in
{
    assert(!id.empty());
    assert(m_entities.find(id) == m_entities.end());

    debug( std::cout << "MemMap::add_id" << std::endl << std::flush;);
    m_additionsById.push_back(id);
    MemEntity * entity = new MemEntity(id);
    return addEntity(entity);
}

void MemMap::del(const std::string & id)
// Delete an entity from memory
{
    debug( std::cout << "MemMap::del(" << id << ")" << std::endl << std::flush;);
    MemEntityDict::iterator I = m_entities.find(id);
    if (I != m_entities.end()) {
        MemEntity * ent = I->second;
        assert(ent != 0);
        std::string next;
        if (m_checkIterator != m_entities.end()) {
            next = m_checkIterator->first;
        }
        m_entities.erase(I);

        // Handling re-parenting is done very similarly to Entity::destroy,
        // but is slightly different as we tolerate LOC being null.
        Entity * mloc = ent->m_location.m_loc;
        if (mloc != 0) {
            // Remove deleted entity from its parents contains
            mloc->m_contains.erase(ent);
        }

        // Add deleted entities children into its parents contains
        EntitySet::const_iterator K = ent->m_contains.begin();
        EntitySet::const_iterator Kend = ent->m_contains.end();
        for (; K != Kend; ++K) {
            Entity * cent = *K;
            cent->m_location.m_loc = mloc;
            // FIXME adjust pos and:
            // FIXME take account of orientation
            if (mloc != 0) {
                mloc->m_contains.insert(cent);
            }
        }

        m_checkIterator = m_entities.find(next);
        std::vector<std::string>::const_iterator J = m_deleteHooks.begin();
        std::vector<std::string>::const_iterator Jend = m_deleteHooks.end();
        for(; J != Jend; ++J) {
            m_script->hook(*J, ent);
        }
        delete ent;
    }
}

MemEntity * MemMap::get(const std::string & id) const
// Get an entity from memory
{
    debug( std::cout << "MemMap::get" << std::endl << std::flush;);
    if (id.empty()) {
        // This shouldn't really occur, and shouldn't be a problem
        log(ERROR, "MemMap::get queried for empty id string");
        return NULL;
    }
    MemEntityDict::const_iterator I = m_entities.find(id);
    if (I != m_entities.end()) {
        assert(I->second != 0);
        return I->second;
    }
    return NULL;
}

MemEntity * MemMap::getAdd(const std::string & id)
// Get an entity from memory, or add it if we haven't seen it yet
// This could be implemented by calling get() for all but the the last line
{
    debug( std::cout << "MemMap::getAdd(" << id << ")" << std::endl << std::flush;);
    if (id.empty()) {
        return NULL;
    }
    MemEntityDict::const_iterator I = m_entities.find(id);
    if (I != m_entities.end()) {
        assert(I->second != 0);
        return I->second;
    }
    return addId(id);
}

void MemMap::addContents(const MapType & entmap)
// Iterate over the contains attribute of a message, looking at all the contents
{
    MapType::const_iterator I = entmap.find("contains");
    if (I == entmap.end()) {
        return;
    }
    if (!I->second.isList()) {
        log(ERROR, "MemMap::addContents, malformed contains is not list");
        return;
    }
    const ListType & contlist = I->second.asList();
    ListType::const_iterator Jend = contlist.end();
    for (ListType::const_iterator J = contlist.begin(); J != Jend; ++J) {
        if (!J->isString()) {
            log(ERROR, "MemMap::addContents, malformed non-string in contains");
            continue;
        }
        getAdd(J->asString());
    }
}

MemEntity * MemMap::updateAdd(const MapType & entmap, const double & d)
// Update an entity in our memory, from an Atlas message
// The mind code relies on this function never sending a Sight to
// be sure that seeing something created does not imply that the created
// entity is visible, as we may have received it because we can see the
// creator.
{
    debug( std::cout << "MemMap::update" << std::endl << std::flush;);
    MapType::const_iterator I = entmap.find("id");
    if (I == entmap.end()) {
        log(ERROR, "MemMap::update, Missing id in updated entity");
        return NULL;
    }
    if (!I->second.isString()) {
        log(ERROR, "MemMap::update, Malformed non-string id in updated entity");
        return NULL;
    }
    const std::string & id = I->second.asString();
    if (id.empty()) {
        log(ERROR, "MemMap::update, Empty id in updated entity");
        return NULL;
    }
    MemEntityDict::const_iterator J = m_entities.find(id);
    MemEntity * entity;
    if (J == m_entities.end()) {
        entity = newEntity(id, entmap);
    } else {
        entity = J->second;
        updateEntity(entity, entmap);
    }
    entity->update(d);
    return entity;
}

MemEntityVector MemMap::findByType(const std::string & what)
// Find an entity in our memory of a certain type
{
    MemEntityVector res;
    
    MemEntityDict::const_iterator Iend = m_entities.end();
    for (MemEntityDict::const_iterator I = m_entities.begin(); I != Iend; ++I) {
        MemEntity * item = I->second;
        debug( std::cout << "F" << what << ":" << item->getType() << ":" << item->getId() << std::endl << std::flush;);
        if (item->isVisible() && item->getType() == what) {
            res.push_back(I->second);
        }
    }
    return res;
}

MemEntityVector MemMap::findByLocation(const Location & loc, double radius)
// Find an entity in our memory in a certain place
// FIXME Don't return by value
{
    MemEntityVector res;
    MemEntityDict::const_iterator Iend = m_entities.end();
    for (MemEntityDict::const_iterator I = m_entities.begin(); I != Iend; ++I) {
        MemEntity * item = I->second;
        if (!item->isVisible()) {
            continue;
        }
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
    MapType omap;
    
    MemEntityDict::const_iterator Iend = m_entities.end();
    for (MemEntityDict::const_iterator I = m_entities.begin(); I != Iend; ++I) {
        I->second->addToMessage((omap[I->first] = MapType()).asMap());
    }
    return Element(omap);
}

void MemMap::check(const double & time)
{
    if (m_checkIterator == m_entities.end()) {
        m_checkIterator = m_entities.begin();
    } else {
        MemEntity * me = m_checkIterator->second;
        assert(me != 0);
        if (!me->isVisible() && ((time - me->lastSeen()) > 600) && (me->m_contains.empty())) {
            debug(std::cout << me->getId() << "|" << me->getType()
                      << " is a waste of space" << std::endl << std::flush;);
            MemEntityDict::const_iterator J = m_checkIterator;
            std::string next;
            if (++J != m_entities.end()) {
                next = J->first;
            }
            m_entities.erase(m_checkIterator);
            // Remove deleted entity from its parents contains attribute
            if (me->m_location.m_loc != 0) {
                me->m_location.m_loc->m_contains.erase(me);
            }
            // m_checkIterator = m_entities.begin();
            m_checkIterator = m_entities.find(next);
            // attribute of its its parent.
            delete me;
        } else {
            debug(std::cout << me->getId() << "|" << me->getType() << "|"
                            << me->lastSeen() << "|" << me->isVisible()
                            << " is fine" << std::endl << std::flush;);
            ++m_checkIterator;
        }
    }
}

void MemMap::flush()
{
    debug(std::cout << "Flushing memory with " << m_entities.size()
                    << " memories" << std::endl << std::flush;);
    
    MemEntityDict::const_iterator Iend = m_entities.end();
    for (MemEntityDict::const_iterator I = m_entities.begin(); I != Iend; ++I) {
        delete I->second;
    }
}

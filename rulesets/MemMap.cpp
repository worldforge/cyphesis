// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000-2005 Alistair Riddoch
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA


#include "MemMap.h"

#include "MemEntity.h"
#include "Script.h"

#include "common/id.h"
#include "common/log.h"
#include "common/debug.h"
#include "common/TypeNode.h"
#include "common/compose.hpp"
#include "common/Inheritance.h"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>

#include <sstream>

static const bool debug_flag = false;

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Objects::Operation::Look;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Entity::Anonymous;

using String::compose;

const TypeNode * MemMap::m_entity_type = 0;

MemEntity * MemMap::addEntity(MemEntity * entity)
{
    assert(entity != 0);
    assert(!entity->getId().empty());
    assert(entity->getType() != 0);

    debug(std::cout << "MemMap::addEntity " << entity << " " << entity->getId()
                    << std::endl << std::flush;);
    long next = -1;
    if (m_checkIterator != m_entities.end()) {
        next = m_checkIterator->first;
    }
    m_entities[entity->getIntId()] = entity;
    m_checkIterator = m_entities.find(next);

    if (m_script != 0) {
        debug( std::cout << this << std::endl << std::flush;);
        std::vector<std::string>::const_iterator I = m_addHooks.begin();
        std::vector<std::string>::const_iterator Iend = m_addHooks.end();
        for (; I != Iend; ++I) {
            m_script->hook(*I, entity);
        }
    }
    return entity;
}

void MemMap::readEntity(MemEntity * entity, const RootEntity & ent)
// Read the contents of an Atlas message into an entity
{
    if (ent->hasAttrFlag(Atlas::Objects::PARENTS_FLAG)) {
        const std::list<std::string> & parents = ent->getParents();
        if (!parents.empty()) {
            if (entity->getType() == m_entity_type) {
                const TypeNode * type = Inheritance::instance().getType(parents.front());
                if (type != 0) {
                    entity->setType(type);
                }
            } else if (entity->getType()->name() != parents.front()) {
                debug(std::cout << "Attempting to mutate " << entity->getType()
                                << " into " << parents.front()
                                << std::endl << std::flush;);
            }
        }
    }
    entity->merge(ent->asMessage());
    if (ent->hasAttrFlag(Atlas::Objects::Entity::LOC_FLAG)) {
        LocatedEntity * old_loc = entity->m_location.m_loc;
        const std::string & new_loc_id = ent->getLoc();
        // Has LOC been changed?
        if (old_loc == 0 || new_loc_id != old_loc->getId()) {
            entity->m_location.m_loc = getAdd(new_loc_id);
            assert(0 != entity->m_location.m_loc);
            assert(old_loc != entity->m_location.m_loc);
            if (old_loc != 0) {
                assert(old_loc->m_contains != 0);
                old_loc->m_contains->erase(entity);
            }
            if (entity->m_location.m_loc->m_contains == 0) {
                entity->m_location.m_loc->m_contains = new LocatedEntitySet;
            }
            entity->m_location.m_loc->m_contains->insert(entity);
        }
        entity->m_location.readFromEntity(ent);
    }
    addContents(ent);
}

void MemMap::updateEntity(MemEntity * entity, const RootEntity & ent)
// Update contents of entity an Atlas message.
{
    assert(entity != 0);

    debug( std::cout << " got " << entity << std::endl << std::flush;);

    readEntity(entity, ent);

    if (m_script != 0) {
        std::vector<std::string>::const_iterator K = m_updateHooks.begin();
        std::vector<std::string>::const_iterator Kend = m_updateHooks.end();
        for (; K != Kend; ++K) {
            m_script->hook(*K, entity);
        }
    }
}

MemEntity * MemMap::newEntity(const std::string & id, long int_id,
                              const RootEntity & ent)
// Create a new entity from an Atlas message.
{
    assert(m_entities.find(int_id) == m_entities.end());

    MemEntity * entity = new MemEntity(id, int_id);
    entity->setType(m_entity_type);

    readEntity(entity, ent);

    return addEntity(entity);
}

MemMap::MemMap(Script *& s) : m_checkIterator(m_entities.begin()), m_script(s)
{
    if (m_entity_type == 0) {
        // m_entity_type = Inheritance::instance().getType("game_entity");
        // FIXME What to do with this?
        m_entity_type = new TypeNode("");
        assert(m_entity_type != 0);
    }
}

void MemMap::sendLooks(OpVector & res)
{
    debug( std::cout << "MemMap::sendLooks" << std::endl << std::flush;);
    std::list<std::string>::const_iterator I = m_additionsById.begin();
    std::list<std::string>::const_iterator Iend = m_additionsById.end();
    for (; I != Iend; ++I) {
        Look l;
        Anonymous look_arg;
        look_arg->setId(*I);
        l->setArgs1(look_arg);
        res.push_back(l);
    }
    m_additionsById.clear();
}

MemEntity * MemMap::addId(const std::string & id, long int_id)
// Queue the ID of an entity we are interested in
{
    assert(!id.empty());
    assert(m_entities.find(int_id) == m_entities.end());

    debug( std::cout << "MemMap::add_id" << std::endl << std::flush;);
    m_additionsById.push_back(id);
    MemEntity * entity = new MemEntity(id, int_id);
    entity->setType(m_entity_type);
    return addEntity(entity);
}

void MemMap::del(const std::string & id)
// Delete an entity from memory
{
    debug( std::cout << "MemMap::del(" << id << ")" << std::endl << std::flush;);

    //HACK: We currently do refcounting for Locations kept in the mind as knowledge.
    //The result is that if an entity is removed here, it will be deleted, and any
    //knowledge or goal referring to it will point to an invalid pointer.
    //Then result is a segfault whenever the mind is queried.
    //To prevent this we'll add this interim fix, where we exit from the method.
    //This is an interim solution until we've better dealt with Locations in goals and knowledge.
    return;

    long int_id = integerId(id);

    MemEntityDict::iterator I = m_entities.find(int_id);
    if (I != m_entities.end()) {
        MemEntity * ent = I->second;
        assert(ent != 0);
        long next = -1;
        if (m_checkIterator != m_entities.end()) {
            next = m_checkIterator->first;
        }
        m_entities.erase(I);

        ent->destroy(); // should probably go here, but maybe earlier

        if (next != -1) {
            m_checkIterator = m_entities.find(next);
        } else {
            m_checkIterator = m_entities.begin();
        }

        if (m_script != 0) {
            std::vector<std::string>::const_iterator J = m_deleteHooks.begin();
            std::vector<std::string>::const_iterator Jend = m_deleteHooks.end();
            for(; J != Jend; ++J) {
                m_script->hook(*J, ent);
            }
        }
        ent->decRef();
    }
}

MemEntity * MemMap::get(const std::string & id) const
// Get an entity from memory
{
    debug( std::cout << "MemMap::get" << std::endl << std::flush;);
    if (id.empty()) {
        // This shouldn't really occur, and shouldn't be a problem
        log(ERROR, "MemMap::get queried for empty ID string.");
        return NULL;
    }

    long int_id = integerId(id);

    MemEntityDict::const_iterator I = m_entities.find(int_id);
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

    long int_id = integerId(id);

    if (int_id == -1) {
        log(ERROR, String::compose("MemMap::getAdd: Invalid ID \"%1\".", id));
        return NULL;
    }

    MemEntityDict::const_iterator I = m_entities.find(int_id);
    if (I != m_entities.end()) {
        assert(I->second != 0);
        return I->second;
    }
    return addId(id, int_id);
}

void MemMap::addContents(const RootEntity & ent)
// Iterate over the contains attribute of a message, looking at all the contents
{
    if (!ent->hasAttrFlag(Atlas::Objects::Entity::CONTAINS_FLAG)) {
        return;
    }
    const std::list<std::string> & contlist = ent->getContains();
    std::list<std::string>::const_iterator Jend = contlist.end();
    std::list<std::string>::const_iterator J = contlist.begin();
    for (; J != Jend; ++J) {
        getAdd(*J);
    }
}

MemEntity * MemMap::updateAdd(const RootEntity & ent, const double & d)
// Update an entity in our memory, from an Atlas message
// The mind code relies on this function never sending a Sight to
// be sure that seeing something created does not imply that the created
// entity is visible, as we may have received it because we can see the
// creator.
{
    debug( std::cout << "MemMap::updateAdd" << std::endl << std::flush;);
    if (!ent->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
        log(ERROR, "MemMap::updateAdd, Missing id in updated entity");
        return NULL;
    }
    const std::string & id = ent->getId();
    if (id.empty()) {
        log(ERROR, "MemMap::updateAdd, Empty ID in updated entity.");
        return NULL;
    }

    long int_id = integerId(id);

    if (int_id == -1) {
        log(ERROR, String::compose("MemMap::updateAdd: Invalid ID \"%1\".", id));
        return NULL;
    }

    MemEntityDict::const_iterator I = m_entities.find(int_id);
    MemEntity * entity;
    if (I == m_entities.end()) {
        entity = newEntity(id, int_id, ent);
    } else {
        entity = I->second;
        updateEntity(entity, ent);
    }
    entity->update(d);
    return entity;
}

void MemMap::addEntityMemory(const std::string& id,
                             const std::string& memory,
                             const Element& value)
{
    auto entity_memory = m_entityRelatedMemory.find(id);
    if (entity_memory != m_entityRelatedMemory.end()) {
        entity_memory->second[memory] = value;
    } else {
        m_entityRelatedMemory.insert(
                std::make_pair(id, std::map<std::string, Element> { { memory, value } }));
    }
}

void MemMap::recallEntityMemory(const std::string& id,
                                const std::string& memory,
                                Element& value) const
{
    auto entity_memory = m_entityRelatedMemory.find(id);
    if (entity_memory != m_entityRelatedMemory.end()) {
        auto specific_memory = entity_memory->second.find(memory);
        if (specific_memory != entity_memory->second.end()) {
            value = specific_memory->second;
        }
    }
}

const std::map<std::string, std::map<std::string, Element>>& MemMap::getEntityRelatedMemory() const
{
    return m_entityRelatedMemory;
}


EntityVector MemMap::findByType(const std::string & what)
// Find an entity in our memory of a certain type
{
    EntityVector res;
    
    MemEntityDict::const_iterator Iend = m_entities.end();
    for (MemEntityDict::const_iterator I = m_entities.begin(); I != Iend; ++I) {
        MemEntity * item = I->second;
        debug( std::cout << "F" << what << ":" << item->getType() << ":" << item->getId() << std::endl << std::flush;);
        if (item->isVisible() && item->getType()->name() == what) {
            res.push_back(I->second);
        }
    }
    return res;
}

EntityVector MemMap::findByLocation(const Location & loc,
                                       WFMath::CoordType radius,
                                       const std::string & what)
// Find an entity in our memory in a certain place
// FIXME Don't return by value
{
    EntityVector res;
    LocatedEntity * place = loc.m_loc;
    if (place->m_contains == 0) {
        return res;
    }
#ifndef NDEBUG
    MemEntity * place_by_id = get(place->getId());
    if (place != place_by_id) {
        log(ERROR, compose("MemMap consistency check failure: find location "
                           "has LOC %1(%2) which is different in dict (%3)",
                           place->getId(), place->getType()->name(),
                           place_by_id->getType()->name()));
        return res;
    }
#endif // NDEBUG
    LocatedEntitySet::const_iterator I = place->m_contains->begin();
    LocatedEntitySet::const_iterator Iend = place->m_contains->end();
    float square_range = radius * radius;
    for (; I != Iend; ++I) {
        assert(*I != 0);
        LocatedEntity * item = *I;
        if (item == 0) {
            log(ERROR, "Weird entity in memory");
            continue;
        }
        if (!item->isVisible() || item->getType()->name() != what) {
            continue;
        }
        if (squareDistance(loc.pos(), item->m_location.pos()) < square_range) {
            res.push_back(item);
        }
    }
    return res;
}

void MemMap::check(const double & time)
{
    MemEntityDict::const_iterator entities_end = m_entities.end();
    if (m_checkIterator == entities_end) {
        m_checkIterator = m_entities.begin();
    } else {
        MemEntity * me = m_checkIterator->second;
        assert(me != 0);
        if (!me->isVisible() && (time - me->lastSeen()) > 600 &&
            (me->m_contains == 0 || me->m_contains->empty())) {
            debug(std::cout << me->getId() << "|" << me->getType()->name()
                      << " is a waste of space" << std::endl << std::flush;);
            MemEntityDict::const_iterator J = m_checkIterator;
            long next = -1;
            if (++J != entities_end) {
                next = J->first;
            }
            m_entities.erase(m_checkIterator);
            // Remove deleted entity from its parents contains attribute
            if (me->m_location.m_loc != 0) {
                assert(me->m_location.m_loc->m_contains != 0);
                me->m_location.m_loc->m_contains->erase(me);
            }
            
            // FIXME This is required until MemMap uses parent refcounting
            me->m_location.m_loc = 0;

            if (next != -1) {
                m_checkIterator = m_entities.find(next);
            } else {
                m_checkIterator = m_entities.begin();
            }
            // attribute of its its parent.
            me->decRef();
        } else {
            debug(std::cout << me->getId() << "|" << me->getType()->name() << "|"
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
        // FIXME This is required until MemMap uses parent refcounting
        I->second->m_location.m_loc = 0;
        I->second->decRef();
    }
}

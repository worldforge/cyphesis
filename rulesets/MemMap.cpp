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
#include "BaseMind.h"
#include "Script.h"

#include "common/id.h"
#include "common/debug.h"
#include "common/TypeNode.h"
#include "common/Inheritance.h"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>

static const bool debug_flag = false;

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Objects::Operation::Look;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Entity::Anonymous;

using String::compose;

const TypeNode * MemMap::m_entity_type = nullptr;

void MemMap::setScript(Script* script)
{
    m_script = script;
}


Ref<MemEntity> MemMap::addEntity(const Ref<MemEntity> entity)
{
    assert(entity != nullptr);
    assert(!entity->getId().empty());
    assert(entity->getType() != nullptr);

    debug(std::cout << "MemMap::addEntity " << entity->describeEntity() << " " << entity->getId()
                    << std::endl << std::flush;);
    long next = -1;
    if (m_checkIterator != m_entities.end()) {
        next = m_checkIterator->first;
    }
    m_entities[entity->getIntId()] = entity;
    m_checkIterator = m_entities.find(next);

    if (m_script) {
        debug( std::cout << this << std::endl << std::flush;);
        auto I = m_addHooks.begin();
        auto Iend = m_addHooks.end();
        for (; I != Iend; ++I) {
            m_script->hook(*I, entity.get());
        }
    }

    if (m_listener) {
        m_listener->entityAdded(*entity);
    }

    return entity;
}

void MemMap::readEntity(const Ref<MemEntity> entity, const RootEntity & ent, double timestamp)
// Read the contents of an Atlas message into an entity
{
    if (ent->hasAttrFlag(Atlas::Objects::PARENT_FLAG)) {
        const std::string& parent = ent->getParent();
        if (entity->getType() == m_entity_type) {
            const TypeNode * type = Inheritance::instance().getType(parent);
            if (type != 0) {
                entity->setType(type);
            }
        } else if (entity->getType()->name() != parent) {
            debug(std::cout << "Attempting to mutate " << entity->getType()
                            << " into " << parent
                            << std::endl << std::flush;);
        }
    }
    entity->merge(ent->asMessage());
    if (ent->hasAttrFlag(Atlas::Objects::Entity::LOC_FLAG)) {
        auto old_loc = entity->m_location.m_loc;
        const std::string & new_loc_id = ent->getLoc();
        // Has LOC been changed?
        if (!old_loc || new_loc_id != old_loc->getId()) {
            entity->m_location.m_loc = getAdd(new_loc_id);
            assert(entity->m_location.m_loc);
            assert(old_loc != entity->m_location.m_loc);
            if (old_loc) {
                assert(old_loc->m_contains != nullptr);
                old_loc->m_contains->erase(entity);
            }
            if (entity->m_location.m_loc->m_contains == nullptr) {
                entity->m_location.m_loc->m_contains = new LocatedEntitySet{};
            }
            entity->m_location.m_loc->m_contains->insert(entity);
        }
        entity->m_location.readFromEntity(ent);
        entity->m_location.update(timestamp);
    }
    addContents(ent);
}

void MemMap::updateEntity(const Ref<MemEntity> entity, const RootEntity & ent, double timestamp)
// Update contents of entity an Atlas message.
{
    assert(entity != nullptr);

    debug( std::cout << " got " << entity->describeEntity() << std::endl << std::flush;);

    auto old_loc = entity->m_location.m_loc;
    readEntity(entity, ent, timestamp);

    if (m_script) {
        auto K = m_updateHooks.begin();
        auto Kend = m_updateHooks.end();
        for (; K != Kend; ++K) {
            m_script->hook(*K, entity.get());
        }
    }
    if (m_listener) {
        m_listener->entityUpdated(*entity, ent, old_loc.get());
    }

}

Ref<MemEntity> MemMap::newEntity(const std::string & id, long int_id,
                              const RootEntity & ent, double timestamp)
// Create a new entity from an Atlas message.
{
    assert(m_entities.find(int_id) == m_entities.end());

    Ref<MemEntity> entity = new MemEntity(id, int_id);
    entity->setType(m_entity_type);

    readEntity(entity, ent, timestamp);

    return addEntity(entity);
}

MemMap::MemMap() : m_checkIterator(m_entities.begin()), m_listener(nullptr)
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
    auto I = m_additionsById.begin();
    auto Iend = m_additionsById.end();
    for (; I != Iend; ++I) {
        Look l;
        Anonymous look_arg;
        look_arg->setId(*I);
        l->setArgs1(look_arg);
        res.push_back(l);
    }
    m_additionsById.clear();
}

Ref<MemEntity> MemMap::addId(const std::string & id, long int_id)
// Queue the ID of an entity we are interested in
{
    assert(!id.empty());
    assert(m_entities.find(int_id) == m_entities.end());

    debug( std::cout << "MemMap::add_id" << std::endl << std::flush;);
    m_additionsById.push_back(id);
    Ref<MemEntity> entity = new MemEntity(id, int_id);
    entity->setType(m_entity_type);
    return addEntity(entity);
}

void MemMap::del(const std::string & id)
// Delete an entity from memory
{
    debug( std::cout << "MemMap::del(" << id << ")" << std::endl << std::flush;);

    long int_id = integerId(id);

    auto I = m_entities.find(int_id);
    if (I != m_entities.end()) {
        auto ent = I->second;
        assert(ent);

        //HACK: We currently do refcounting for Locations kept in the mind as knowledge.
        //The result is that if an entity is removed here, it will be deleted, and any
        //knowledge or goal referring to it will point to an invalid pointer.
        //Then result is a segfault whenever the mind is queried.
        //To prevent this we'll add this interim fix, where we exit from the method.
        //This is an interim solution until we've better dealt with Locations in goals and knowledge.

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

        if (m_script) {
            for (auto& hook : m_deleteHooks) {
                m_script->hook(hook, ent.get());
            }
        }

        if (m_listener) {
            m_listener->entityDeleted(*ent);
        }

//        ent->decRef();
    }
}

Ref<MemEntity> MemMap::get(const std::string & id) const
// Get an entity from memory
{
    debug( std::cout << "MemMap::get" << std::endl << std::flush;);
    if (id.empty()) {
        // This shouldn't really occur, and shouldn't be a problem
        log(ERROR, "MemMap::get queried for empty ID string.");
        return nullptr;
    }

    long int_id = integerId(id);

    auto I = m_entities.find(int_id);
    if (I != m_entities.end()) {
        assert(I->second != nullptr);
        return I->second;
    }
    return nullptr;
}

Ref<MemEntity> MemMap::getAdd(const std::string & id)
// Get an entity from memory, or add it if we haven't seen it yet
// This could be implemented by calling get() for all but the the last line
{
    debug( std::cout << "MemMap::getAdd(" << id << ")" << std::endl << std::flush;);
    if (id.empty()) {
        return nullptr;
    }

    long int_id = integerId(id);

    if (int_id == -1) {
        log(ERROR, String::compose("MemMap::getAdd: Invalid ID \"%1\".", id));
        return nullptr;
    }

    MemEntityDict::const_iterator I = m_entities.find(int_id);
    if (I != m_entities.end()) {
        assert(I->second != nullptr);
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
    auto Jend = contlist.end();
    auto J = contlist.begin();
    for (; J != Jend; ++J) {
        getAdd(*J);
    }
}

Ref<MemEntity> MemMap::updateAdd(const RootEntity & ent, const double & d)
// Update an entity in our memory, from an Atlas message
// The mind code relies on this function never sending a Sight to
// be sure that seeing something created does not imply that the created
// entity is visible, as we may have received it because we can see the
// creator.
{
    debug( std::cout << "MemMap::updateAdd" << std::endl << std::flush;);
    if (!ent->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
        log(ERROR, "MemMap::updateAdd, Missing id in updated entity");
        return nullptr;
    }
    const std::string & id = ent->getId();
    if (id.empty()) {
        log(ERROR, "MemMap::updateAdd, Empty ID in updated entity.");
        return nullptr;
    }

    long int_id = integerId(id);

    if (int_id == -1) {
        log(ERROR, String::compose("MemMap::updateAdd: Invalid ID \"%1\".", id));
        return nullptr;
    }

    MemEntityDict::const_iterator I = m_entities.find(int_id);
    Ref<MemEntity> entity;
    if (I == m_entities.end()) {
        entity = newEntity(id, int_id, ent, d);
    } else {
        entity = I->second;
        updateEntity(entity, ent, d);
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
    
    auto Iend = m_entities.end();
    for (auto I = m_entities.begin(); I != Iend; ++I) {
        auto item = I->second;
        debug( std::cout << "F" << what << ":" << item->getType() << ":" << item->getId() << std::endl << std::flush;);
        if (item->isVisible() && item->getType()->name() == what) {
            res.push_back(I->second.get());
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
    auto place = loc.m_loc;
    if (place->m_contains == 0) {
        return res;
    }
#ifndef NDEBUG
    auto place_by_id = get(place->getId());
    if (place != place_by_id) {
        log(ERROR, compose("MemMap consistency check failure: find location "
                           "has LOC %1(%2) which is different in dict (%3)",
                           place->getId(), place->getType()->name(),
                           place_by_id->getType()->name()));
        return res;
    }
#endif // NDEBUG
    auto I = place->m_contains->begin();
    auto Iend = place->m_contains->end();
    float square_range = radius * radius;
    for (; I != Iend; ++I) {
        assert(*I != nullptr);
        auto item = *I;
        if (!item) {
            log(ERROR, "Weird entity in memory");
            continue;
        }
        if (!item->isVisible() || item->getType()->name() != what) {
            continue;
        }
        if (squareDistance(loc.pos(), item->m_location.pos()) < square_range) {
            res.push_back(item.get());
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
        auto me = m_checkIterator->second;
        assert(me != nullptr);
        if (!me->isVisible() && (time - me->lastSeen()) > 600 &&
            (me->m_contains == nullptr || me->m_contains->empty())) {
            //Don't remove any entities; we need to instead implement reference counted locations
//            debug(std::cout << me->getId() << "|" << me->getType()->name()
//                      << " is a waste of space" << std::endl << std::flush;);
//            MemEntityDict::const_iterator J = m_checkIterator;
//            long next = -1;
//            if (++J != entities_end) {
//                next = J->first;
//            }
//            m_entities.erase(m_checkIterator);
//            // Remove deleted entity from its parents contains attribute
//            if (me->m_location.m_loc != 0) {
//                assert(me->m_location.m_loc->m_contains != 0);
//                me->m_location.m_loc->m_contains->erase(me);
//            }
//
//            // FIXME This is required until MemMap uses parent refcounting
//            me->m_location.m_loc = 0;
//
//            if (next != -1) {
//                m_checkIterator = m_entities.find(next);
//            } else {
//                m_checkIterator = m_entities.begin();
//            }
            //HACK: We currently do refcounting for Locations kept in the mind as knowledge.
            //The result is that if an entity is removed here, it will be deleted, and any
            //knowledge or goal referring to it will point to an invalid pointer.
            //Then result is a segfault whenever the mind is queried.
            //To prevent this we'll add this interim fix, where we won't decrease the reference.
            //This is an interim solution until we've better dealt with Locations in goals and knowledge.

            // attribute of its its parent.
            //me->decRef();
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
    m_entities.clear();
//    MemEntityDict::const_iterator Iend = m_entities.end();
//    for (MemEntityDict::const_iterator I = m_entities.begin(); I != Iend; ++I) {
//        // FIXME This is required until MemMap uses parent refcounting
//        I->second->m_location.m_loc = 0;
//        I->second->decRef();
//    }
}

void MemMap::setListener(MapListener* listener)
{
    m_listener = listener;
}


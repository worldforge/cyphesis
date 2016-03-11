// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2008 Alistair Riddoch
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


#include "StorageManager.h"

#include "WorldRouter.h"
#include "EntityBuilder.h"
#include "MindInspector.h"

#include "rulesets/LocatedEntity.h"
#include "rulesets/Character.h"
#include "rulesets/MindProperty.h"
#include "rulesets/Domain.h"

#include "common/Database.h"
#include "common/TypeNode.h"
#include "common/Property.h"
#include "common/debug.h"
#include "common/Monitors.h"
#include "common/PropertyManager.h"
#include "common/id.h"
#include "common/log.h"
#include "common/compose.hpp"
#include "common/Variable.h"
#include "common/custom.h"
#include "common/Think.h"
#include "common/Commune.h"
#include "common/SystemTime.h"

#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/SmartPtr.h>
#include <Atlas/Message/Element.h>

#include <wfmath/atlasconv.h>

#include <sigc++/adaptors/bind.h>
#include <sigc++/functors/mem_fun.h>

#include <iostream>
#include <unordered_set>

using Atlas::Message::MapType;
using Atlas::Message::Element;

using String::compose;

typedef Database::KeyValues KeyValues;

static const bool debug_flag = false;

StorageManager:: StorageManager(WorldRouter & world) :
        m_mindInspector(nullptr),
      m_insertEntityCount(0), m_updateEntityCount(0),
      m_insertPropertyCount(0), m_updatePropertyCount(0),
      m_insertQps(0), m_updateQps(0),
      m_insertQpsNow(0), m_updateQpsNow(0),
      m_insertQpsAvg(0), m_updateQpsAvg(0),
      m_insertQpsIndex(0), m_updateQpsIndex(0)
{
    if (database_flag) {

        m_mindInspector = new MindInspector();
        m_mindInspector->ThoughtsReceived.connect(sigc::mem_fun(*this, &StorageManager::thoughtsReceived));

        world.inserted.connect(sigc::mem_fun(this,
              &StorageManager::entityInserted));

        Monitors::instance()->watch("storage_entity_inserts",
                                    new Variable<int>(m_insertEntityCount));
        Monitors::instance()->watch("storage_entity_updates",
                                    new Variable<int>(m_updateEntityCount));
        Monitors::instance()->watch("storage_property_inserts",
                                    new Variable<int>(m_insertPropertyCount));
        Monitors::instance()->watch("storage_property_updates",
                                    new Variable<int>(m_updatePropertyCount));

        Monitors::instance()->watch("storage_qps{qtype=\"inserts\",t=\"1\"}",
                                    new Variable<int>(m_insertQpsNow));
        Monitors::instance()->watch("storage_qps{qtype=\"updates\",t=\"1\"}",
                                    new Variable<int>(m_updateQpsNow));

        Monitors::instance()->watch("storage_qps{qtype=\"inserts\",t=\"32\"}",
                                    new Variable<int>(m_insertQpsAvg));
        Monitors::instance()->watch("storage_qps{qtype=\"updates\",t=\"32\"}",
                                    new Variable<int>(m_updateQpsAvg));

        for (int i = 0; i < 32; ++i) {
            m_insertQpsRing[i] = 0;
            m_updateQpsRing[i] = 0;
        }

        Persistence::instance()->characterAdded.connect(sigc::mem_fun(*this, &StorageManager::persistance_characterAdded));
        Persistence::instance()->characterDeleted.connect(sigc::mem_fun(*this, &StorageManager::persistance_characterDeleted));
    }
}

StorageManager::~StorageManager()
{
    delete m_mindInspector;
}

/// \brief Called when a new Entity is inserted in the world
void StorageManager::entityInserted(LocatedEntity * ent)
{
    if (ent->getFlags() & (entity_ephem)) {
        // This entity is not persisted.
        return;
    }
    if (ent->getFlags() & (entity_clean)) {
        // This entity has just been restored from the database, so does
        // not need to be inserted, but will need to be updated.
        // For non-restored entities that have been newly created this
        // signal will be connected later once the initial database insert
        // has been done.
        ent->updated.connect(sigc::bind(sigc::mem_fun(this, &StorageManager::entityUpdated), ent));
        ent->containered.connect(sigc::bind(sigc::mem_fun(this, &StorageManager::entityContainered), ent));
        return;
    }
    // Queue the entity to be inserted into the persistence tables.
    m_unstoredEntities.push_back(EntityRef(ent));
    ent->setFlags(entity_queued);
}

/// \brief Called when an Entity is modified
void StorageManager::entityUpdated(LocatedEntity * ent)
{
    if (ent->isDestroyed()) {
        m_destroyedEntities.push_back(ent->getIntId());
        return;
    }
    // Is it already in the dirty Entities queue?
    // Perhaps we need to modify the semantics of the updated signal
    // so it is only emitted if the entity was not marked as dirty.
    if (ent->getFlags() & entity_queued) {
        // std::cout << "Already queued " << ent->getId() << std::endl << std::flush;
        return;
    }
    m_dirtyEntities.push_back(EntityRef(ent));
    // std::cout << "Updated fired " << ent->getId() << std::endl << std::flush;
    ent->setFlags(entity_queued);
}

void StorageManager::entityContainered(const LocatedEntity *oldLocation, LocatedEntity *entity)
{
    entityUpdated(entity);
}

bool StorageManager::persistance_characterAdded(const Persistence::AddCharacterData& data)
{
    m_addedCharacters.push_back(data);
    return true;
}

bool StorageManager::persistance_characterDeleted(const std::string& entityId)
{
    m_deletedCharacters.push_back(entityId);
    return true;
}

void StorageManager::encodeProperty(PropertyBase * prop, std::string & store)
{
    Atlas::Message::MapType map;
    prop->get(map["val"]);
    Database::instance()->encodeObject(map, store);
}

void StorageManager::restorePropertiesRecursively(LocatedEntity * ent)
{
    Database * db = Database::instance();
    PropertyManager * pm = PropertyManager::instance();
    DatabaseResult res = db->selectProperties(ent->getId());

    //Keep track of those properties that have been set on the instance, so we'll know what
    //type properties we should ignore.
    std::unordered_set<std::string> instanceProperties;

    DatabaseResult::const_iterator I = res.begin();
    DatabaseResult::const_iterator Iend = res.end();
    for (; I != Iend; ++I) {
        const std::string name = I.column("name");
        if (name.empty()) {
            log(ERROR, compose("No name column in property row for %1",
                               ent->getId()));
            continue;
        }
        const std::string val_string = I.column("value");
        if (name.empty()) {
            log(ERROR, compose("No value column in property row for %1,%2",
                               ent->getId(), name));
            continue;
        }
        MapType prop_data;
        db->decodeMessage(val_string, prop_data);
        MapType::const_iterator J = prop_data.find("val");
        if (J == prop_data.end()) {
            log(ERROR, compose("No property value data for %1:%2",
                               ent->getId(), name));
            continue;
        }
        assert(ent->getType() != 0);
        const Element & val = J->second;

        Element existingVal;
        if (ent->getAttr(name, existingVal) == 0) {
            if (existingVal == val) {
                //If the existing property, either on the instance or the type, is equal to the persisted one just skip it.
                continue;
            }
        }


        PropertyBase * prop = ent->modProperty(name);
        if (prop == nullptr) {
            prop = pm->addProperty(name, val.getType());
            prop->install(ent, name);
            //This transfers ownership of the property to the entity.
            ent->setProperty(name, prop);
        }

        //If we get to here the property either doesn't exists, or have a different value than the default or existing property.
        prop->set(val);
        prop->setFlags(per_clean | per_seen);
        prop->apply(ent);
        instanceProperties.insert(name);
    }

    if (ent->getType()) {
        for (auto& propIter : ent->getType()->defaults()) {
            if (!instanceProperties.count(propIter.first)) {
                PropertyBase * prop = propIter.second;
                // If a property is in the class it won't have been installed
                // as setAttr() checks
                prop->install(ent, propIter.first);
                // The property will have been applied if it has an overriden
                // value, so we only apply it the value is still default.
                prop->apply(ent);
            }
        }
    }

    auto domain = ent->getMovementDomain();
    if (domain) {
        domain->addEntity(*ent);
    }


    //Now restore all properties of the child entities.
    if (ent->m_contains) {
        for (auto& childEntity : *ent->m_contains) {
            restorePropertiesRecursively(childEntity);
        }
    }

    //We must send a sight op to the entity informing it of itself before we send any thoughts.
    //Else the mind won't have any information about itself.
    {
        Atlas::Objects::Operation::Sight sight;
        sight->setTo(ent->getId());
        Atlas::Objects::Entity::Anonymous args;
        ent->addToEntity(args);
        sight->setArgs1(args);
        ent->sendWorld(sight);
    }
    //We should also send a sight op to the parent entity which owns the entity.
    //TODO: should this really be necessary or should we rely on other Sight functionality?
    if (ent->m_location.m_loc) {
        Atlas::Objects::Operation::Sight sight;
        sight->setTo(ent->m_location.m_loc->getId());
        Atlas::Objects::Entity::Anonymous args;
        ent->addToEntity(args);
        sight->setArgs1(args);
        ent->m_location.m_loc->sendWorld(sight);
    }

    restoreThoughts(ent);

}

void StorageManager::restoreThoughts(LocatedEntity * ent)
{
    Database * db = Database::instance();
    const DatabaseResult res = db->selectThoughts(ent->getId());
    Atlas::Message::ListType thoughts_data;

    DatabaseResult::const_iterator I = res.begin();
    DatabaseResult::const_iterator Iend = res.end();
    for (; I != Iend; ++I) {
        const std::string thought = I.column("thought");
        if (thought.empty()) {
            log(ERROR,
                    compose("No thought column in property row for %1",
                            ent->getId()));
            continue;
        }
        MapType thought_data;
        db->decodeMessage(thought, thought_data);
        thoughts_data.push_back(thought_data);
    }

    if (!thoughts_data.empty()) {
        OpVector opRes;

        Atlas::Objects::Operation::Think thoughtOp;
        Atlas::Objects::Operation::Set setOp;
        setOp->setArgsAsList(thoughts_data);
        //Make the thought come from the entity itself
        thoughtOp->setArgs1(setOp);
        thoughtOp->setTo(ent->getId());
        thoughtOp->setFrom(ent->getId());

        ent->sendWorld(thoughtOp);
    }
}

bool StorageManager::storeThoughts(LocatedEntity * ent)
{
    if (!m_mindInspector) {
        return false;
    }

    if (ent->getFlags() & (entity_ephem)) {
        // This entity is not persisted.
        return false;
    }
    //Check if the entity has a mind. Perhaps do this in another way than using dynamic cast?
    Character* character = dynamic_cast<Character*>(ent);
    if (character) {

        const MindProperty* mindProperty = character->getPropertyClass<MindProperty>("mind");
        if (mindProperty) {
            if (mindProperty->isMindEnabled()) {
                m_mindInspector->queryEntityForThoughts(character->getId());
                m_outstandingThoughtRequests.insert(character->getId());
                return true;
            }
        }
    }
    return false;
}


void StorageManager::insertEntity(LocatedEntity * ent)
{
    std::string location;
    Atlas::Message::MapType map;
    map["pos"] = ent->m_location.pos().toAtlas();
    if (ent->m_location.orientation().isValid()) {
        map["orientation"] = ent->m_location.orientation().toAtlas();
    }
    Database::instance()->encodeObject(map, location);

    Database::instance()->insertEntity(ent->getId(),
                                       ent->m_location.m_loc->getId(),
                                       ent->getType()->name(),
                                       ent->getSeq(),
                                       location);
    ++m_insertEntityCount;
    KeyValues property_tuples;
    const PropertyDict & properties = ent->getProperties();
    PropertyDict::const_iterator I = properties.begin();
    PropertyDict::const_iterator Iend = properties.end();
    for (; I != Iend; ++I) {
        PropertyBase * prop = I->second;
        if (prop->flags() & per_ephem) {
            continue;
        }
        encodeProperty(prop, property_tuples[I->first]);
        prop->setFlags(per_clean | per_seen);
    }
    if (!property_tuples.empty()) {
        Database::instance()->insertProperties(ent->getId(), property_tuples);
        ++m_insertPropertyCount;
    }
    ent->resetFlags(entity_queued);
    ent->setFlags(entity_clean | entity_pos_clean | entity_orient_clean);
    ent->updated.connect(sigc::bind(sigc::mem_fun(this, &StorageManager::entityUpdated), ent));
    ent->containered.connect(sigc::bind(sigc::mem_fun(this, &StorageManager::entityContainered), ent));

}

void StorageManager::updateEntityThoughts(LocatedEntity * ent)
{
    Database * db = Database::instance();
    auto thoughts = ent->getThoughts();
    std::vector<std::string> thoughtsList;
    for (auto& thoughtOp : thoughts) {
        Atlas::Message::MapType map;
        thoughtOp->addToMessage(map);
        std::string value;
        db->encodeObject(map, value);
        thoughtsList.push_back(value);
    }
    db->replaceThoughts(ent->getId(), thoughtsList);

    ent->resetFlags(entity_dirty_thoughts);
}

void StorageManager::updateEntity(LocatedEntity * ent)
{
    std::string location;
    Atlas::Message::MapType map;
    map["pos"] = ent->m_location.pos().toAtlas();
    if (ent->m_location.orientation().isValid()) {
        map["orientation"] = ent->m_location.orientation().toAtlas();
    }
    Database::instance()->encodeObject(map, location);

    //Under normal circumstances only the top world won't have a location.
    if (ent->m_location.m_loc) {
        Database::instance()->updateEntity(ent->getId(),
                                       ent->getSeq(),
                                       location,
                                       ent->m_location.m_loc->getId());
    } else {
        Database::instance()->updateEntityWithoutLoc(ent->getId(),
                                       ent->getSeq(),
                                       location);
    }
    ++m_updateEntityCount;
    KeyValues new_property_tuples;
    KeyValues upd_property_tuples;
    const PropertyDict & properties = ent->getProperties();
    PropertyDict::const_iterator I = properties.begin();
    PropertyDict::const_iterator Iend = properties.end();
    for (; I != Iend; ++I) {
        PropertyBase * prop = I->second;
        if (prop->flags() & per_mask) {
            continue;
        }
        // FIXME check if this is new or just modded.
        if (prop->flags() & per_seen) {
            encodeProperty(prop, upd_property_tuples[I->first]);
            ++m_updatePropertyCount;
        } else {
            encodeProperty(prop, new_property_tuples[I->first]);
            ++m_insertPropertyCount;
        }
        prop->setFlags(per_clean | per_seen);
    }
    if (!new_property_tuples.empty()) {
        Database::instance()->insertProperties(ent->getId(),
                                               new_property_tuples);
    }
    if (!upd_property_tuples.empty()) {
        Database::instance()->updateProperties(ent->getId(),
                                               upd_property_tuples);
    }
    ent->setFlags(entity_clean);
}

void StorageManager::restoreChildren(LocatedEntity * parent)
{
    Database * db = Database::instance();
    DatabaseResult res = db->selectEntities(parent->getId());
    EntityBuilder * eb = EntityBuilder::instance();

    // Iterate over res creating entities, and sorting out position, location
    // and orientation. Restore children, but don't restore any properties yet.
    DatabaseResult::const_iterator I = res.begin();
    DatabaseResult::const_iterator Iend = res.end();
    for (; I != Iend; ++I) {
        const std::string id = I.column("id");
        const int int_id = forceIntegerId(id);
        const std::string type = I.column("type");
        //By sending an empty attributes pointer we're telling the builder not to apply any default
        //attributes. We will instead apply all attributes ourselves when we later on restore attributes.
        Atlas::Objects::SmartPtr<Atlas::Objects::Entity::RootEntityData> attrs(nullptr);
        LocatedEntity * child = eb->newEntity(id, int_id, type, attrs, BaseWorld::instance());
        if (!child) {
            log(ERROR, compose("Could not restore entity with id %1 of type %2"
                    ", most likely caused by this type missing.",
                    id, type));
            continue;
        }
        
        const std::string location_string = I.column("location");
        MapType loc_data;
        db->decodeMessage(location_string, loc_data);
        child->m_location.readFromMessage(loc_data);
        if (!child->m_location.pos().isValid()) {
            std::cout << "No pos data" << std::endl << std::flush;
            log(ERROR, compose("Entity %1 restored from database has no "
                               "POS data. Ignored.", child->getId()));
            delete child;
            continue;
        }
        child->m_location.m_loc = parent;
        child->setFlags(entity_clean | entity_pos_clean | entity_orient_clean);
        BaseWorld::instance().addEntity(child);
        restoreChildren(child);
    }
}

void StorageManager::tick()
{
    int inserts = 0, updates = 0;
    int old_insert_queries = m_insertEntityCount + m_insertPropertyCount;
    int old_update_queries = m_updateEntityCount + m_updatePropertyCount;

    while (!m_destroyedEntities.empty()) {
        long id = m_destroyedEntities.front();
        Database::instance()->dropEntity(id);
        m_destroyedEntities.pop_front();
    }

    while (!m_unstoredEntities.empty()) {
        const EntityRef & ent = m_unstoredEntities.front();
        if (ent.get() != 0) {
            debug( std::cout << "storing " << ent->getId() << std::endl << std::flush; );
            insertEntity(ent.get());
            ++inserts;
        } else {
            debug( std::cout << "deleted" << std::endl << std::flush; );
        }
        m_unstoredEntities.pop_front();
    }

    while (!m_addedCharacters.empty()) {
        auto& data = m_addedCharacters.front();
        Database::instance()->createRelationRow(Persistence::instance()->getCharacterAccountRelationName(), data.account_id, data.entity_id);
        m_addedCharacters.pop_front();
    }

    while (!m_deletedCharacters.empty()) {
        auto& entity_id = m_deletedCharacters.front();
        Database::instance()->removeRelationRowByOther(Persistence::instance()->getCharacterAccountRelationName(), entity_id);
        m_deletedCharacters.pop_front();
    }

    while (!m_dirtyEntities.empty()) {
        if (Database::instance()->queryQueueSize() > 200) {
            debug(std::cout << "Too many" << std::endl << std::flush;);
            break;
        }
        const EntityRef & ent = m_dirtyEntities.front();
        if (ent.get() != 0) {
            if ((ent->getFlags() & entity_clean_mask) == 0) {
                debug( std::cout << "updating " << ent->getId() << std::endl << std::flush; );
                updateEntity(ent.get());
                ++updates;
            }
            if ((ent->getFlags() & entity_dirty_thoughts) != 0) {
                debug( std::cout << "updating thoughts " << ent->getId() << std::endl << std::flush; );
                updateEntityThoughts(ent.get());
                ++updates;
            }
            ent->resetFlags(entity_queued);
        } else {
            debug( std::cout << "deleted" << std::endl << std::flush; );
        }
        m_dirtyEntities.pop_front();
    }

    if (inserts > 0 || updates > 0) {
        debug(std::cout << "I: " << inserts << " U: " << updates
                        << std::endl << std::flush;);
    }
    int insert_queries = m_insertEntityCount + m_insertPropertyCount 
                         - old_insert_queries;

    if (++m_insertQpsIndex >= 32) {
        m_insertQpsIndex = 0;
    }
    m_insertQps -= m_insertQpsRing[m_insertQpsIndex];
    m_insertQps += insert_queries;
    m_insertQpsRing[m_insertQpsIndex] = insert_queries;
    m_insertQpsAvg = m_insertQps / 32;
    m_insertQpsNow = insert_queries;

    debug(if (insert_queries) { std::cout << "Ins: " << insert_queries << ", " << m_insertQps / 32
                    << std::endl << std::flush;});

    int update_queries = m_updateEntityCount + m_updatePropertyCount 
                         - old_update_queries;

    if (++m_updateQpsIndex >= 32) {
        m_updateQpsIndex = 0;
    }
    m_updateQps -= m_updateQpsRing[m_updateQpsIndex];
    m_updateQps += update_queries;
    m_updateQpsRing[m_updateQpsIndex] = update_queries;
    m_updateQpsAvg = m_updateQps / 32;
    m_updateQpsNow = update_queries;

    debug(if (update_queries) { std::cout << "Ups: " << update_queries << ", " << m_updateQps / 32
                    << std::endl << std::flush;});
}

void StorageManager::thoughtsReceived(const std::string& entityId, const Operation& op)
{
    m_outstandingThoughtRequests.erase(entityId);
    //Note that the received operation originated from an external mind, so we must
    // treat it as unsafe.
    if (op->getClassNo() == Atlas::Objects::Operation::THINK_NO) {
        if (op->getArgs().empty()) {
            return;
        }
        auto arg = op->getArgs().front();

        if (arg->getClassNo() != Atlas::Objects::Operation::SET_NO) {
            log(WARNING, "Got thought op where the first argument wasn't a Set op.");
        } else {
            auto setOp = Atlas::Objects::smart_dynamic_cast<Atlas::Objects::Operation::Set>(arg);
            Database * db = Database::instance();
            std::vector<std::string> thoughtsList;
            Atlas::Message::ListType thoughts = setOp->getArgsAsList();
            for (auto& thoughtElement : thoughts) {
                if (thoughtElement.isMap()) {
                    std::string value;
                    db->encodeObject(thoughtElement.asMap(), value);
                    thoughtsList.push_back(value);
                }
            }
            db->replaceThoughts(entityId, thoughtsList);
        }

    } else if (op->getClassNo()
            == Atlas::Objects::Operation::ROOT_OPERATION_NO) {
        //A RootOperation indicates that the relay timed out; we'll just ignore it
    } else {
        log(WARNING,
                String::compose(
                        "Got response to a thoughts Get request from mind %1 with an operation of type %2. This could signal a malicious client.",
                        op->getFrom(), op->getParents().front()));
    }

}

int StorageManager::initWorld()
{
    LocatedEntity * ent = &BaseWorld::instance().getRootEntity();

    ent->updated.connect(sigc::bind(sigc::mem_fun(this, &StorageManager::entityUpdated), ent));
    ent->setFlags(entity_clean);
    // FIXME queue it so the initial state gets persisted.
    return 0;
}

int StorageManager::restoreWorld()
{
    log(INFO, "Starting restoring world from storage.");
    LocatedEntity * ent = &BaseWorld::instance().getRootEntity();

    //The order here is important. We want to restore the children before we restore the properties.
    //The reason for this is that some properties (such as "outfit") refer to child entities; if
    //the child isn't present when the property is installed there will be issues.
    //We do this by first restoring the children, without any properties, and the assigning the properties to
    //all entities in order.
    restoreChildren(ent);

    restorePropertiesRecursively(ent);

    log(INFO, "Completed restoring world from storage.");
    return 0;
}

int StorageManager::shutdown(bool& exit_flag, const std::map<long, LocatedEntity *>& entites)
{
    tick();
    while (Database::instance()->queryQueueSize()) {
        //Allow for any user to abort the process.
        if(exit_flag) {
            log(NOTICE, "Aborted entity persisting. This might lead to lost entities.");
            return 0;
        }
        if (!Database::instance()->queryInProgress()) {
            Database::instance()->launchNewQuery();
        } else {
            Database::instance()->clearPendingQuery();
        }
    }
    return 0;
}

size_t StorageManager::requestMinds(const std::map<long, LocatedEntity *>& entites)
{
    size_t requests = 0;
    for (auto& pair : entites) {
        if (storeThoughts(pair.second)) {
            requests++;
        }
    }
    return requests;
}


size_t StorageManager::numberOfOutstandingThoughtRequests() const
{
    return m_outstandingThoughtRequests.size();
}


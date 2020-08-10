#include <memory>

/*
 Copyright (C) 2015 erik

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifdef HAVE_CONFIG_H
#endif

#include "AwareMind.h"
#include "AwarenessStore.h"
#include "AwarenessStoreProvider.h"
#include "SharedTerrain.h"

#include "navigation/Awareness.h"
#include "navigation/Steering.h"

#include "common/operations/Tick.h"
#include "common/debug.h"


#include <Atlas/Objects/RootEntity.h>
#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Entity.h>

#include <wfmath/atlasconv.h>
#include <chrono>

static const bool debug_flag = false;

AwareMind::AwareMind(const std::string& mind_id,
                     std::string entity_id,
                     const PropertyManager& propertyManager,
                     SharedTerrain& sharedTerrain,
                     AwarenessStoreProvider& awarenessStoreProvider) :
        BaseMind(mind_id, std::move(entity_id), propertyManager),
        mSharedTerrain(sharedTerrain),
        mAwarenessStoreProvider(awarenessStoreProvider),
        mAwarenessStore(nullptr)
{
}

AwareMind::~AwareMind()
{
    if (mAwareness && m_ownEntity) {
        auto& entities = m_map.getEntities();
        //Remove all our still known entities from the awareness
        for (const auto& entry : entities) {
            if (entry.second->m_location.m_parent == m_ownEntity->m_location.m_parent) {
                mAwareness->removeEntity(*m_ownEntity, *entry.second);
            }
        }
        mAwareness->removeAwarenessArea(getId());
        mAwareness->removeObserver();
    }
}

void AwareMind::operation(const Operation& op, OpVector& res)
{


    //If it's a "move" tick we'll process it here and won't send it on to the mind.
    if (op->getClassNo() == Atlas::Objects::Operation::TICK_NO) {
        if (!op->getArgs().empty()) {
            auto arg = op->getArgs().front();
            if (arg->getName() == "move") {
                updateServerTimeFromOperation(*op);
                processMoveTick(op, res);
                return;
            }
        }
    }

//    if (mServerTimeDiff == 0 && op->getClassNo() == Atlas::Objects::Operation::SIGHT_NO) {
//        if (op->hasAttrFlag(Atlas::Objects::Operation::SECONDS_FLAG)) {
//            double stamp = op->getSeconds();
//            mServerTimeDiff = getCurrentLocalTime() - stamp;
//        }
//    }

    BaseMind::operation(op, res);
}

double AwareMind::getCurrentLocalTime() const
{
    return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch()).count() / 1000000.0;
}

double AwareMind::getCurrentServerTime() const
{
    return mServerTime;
}

void AwareMind::processMoveTick(const Operation& op, OpVector& res)
{
    //Default to checking movement every 0.2 seconds, unless steering tells us otherwise
    double futureTick = 0.2;

    if (mAwareness) {
        auto remainingDirtyTiles = mAwareness->rebuildDirtyTile();
        if (remainingDirtyTiles > 0) {
            futureTick = 0;
        } else {
            if (mAwareness->needsPruning()) {
                mAwareness->pruneTiles();
            }
            if (mAwareness->needsPruning()) {
                futureTick = 0;
            }
        }
    }

    if (mSteering) {
        SteeringResult result = mSteering->update(op->getSeconds());
        if (result.direction.isValid()) {
            Atlas::Objects::Operation::Move move;
            Atlas::Objects::Entity::Anonymous what;
            what->setId(m_ownEntity->getId());
            what->setAttr("propel", result.direction.toAtlas());
            //log(INFO, String::compose("Moving in direction %1, %2 with velocity %3", result.direction.x(), result.direction.y(), result.direction.mag()));
            if (result.direction != WFMath::Vector<3>::ZERO()) {
                WFMath::Quaternion orientation;
                orientation.rotation(WFMath::Vector<3>(0, 0, 1), result.direction, WFMath::Vector<3>(0, 1, 0));
                if (orientation.isValid()) {
                    what->setAttr("orientation", orientation.toAtlas());
                } else {
                    log(WARNING, "Orientation to be sent in steering isn't valid.");
                }
            }
            if (result.destination.isValid()) {
                what->setAttr("pos", result.destination.toAtlas());
            }

            move->setFrom(getId());
            move->setArgs1(what);

//            if (debug_flag) {
//                std::cout << "Move arg {" << std::endl;
//                debug_dump(what, std::cout);
//                std::cout << "}" << std::endl << std::flush;
//            }

            res.push_back(move);
        }
        if (result.timeToNextWaypoint) {
            futureTick = std::min(*result.timeToNextWaypoint, futureTick);
        }
    }

    Atlas::Objects::Operation::Tick tick;
    Atlas::Objects::Entity::Anonymous arg;
    arg->setName("move");
    tick->setArgs1(arg);
    tick->setFutureSeconds(futureTick);
    tick->setTo(getId());
    tick->setFrom(getId());

    res.push_back(tick);
}

int AwareMind::updatePath()
{
    return mSteering->updatePath(getCurrentServerTime());
}

Steering* AwareMind::getSteering()
{
    return mSteering.get();
}

const std::shared_ptr<Awareness>& AwareMind::getAwareness() const
{
    return mAwareness;
}


void AwareMind::entityAdded(MemEntity& entity)
{
    BaseMind::entityAdded(entity);
    if (m_ownEntity) {
        if (mAwareness) {
//        log(INFO, String::compose("Adding entity %1", entity.getId()));
            //TODO: check if the entity is dynamic
            if (entity.m_location.m_parent == m_ownEntity->m_location.m_parent) {
                mAwareness->addEntity(*m_ownEntity, entity, false);
            }
        } else {
            //Check if we've received the current domain entity.
            if (m_ownEntity->m_location.m_parent && entity.getIntId() == m_ownEntity->m_location.m_parent->getIntId()) {
                //log(INFO, "Creating awareness.");
                requestAwareness(entity);

                auto terrainElem = entity.getAttr("terrain_points");
                if (terrainElem) {
                    parseTerrain(*terrainElem);
                }
            }
        }
    }
}

void AwareMind::requestAwareness(const MemEntity& entity)
{
    mAwareness = mAwarenessStore->requestAwareness(entity);
    mAwareness->addObserver();
    mAwareness->addEntity(*m_ownEntity, *m_ownEntity, true);
    auto& entities = m_map.getEntities();
    //Add all existing known entities that have the same parent entity as ourselves.
    for (const auto& entry : entities) {
        if (entry.first != getIntId()) {
            if (entry.second->m_location.m_parent == m_ownEntity->m_location.m_parent) {
                mAwareness->addEntity(*m_ownEntity, *entry.second, false);
            }
        }
    }
    mSteering->setAwareness(mAwareness.get());
}

void AwareMind::entityUpdated(MemEntity& entity, const Atlas::Objects::Entity::RootEntity& ent, LocatedEntity* oldLocation)
{
    BaseMind::entityUpdated(entity, ent, oldLocation);
    if (mAwareness) {
        //Update the awareness if location, position, velocity, orientation, scale or bbox has changed
        if (ent->hasAttrFlag(Atlas::Objects::Entity::LOC_FLAG)
            || ent->hasAttrFlag(Atlas::Objects::Entity::POS_FLAG)
            || ent->hasAttrFlag(Atlas::Objects::Entity::VELOCITY_FLAG)
            || ent->hasAttr("orientation")
            || ent->hasAttr("bbox")
            || ent->hasAttr("scale")) {

            if (oldLocation == entity.m_location.m_parent.get()) {
                //Location wasn't changed
                if (entity.m_location.m_parent == m_ownEntity->m_location.m_parent) {
                    //log(INFO, "Updated entity.");
                    mAwareness->updateEntityMovement(*m_ownEntity, entity);
                }
            } else {
                //Check if new location is the domain, and then add the entity
                if (entity.m_location.m_parent == m_ownEntity->m_location.m_parent) {
                    //log(INFO, "Adding entity.");
                    mAwareness->addEntity(*m_ownEntity, entity, false);
                } else if (oldLocation == m_ownEntity->m_location.m_parent.get()) {
                    //log(INFO, "Removing entity.");
                    mAwareness->removeEntity(*m_ownEntity, entity);
                }
            }

            //If it was ourselves that moved we should notify steering that it shouldn't wait any more for a movement op.
            if (entity.getIntId() == m_ownEntity->getIntId()) {
                mSteering->setIsExpectingServerMovement(false);
            }
        }
    } else {
        if (m_ownEntity) {
            if (m_ownEntity->m_location.m_parent && entity.getIntId() == m_ownEntity->m_location.m_parent->getIntId()) {
                if (ent->hasAttr("terrain")) {

                    Atlas::Message::Element terrainElement;
                    if (ent->copyAttr("terrain", terrainElement) == 0) {
                        parseTerrain(terrainElement);
                    }
                }
                if (!mAwareness) {
                    requestAwareness(entity);

                }
            }
        }
    }
}

void AwareMind::parseTerrain(const Atlas::Message::Element& terrainElement)
{
    if (terrainElement.isMap()) {
        std::vector<SharedTerrain::BasePointDefinition> pointDefs;
        auto& pointsMap = terrainElement.Map();
        for (auto& pointEntry : pointsMap) {
            if (pointEntry.second.isList()) {
                auto& pointsList = pointEntry.second.List();
                if (pointsList.size() == 3) {
                    if (!pointsList[0].isNum()) {
                        continue;
                    }
                    if (!pointsList[1].isNum()) {
                        continue;
                    }
                    if (!pointsList[2].isNum()) {
                        continue;
                    }

                    pointDefs.emplace_back(SharedTerrain::BasePointDefinition{
                            (int) pointsList[0].asNum(),
                            (int) pointsList[1].asNum(),
                            Mercator::BasePoint(static_cast<float>(pointsList[2].asNum()))
                    });
                }
            }
        }
        std::vector<SharedTerrain::BasePointDefinition> changedPoints = mSharedTerrain.setBasePoints(pointDefs);
        if (mAwareness) {
            int res = mSharedTerrain.getTerrain().getResolution();
            for (auto& entry : changedPoints) {
                WFMath::AxisBox<2> area(WFMath::Point<2>(entry.x - res, entry.y - res), WFMath::Point<2>(entry.x + res, entry.y + res));
                mAwareness->markTilesAsDirty(area);
            }
        }
    }

}

void AwareMind::entityDeleted(MemEntity& entity)
{
    BaseMind::entityDeleted(entity);
    if (mAwareness) {
        //log(INFO, "Removed entity.");
        mAwareness->removeEntity(*m_ownEntity, entity);
    }
}

//double AwareMind::getServerTimeDiff() const
//{
//    return mServerTimeDiff;
//}

void AwareMind::setOwnEntity(OpVector& res, Ref<MemEntity> ownEntity)
{
    BaseMind::setOwnEntity(res, ownEntity);

    mSteering = std::make_unique<Steering>(*ownEntity);
    mAwarenessStore = &mAwarenessStoreProvider.getStore(ownEntity->getType());

    //Start the move ticks
    Atlas::Objects::Operation::Tick tick;
    Atlas::Objects::Entity::Anonymous arg;
    arg->setName("move");
    tick->setArgs1(arg);
    tick->setTo(getId());

    res.push_back(tick);
}


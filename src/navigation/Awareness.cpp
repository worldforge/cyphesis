/*
 Copyright (C) 2014 Erik Ogenvik

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

 Some portions of this file include code taken from the OgreCrowd project, which has the copyrights and license as described below.
 These portions are the findPath() and processTiles() methods.

 OgreCrowd
 ---------

 Copyright (c) 2012 Jonas Hauquier

 Additional contributions by:

 - mkultra333
 - Paul Wilson

 Sincere thanks and to:

 - Mikko Mononen (developer of Recast navigation libraries)

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.


 */

#include "Awareness.h"
#include "AwarenessUtils.h"

#include "IHeightProvider.h"

#include "RecastDetour/Detour/Include/DetourNavMesh.h"
#include "RecastDetour/Detour/Include/DetourNavMeshQuery.h"
#include "RecastDetour/Detour/Include/DetourNavMeshBuilder.h"
#include "RecastDetour/DetourTileCache/Include/DetourTileCache.h"
#include "RecastDetour/DetourTileCache/Include/DetourTileCacheBuilder.h"
#include "RecastDetour/Detour/Include/DetourCommon.h"
#include "RecastDetour/Detour/Include/DetourObstacleAvoidance.h"

#include "common/debug.h"

#include "rules/ai/MemEntity.h"

#include "Remotery.h"

#include <wfmath/wfmath.h>

#include <Atlas/Message/Element.h>

#include <sigc++/bind.h>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/sequenced_index.hpp>

#include <cmath>
#include <vector>
#include <cstring>
#include <queue>
#include <rules/SolidProperty.h>
#include <rules/BBoxProperty.h>
#include <Atlas/Objects/RootEntity.h>
#include <wfmath/atlasconv.h>
#include <rules/PhysicalProperties.h>
#include <rules/ScaleProperty.h>

static const bool debug_flag = false;

#define MAX_PATHPOLY      256 // max number of polygons in a path
#define MAX_PATHVERT      512 // most verts in a path
#define MAX_OBSTACLES_CIRCLES 4 // max number of circle obstacles to consider when doing avoidance

// This value specifies how many layers (or "floors") each navmesh tile is expected to have.
static const int EXPECTED_LAYERS_PER_TILE = 1;

using namespace boost::multi_index;

/**
 * @brief A Most Recently Used list implemented using boost::multi_index.
 *
 */
template<typename TItem>
class MRUList
{
    public:

        void insert(TItem item)
        {
            auto p = mItems.push_front(std::move(item));

            if (!p.second) {
                mItems.relocate(mItems.begin(), p.first);
            }
        }

        TItem pop_back()
        {
            TItem back = std::move(mItems.back());
            mItems.pop_back();
            return back;
        }

        std::size_t size() const
        {
            return mItems.size();
        }

    private:
        multi_index_container<TItem, indexed_by<sequenced<>, hashed_unique<identity<TItem> > > > mItems;

};

struct InputGeometry
{
    std::vector<float> verts;
    std::vector<int> tris;
    std::vector<WFMath::RotBox<2>> entityAreas;
};

class AwarenessContext : public rcContext
{
    protected:
        void doLog(const rcLogCategory category, const char* msg, const int len) override
        {
            if (category == RC_LOG_PROGRESS) {
                ::log(INFO, String::compose("Recast: %1", msg));
            } else if (category == RC_LOG_WARNING) {
                ::log(WARNING, String::compose("Recast: %1", msg));
            } else {
                ::log(ERROR, String::compose("Recast: %1", msg));
            }
        }

};

Awareness::Awareness(long domainEntityId,
                     float agentRadius,
                     float agentHeight,
                     float stepHeight,
                     IHeightProvider& heightProvider,
                     const WFMath::AxisBox<3>& extent,
                     int tileSize) :
        mHeightProvider(heightProvider),
        mDomainEntityId(domainEntityId),
        mTalloc(nullptr),
        mTcomp(nullptr),
        mTmproc(nullptr),
        mAgentRadius(agentRadius),
        mStepHeight(stepHeight),
        mBaseTileAmount(128),
        mDesiredTilesAmount(128),
        mCtx(new AwarenessContext()),
        mCfg{},
        mTileCache(nullptr),
        mNavMesh(nullptr),
        mNavQuery(dtAllocNavMeshQuery()),
        mFilter(new dtQueryFilter()),
        mActiveTileList(new MRUList<std::pair<int, int>>()),
        mObserverCount(0)
{
    auto validExtent = extent;
    if (!extent.isValid()) {
        ::log(WARNING, "No valid extent, will default to small area");
        validExtent = {{-100, -100, -100},
                       {100,  100,  100}};
    }
    debug_print("Creating awareness with extent " << extent << " and agent radius " << agentRadius)
    try {
        auto talloc = std::make_unique<LinearAllocator>(128000);
        auto tcomp = std::make_unique<FastLZCompressor>();
        auto tmproc = std::make_unique<MeshProcess>();

        // Setup the default query filter
        mFilter->setIncludeFlags(0xFFFF); // Include all
        mFilter->setExcludeFlags(0); // Exclude none
        // Area flags for polys to consider in search, and their cost
        mFilter->setAreaCost(POLYAREA_GROUND, 1.0f);

        const WFMath::Point<3>& lower = validExtent.lowCorner();
        const WFMath::Point<3>& upper = validExtent.highCorner();

        mCfg.bmin[0] = lower.x();
        mCfg.bmin[1] = std::min(-500.0, lower.y());
        mCfg.bmin[2] = lower.z();
        mCfg.bmax[0] = upper.x();
        mCfg.bmax[1] = std::max(500.0, upper.y());
        mCfg.bmax[2] = upper.z();

        int gw = 0, gh = 0;
        float cellsize = mAgentRadius / 2.0f; //Should be enough for outdoors; indoors we might want r / 3.0 instead
        rcCalcGridSize(mCfg.bmin, mCfg.bmax, cellsize, &gw, &gh);
        const int tilewidth = (gw + tileSize - 1) / tileSize;
        const int tileheight = (gh + tileSize - 1) / tileSize;

        // Max tiles and max polys affect how the tile IDs are caculated.
        // There are 22 bits available for identifying a tile and a polygon.
        int tileBits = rcMin((int) dtIlog2(dtNextPow2(tilewidth * tileheight * EXPECTED_LAYERS_PER_TILE)), 14);
        if (tileBits > 14) {
            tileBits = 14;
        }
        int polyBits = 22 - tileBits;
        unsigned int maxTiles = 1u << tileBits;
        unsigned int maxPolysPerTile = 1u << polyBits;

        //For an explanation of these values see http://digestingduck.blogspot.se/2009/08/recast-settings-uncovered.html

        mCfg.cs = cellsize;
        mCfg.ch = mCfg.cs / 2.0f; //Height of one voxel; should really only come into play when doing 3d traversal
        //	m_cfg.ch = std::max(upper.z() - lower.z(), 100.0f); //For 2d traversal make the voxel size as large as possible
        mCfg.walkableHeight = std::max(3, static_cast<int>(std::ceil(agentHeight / mCfg.ch))); //This is in voxels
        mCfg.walkableClimb = std::ceil(stepHeight / mCfg.ch); //This is in voxels
        mCfg.walkableRadius = std::ceil(mAgentRadius / mCfg.cs);
        mCfg.walkableSlopeAngle = 70; //TODO: implement proper system for limiting climbing; for now just use 70 degrees

        mCfg.maxEdgeLen = (int) (mCfg.walkableRadius * 8.0f);
        mCfg.maxSimplificationError = 1.3f;
        mCfg.minRegionArea = (int) rcSqr(8);
        mCfg.mergeRegionArea = (int) rcSqr(20);

        mCfg.tileSize = tileSize;
        mCfg.borderSize = mCfg.walkableRadius + 3; // Reserve enough padding.
        mCfg.width = mCfg.tileSize + mCfg.borderSize * 2;
        mCfg.height = mCfg.tileSize + mCfg.borderSize * 2;
        //	m_cfg.detailSampleDist = m_detailSampleDist < 0.9f ? 0 : m_cfg.cs * m_detailSampleDist;
        //	m_cfg.detailSampleMaxError = m_cfg.m_cellHeight * m_detailSampleMaxError;

        // Tile cache params.
        dtTileCacheParams tcparams{};
        memset(&tcparams, 0, sizeof(tcparams));
        rcVcopy(tcparams.orig, mCfg.bmin);
        tcparams.cs = mCfg.cs;
        tcparams.ch = mCfg.ch;
        tcparams.width = (int) mCfg.tileSize;
        tcparams.height = (int) mCfg.tileSize;
        tcparams.walkableHeight = agentHeight;
        tcparams.walkableRadius = mAgentRadius;
        tcparams.walkableClimb = mCfg.walkableClimb;
        //	tcparams.maxSimplificationError = m_edgeMaxError;
        tcparams.maxTiles = tilewidth * tileheight * EXPECTED_LAYERS_PER_TILE;
        tcparams.maxObstacles = 128;

        dtFreeTileCache(mTileCache);

        dtStatus status;

        mTileCache = dtAllocTileCache();
        if (!mTileCache) {
            throw std::runtime_error("buildTiledNavigation: Could not allocate tile cache.");
        }
        status = mTileCache->init(&tcparams, talloc.get(), tcomp.get(), tmproc.get());
        if (dtStatusFailed(status)) {
            throw std::runtime_error("buildTiledNavigation: Could not init tile cache.");
        }

        dtFreeNavMesh(mNavMesh);

        mNavMesh = dtAllocNavMesh();
        if (!mNavMesh) {
            throw std::runtime_error("buildTiledNavigation: Could not allocate navmesh.");
        }

        dtNavMeshParams params{};
        memset(&params, 0, sizeof(params));
        rcVcopy(params.orig, mCfg.bmin);
        params.tileWidth = tileSize * cellsize;
        params.tileHeight = tileSize * cellsize;
        params.maxTiles = maxTiles;
        params.maxPolys = maxPolysPerTile;

        status = mNavMesh->init(&params);
        if (dtStatusFailed(status)) {
            throw std::runtime_error("buildTiledNavigation: Could not init navmesh.");
        }

        status = mNavQuery->init(mNavMesh, 2048);
        if (dtStatusFailed(status)) {
            throw std::runtime_error("buildTiledNavigation: Could not init Detour navmesh query");
        }

        mObstacleAvoidanceQuery = dtAllocObstacleAvoidanceQuery();
        mObstacleAvoidanceQuery->init(MAX_OBSTACLES_CIRCLES, 0);

        mObstacleAvoidanceParams = std::make_unique<dtObstacleAvoidanceParams>();
        mObstacleAvoidanceParams->velBias = 0.5f;
        mObstacleAvoidanceParams->weightDesVel = 2.0f;
        mObstacleAvoidanceParams->weightCurVel = 0.75f;
        mObstacleAvoidanceParams->weightSide = 0.75f;
        mObstacleAvoidanceParams->weightToi = 2.5f;
        mObstacleAvoidanceParams->horizTime = 2.5f;
        mObstacleAvoidanceParams->gridSize = 33;
        mObstacleAvoidanceParams->adaptiveDivs = 7;
        mObstacleAvoidanceParams->adaptiveRings = 2;
        mObstacleAvoidanceParams->adaptiveDepth = 3;
        mTalloc = std::move(talloc);
        mTcomp = std::move(tcomp);
        mTmproc = std::move(tmproc);

    } catch (const std::exception& e) {
        dtFreeObstacleAvoidanceQuery(mObstacleAvoidanceQuery);

        dtFreeNavMesh(mNavMesh);
        dtFreeNavMeshQuery(mNavQuery);

        dtFreeTileCache(mTileCache);

        throw;
    }
}

Awareness::~Awareness()
{

    dtFreeObstacleAvoidanceQuery(mObstacleAvoidanceQuery);

    dtFreeNavMesh(mNavMesh);
    dtFreeNavMeshQuery(mNavQuery);

    dtFreeTileCache(mTileCache);

}

void Awareness::addObserver()
{
    mObserverCount++;
    mDesiredTilesAmount = mBaseTileAmount + ((double) (mObserverCount - 1) * (mBaseTileAmount * 0.4));
}

void Awareness::removeObserver()
{
    mObserverCount--;
    if (mObserverCount == 0) {
        mDesiredTilesAmount = mBaseTileAmount;
    } else {
        mDesiredTilesAmount = mBaseTileAmount + ((double) (mObserverCount - 1) * (mBaseTileAmount * 0.4));
    }
}

void Awareness::updateEntity(const MemEntity& observer, const MemEntity& entity, const Atlas::Objects::Entity::RootEntity& ent)
{
    auto I = mObservedEntities.find(entity.getIntId());
    if (I != mObservedEntities.end()) {
        if (!entity.m_parent || entity.m_parent->getIntId() != mDomainEntityId) {
            removeEntity(observer, entity);
        } else {
            processEntityUpdate(*I->second, entity, ent, entity.m_lastUpdated);
        }
    } else {
        if (entity.m_parent && entity.m_parent->getIntId() == mDomainEntityId) {
            addEntity(observer, entity, true);
        }
    }
}


void Awareness::addEntity(const MemEntity& observer, const MemEntity& entity, bool isDynamic)
{
    rmt_ScopedCPUSample(Awareness_addEntity, 0)
    auto I = mObservedEntities.find(entity.getIntId());
    if (I == mObservedEntities.end()) {
        std::unique_ptr<EntityEntry> entityEntry(new EntityEntry());
        entityEntry->entityId = entity.getIntId();
        entityEntry->numberOfObservers = 1;
        auto bboxProp = entity.getPropertyClassFixed<BBoxProperty>();
        entityEntry->isIgnored = !bboxProp || !bboxProp->data().isValid();
        entityEntry->isMoving = isDynamic;
        entityEntry->isActorOwned = false;
        auto solidPropery = entity.getPropertyClassFixed<SolidProperty>();
        //TODO: handle the entity changing solid status
        entityEntry->isSolid = !solidPropery || solidPropery->isTrue();
        if (isDynamic) {
            mMovingEntities.insert(entityEntry.get());
        }
        I = mObservedEntities.emplace(entity.getIntId(), std::move(entityEntry)).first;
        debug_print("Creating new entry for " << entity.getId())
    } else {
        I->second->numberOfObservers++;
    }

    //Entity already exists; check if it's the same as the observer and marked it as owned.
    if (I->first == observer.getIntId()) {
        I->second->isActorOwned = true;
    }

    bool isNotActorAndFirstSeen = !I->second->isActorOwned && I->second->numberOfObservers == 1;
    bool isOwnEntity = I->first == observer.getIntId();

    //Only do movement change processing if this is the first observer; otherwise that should already have been done
    //Or if the entity is the actor`s own entity.
    if (isNotActorAndFirstSeen || isOwnEntity) {
        processEntityUpdate(*I->second, entity, nullptr, entity.m_lastUpdated);
    }

}

void Awareness::removeEntity(const MemEntity& observer, const MemEntity& entity)
{
    auto I = mObservedEntities.find(entity.getIntId());
    if (I != mObservedEntities.end()) {
        debug_print("Removing entity " << entity.getId())
        //Decrease the number of observers, and delete entry if there's none left
        auto& entityEntry = I->second;
        if (entityEntry->numberOfObservers == 0) {
            log(WARNING, String::compose("Entity entry %1 has decreased number of observers to < 0. This indicates an error.", entity.getId()));
        }
        entityEntry->numberOfObservers--;
        if (entityEntry->numberOfObservers == 0) {
            if (entityEntry->isMoving) {
                mMovingEntities.erase(entityEntry.get());
            }
            auto areasI = mEntityAreas.find(entityEntry.get());
            if (areasI != mEntityAreas.end()) {
                if (areasI->second.isValid()) {
                    markTilesAsDirty(areasI->second.boundingBox());
                }
                mEntityAreas.erase(areasI);
            }
            mObservedEntities.erase(I);
        } else {
            //If the entity and the observer are the same we need to remove the marking of the entry being owned by an actor.
            if (observer.getIntId() == entity.getIntId()) {
                entityEntry->isActorOwned = false;
            }
        }
    }
}

bool Awareness::processEntityUpdate(EntityEntry& entityEntry, const MemEntity& entity, const Atlas::Objects::Entity::RootEntity& ent, double timestamp)
{
    bool hasNewPosition = false;
    bool hasNewBbox = false;
    bool hasNewMovement = false;
    if (!ent || ent->hasAttrFlag(Atlas::Objects::Entity::POS_FLAG)) {
        if (timestamp > entityEntry.pos.timestamp) {
            if (auto prop = entity.getPropertyClassFixed<PositionProperty>()) {
                entityEntry.pos.data = prop->data();
                entityEntry.pos.timestamp = timestamp;
                hasNewPosition = true;
            }
        }
    }
    if (!ent || ent->hasAttrFlag(Atlas::Objects::Entity::VELOCITY_FLAG)) {
        if (timestamp > entityEntry.velocity.timestamp) {
            if (auto prop = entity.getPropertyClassFixed<VelocityProperty>()) {
                entityEntry.velocity.data = prop->data();
                entityEntry.velocity.timestamp = timestamp;
                hasNewMovement = true;
            }
        }
    }
    if (!ent || ent->hasAttr("orientation")) {
        if (timestamp > entityEntry.orientation.timestamp) {
            if (auto prop = entity.getPropertyClassFixed<OrientationProperty>()) {
                entityEntry.orientation.data = prop->data();
                entityEntry.orientation.timestamp = timestamp;
                hasNewPosition = true;
            }
        }
    }
    if (!ent || ent->hasAttr("angular")) {
        if (timestamp > entityEntry.angular.timestamp) {
            if (auto prop = entity.getPropertyClassFixed<AngularVelocityProperty>()) {
                entityEntry.angular.data = prop->data();
                entityEntry.angular.timestamp = timestamp;
                hasNewMovement = true;
            }
        }
    }
    if (!ent || ent->hasAttr("bbox")) {
        if (timestamp > entityEntry.bbox.timestamp) {
            if (auto prop = entity.getPropertyClassFixed<BBoxProperty>()) {
                entityEntry.bbox.data = prop->data();
                entityEntry.bbox.timestamp = timestamp;
                hasNewBbox = true;
            }
        }
    }
    if (!ent || ent->hasAttr("scale")) {
        if (timestamp > entityEntry.scale.timestamp) {
            if (auto prop = entity.getPropertyClassFixed<ScaleProperty>()) {
                entityEntry.scale.data = prop->data();
                entityEntry.scale.timestamp = timestamp;
                hasNewBbox = true;
            }
        }
    }


    if (hasNewBbox) {
        if (entityEntry.bbox.data.isValid()) {
            if (entityEntry.scale.data.isValid()) {
                entityEntry.scaledBbox = ScaleProperty::scaledBbox(entityEntry.bbox.data, entityEntry.scale.data);
            } else {
                entityEntry.scaledBbox = entityEntry.bbox.data;
            }
        }
    }




    //If entity already is moving we just need to update its location
    if (entityEntry.isMoving) {
    } else {
        //If it previously was ignored, see if it shouldn't be that anymore.
        if (entityEntry.isIgnored) {
            if (hasNewBbox) {
                entityEntry.isIgnored = !entityEntry.scaledBbox.isValid();
            }
        }

        if (!entityEntry.isIgnored) {
            //Check if the bbox now is invalid
            if (!entityEntry.bbox.data.isValid()) {
                debug_print("Ignoring entity " << entity.getId() << " because it has no valid bbox.")
                entityEntry.isIgnored = true;

                //We must now mark those areas that the entities used to touch as dirty, as well as remove the entity areas
                auto existingI = mEntityAreas.find(&entityEntry);
                if (existingI != mEntityAreas.end()) {
                    //The entity already was registered; mark those tiles where the entity previously were as dirty.
                    markTilesAsDirty(existingI->second.boundingBox());
                    mEntityAreas.erase(&entityEntry);
                }
            } else {

                //Only update if there's a change
                if (hasNewPosition || hasNewBbox) {

                    debug_print("Updating entity location for entity " << entityEntry.entityId)

                    //If an entity which previously didn't move start moving we need to move it to the "movable entities" collection.
                    if (entityEntry.velocity.data.isValid() && entityEntry.velocity.data != WFMath::Vector<3>::ZERO()) {
                        debug_print("Entity is now moving.")
                        mMovingEntities.insert(&entityEntry);
                        entityEntry.isMoving = true;
                        auto existingI = mEntityAreas.find(&entityEntry);
                        if (existingI != mEntityAreas.end()) {
                            //The entity already was registered; mark those tiles where the entity previously were as dirty.
                            markTilesAsDirty(existingI->second.boundingBox());
                            mEntityAreas.erase(&entityEntry);
                        }
                    } else {
                        auto area = buildEntityAreas(entityEntry);

                        if (area.isValid()) {
                            auto bbox = area.boundingBox();
                            auto existingI = mEntityAreas.find(&entityEntry);
                            //Check if it was a minor change; if so we might keep the old entry without much effect on navigation.
                            bool isLargeEnoughChange = true;
                            if (existingI != mEntityAreas.end()) {
                                auto existingBbox = existingI->second.boundingBox();
                                if (WFMath::Distance(existingBbox.lowCorner(), bbox.lowCorner()) > 0.1 || WFMath::Distance(existingBbox.highCorner(), bbox.highCorner()) > 0.1) {
                                    isLargeEnoughChange = true;
                                } else {
                                    isLargeEnoughChange = false;
                                }
                            }
                            if (isLargeEnoughChange) {
                                markTilesAsDirty(area.boundingBox());
                                if (existingI != mEntityAreas.end()) {
                                    //The entity already was registered; mark both those tiles where the entity previously were as well as the new tiles as dirty.
                                    markTilesAsDirty(existingI->second.boundingBox());
                                    existingI->second = area;
                                } else {
                                    mEntityAreas.emplace(&entityEntry, area);
                                }
                            }
                        }
                        debug_print("Entity affects " << area << ". Dirty unaware tiles: " << mDirtyUnwareTiles.size() << " Dirty aware tiles: " << mDirtyAwareTiles.size())
                    }

                }
            }
        }
    }
    return hasNewBbox || hasNewPosition;
}

bool Awareness::avoidObstacles(long avatarEntityId,
                               const WFMath::Point<2>& position,
                               const WFMath::Vector<2>& desiredVelocity,
                               WFMath::Vector<2>& newVelocity,
                               double currentTimestamp,
                               const WFMath::Point<2>* nextWayPoint) const
{
    struct EntityCollisionEntry
    {
        float distance;
        const EntityEntry* entity;
        WFMath::Point<2> viewPosition;
        WFMath::Ball<2> viewRadius;
    };

    if (nextWayPoint) {
        //Check the time to next waypoint and clamp at that
        auto distanceToNextWaypoint = WFMath::Distance(position, *nextWayPoint);
        auto timeToNextWaypoint = distanceToNextWaypoint / desiredVelocity.mag();
        mObstacleAvoidanceParams->horizTime = std::min(2.5, timeToNextWaypoint);
    } else {
        mObstacleAvoidanceParams->horizTime = 2.5;
    }

    auto comp = [](EntityCollisionEntry& a, EntityCollisionEntry& b) { return a.distance < b.distance; };
    std::priority_queue<EntityCollisionEntry, std::vector<EntityCollisionEntry>, decltype(comp)> nearestEntities(comp);

    WFMath::Ball<2> playerRadius(position, 5);

    for (auto& entry : mMovingEntities) {

        //All of the entities have the same location as we have, so we don't need to resolve the position in the world.

        if (entry->entityId == avatarEntityId) {
            //Don't avoid ourselves.
            continue;
        }


        // Update location
        auto pos = entry->pos.data;
        if (entry->velocity.data.isValid()) {
            double time_diff = currentTimestamp - entry->velocity.timestamp;
            pos += (entry->velocity.data * time_diff);
        }

        if (!pos.isValid()) {
            continue;
        }

        WFMath::Point<2> entityView2dPos(pos.x(), pos.z());
        WFMath::Ball<2> entityViewRadius(entityView2dPos, (entry->bbox.data.highCorner().x() - entry->bbox.data.lowCorner().z()) * 0.5);
        //WFMath::Ball<2> entityViewRadius(entityView2dPos, (entity->location.bBox().highCorner().x() - entity->location.bBox().lowCorner().z()));

        if (WFMath::Intersect(playerRadius, entityViewRadius, false) || WFMath::Contains(playerRadius, entityViewRadius, false)) {
            nearestEntities.push(EntityCollisionEntry({static_cast<float>(WFMath::Distance(position, entityView2dPos)), entry, entityView2dPos, entityViewRadius}));
        }

    }

    if (!nearestEntities.empty()) {
        mObstacleAvoidanceQuery->reset();
        int i = 0;
        while (!nearestEntities.empty() && i < MAX_OBSTACLES_CIRCLES) {
            const EntityCollisionEntry& entry = nearestEntities.top();
            auto& entity = entry.entity;
            float pos[]{static_cast<float>(entry.viewPosition.x()), 0, static_cast<float>(entry.viewPosition.y())};
            float vel[]{static_cast<float>(entity->velocity.data.x()), 0, static_cast<float>(entity->velocity.data.z())};
            mObstacleAvoidanceQuery->addCircle(pos, entry.viewRadius.radius(), vel, vel);
            nearestEntities.pop();
            ++i;
        }

        float pos[]{static_cast<float>(position.x()), 0, static_cast<float>(position.y())};
        float vel[]{static_cast<float>(desiredVelocity.x()), 0, static_cast<float>(desiredVelocity.y())};
        float dvel[]{static_cast<float>(desiredVelocity.x()), 0, static_cast<float>(desiredVelocity.y())};
        float nvel[]{0, 0, 0};
        float desiredSpeed = desiredVelocity.mag();

//        dtObstacleAvoidanceDebugData debug{};
//        debug.init(2000);
        //Double the agent radius to give us some leeway.
        int samples = mObstacleAvoidanceQuery->sampleVelocityAdaptive(pos, mAgentRadius * 2, desiredSpeed, vel, dvel, nvel, mObstacleAvoidanceParams.get(), nullptr);
        if (samples > 0) {
            if (!WFMath::Equal(vel[0], nvel[0]) || !WFMath::Equal(vel[2], nvel[2])) {

                newVelocity.x() = nvel[0];
                newVelocity.y() = nvel[2];
                newVelocity.setValid(true);
                return true;
            }
        }

    }
    return false;

}

void Awareness::markTilesAsDirty(const WFMath::AxisBox<2>& area)
{
    int tileMinXIndex, tileMaxXIndex, tileMinYIndex, tileMaxYIndex;
    findAffectedTiles(area, tileMinXIndex, tileMaxXIndex, tileMinYIndex, tileMaxYIndex);
    markTilesAsDirty(tileMinXIndex, tileMaxXIndex, tileMinYIndex, tileMaxYIndex);
}

void Awareness::markTilesAsDirty(int tileMinXIndex, int tileMaxXIndex, int tileMinYIndex, int tileMaxYIndex)
{
    bool wereDirtyTiles = !mDirtyAwareTiles.empty();

    for (int tx = tileMinXIndex; tx <= tileMaxXIndex; ++tx) {
        for (int ty = tileMinYIndex; ty <= tileMaxYIndex; ++ty) {
            std::pair<int, int> index(tx, ty);
            if (mAwareTiles.find(index) != mAwareTiles.end()) {
                if (mDirtyAwareTiles.insert(index).second) {
                    mDirtyAwareOrderedTiles.push_back(index);
                }
            } else {
                mDirtyUnwareTiles.insert(index);
            }
        }
    }
    debug_print("Marking tiles as dirty. Aware: " << mDirtyAwareTiles.size() << " Unaware: " << mDirtyUnwareTiles.size())
    if (!wereDirtyTiles && !mDirtyAwareTiles.empty()) {
        EventTileDirty();
    }
}

size_t Awareness::rebuildDirtyTile()
{
    if (!mDirtyAwareTiles.empty()) {
        debug_print("Rebuilding aware tiles. Number of dirty aware tiles: " << mDirtyAwareTiles.size())
        rmt_ScopedCPUSample(rebuildDirtyTile, 0)
        const auto tileIndexI = mDirtyAwareOrderedTiles.begin();
        const auto& tileIndex = *tileIndexI;

        float tilesize = mCfg.tileSize * mCfg.cs;
        WFMath::AxisBox<2> adjustedArea(WFMath::Point<2>(mCfg.bmin[0] + (tileIndex.first * tilesize), mCfg.bmin[2] + (tileIndex.second * tilesize)),
                                        WFMath::Point<2>(mCfg.bmin[0] + ((tileIndex.first + 1) * tilesize), mCfg.bmin[2] + ((tileIndex.second + 1) * tilesize)));

        std::vector<WFMath::RotBox<2>> entityAreas;
        findEntityAreas(adjustedArea, entityAreas);

        rebuildTile(tileIndex.first, tileIndex.second, entityAreas);
        mDirtyAwareTiles.erase(tileIndex);
        mDirtyAwareOrderedTiles.erase(tileIndexI);
    }
    return mDirtyAwareTiles.size();
}

void Awareness::pruneTiles()
{
    //remove any tiles that aren't used
    if (mActiveTileList->size() > mAwareTiles.size()) {
        if (mActiveTileList->size() > mDesiredTilesAmount) {
            //debug_print("Pruning tiles. Number of active tiles: " << mActiveTileList->size() << ". Number of aware tiles: " << mAwareTiles.size() << " Desired amount: " << mDesiredTilesAmount);
            std::pair<int, int> entry = mActiveTileList->pop_back();

            dtCompressedTileRef tilesRefs[MAX_LAYERS];
            const int ntiles = mTileCache->getTilesAt(entry.first, entry.second, tilesRefs, MAX_LAYERS);
            for (int i = 0; i < ntiles; ++i) {
                const dtCompressedTile* tile = mTileCache->getTileByRef(tilesRefs[i]);
                float min[3];
                int tx = tile->header->tx;
                int ty = tile->header->ty;
                int tlayer = tile->header->tlayer;
                rcVcopy(min, tile->header->bmin);
                mTileCache->removeTile(tilesRefs[i], nullptr, nullptr);
                mNavMesh->removeTile(mNavMesh->getTileRefAt(tx, ty, tlayer), 0, 0);

                EventTileRemoved(tx, ty, tlayer);
            }

        }
    }
}

bool Awareness::needsPruning() const
{
    return (mActiveTileList->size() > mDesiredTilesAmount) && (mActiveTileList->size() > mAwareTiles.size());
}

void Awareness::setDesiredTilesAmount(size_t amount)
{
    mDesiredTilesAmount = amount;
}

float Awareness::getTileSizeInMeters() const
{
    return mCfg.tileSize * mCfg.cs;
}

bool Awareness::isPositionAware(float x, float y) const
{
    float tilesize = mCfg.tileSize * mCfg.cs;
    std::pair<int, int> tileIndex((x - mCfg.bmin[0]) / tilesize, (y - mCfg.bmin[2]) / tilesize);
    return mAwareTiles.find(tileIndex) != mAwareTiles.end();
}


void Awareness::findAffectedTiles(const WFMath::AxisBox<2>& area, int& tileMinXIndex, int& tileMaxXIndex, int& tileMinZIndex, int& tileMaxZIndex) const
{
    float tilesize = mCfg.tileSize * mCfg.cs;
    WFMath::Point<2> lowCorner = area.lowCorner();
    WFMath::Point<2> highCorner = area.highCorner();

    if (lowCorner.x() < mCfg.bmin[0]) {
        lowCorner.x() = mCfg.bmin[0];
    }
    if (lowCorner.y() < mCfg.bmin[2]) {
        lowCorner.y() = mCfg.bmin[2];
    }
    if (lowCorner.x() > mCfg.bmax[0]) {
        lowCorner.x() = mCfg.bmax[0];
    }
    if (lowCorner.y() > mCfg.bmax[2]) {
        lowCorner.y() = mCfg.bmax[2];
    }

    if (highCorner.x() < mCfg.bmin[0]) {
        highCorner.x() = mCfg.bmin[0];
    }
    if (highCorner.y() < mCfg.bmin[2]) {
        highCorner.y() = mCfg.bmin[2];
    }
    if (highCorner.x() > mCfg.bmax[0]) {
        highCorner.x() = mCfg.bmax[0];
    }
    if (highCorner.y() > mCfg.bmax[2]) {
        highCorner.y() = mCfg.bmax[2];
    }

    tileMinXIndex = (lowCorner.x() - mCfg.bmin[0]) / tilesize;
    tileMaxXIndex = (highCorner.x() - mCfg.bmin[0]) / tilesize;
    tileMinZIndex = (lowCorner.y() - mCfg.bmin[2]) / tilesize;
    tileMaxZIndex = (highCorner.y() - mCfg.bmin[2]) / tilesize;
}

int Awareness::findPath(const WFMath::Point<3>& start, const WFMath::Point<3>& end, float radius, std::vector<WFMath::Point<3>>& path) const
{

    float pStartPos[]{static_cast<float>(start.x()), static_cast<float>(start.y()), static_cast<float>(start.z())};
    float pEndPos[]{static_cast<float>(end.x()), static_cast<float>(end.y()), static_cast<float>(end.z())};
    float startExtent[]{5, 100, 5}; //Only extend radius in horizontal plane
    //To make sure that the agent can move close enough we need to subtract the agent's radius from the destination radius.
    //We'll also adjust with 0.95 to allow for some padding.
    //float destinationRadius = (radius - mAgentRadius) * 0.95f;
    float endExtent[]{5, 100, 5}; //Only extend radius in horizontal plane


    dtStatus status;
    dtPolyRef StartPoly;
    float StartNearest[3];
    dtPolyRef EndPoly;
    float EndNearest[3];
    dtPolyRef PolyPath[MAX_PATHPOLY];
    int nPathCount = 0;
    float StraightPath[MAX_PATHVERT * 3];
    int nVertCount = 0;

// find the start polygon
    status = mNavQuery->findNearestPoly(pStartPos, startExtent, mFilter.get(), &StartPoly, StartNearest);
    if ((status & DT_FAILURE) || StartPoly == 0) {
        return -1;
    } // couldn't find a polygon

// find the end polygon
    status = mNavQuery->findNearestPoly(pEndPos, endExtent, mFilter.get(), &EndPoly, EndNearest);
    if ((status & DT_FAILURE) || EndPoly == 0) {
        return -2;
    } // couldn't find a polygon

    status = mNavQuery->findPath(StartPoly, EndPoly, StartNearest, EndNearest, mFilter.get(), PolyPath, &nPathCount, MAX_PATHPOLY);
    if ((status & DT_FAILURE)) {
        return -3;
    } // couldn't create a path
    if (nPathCount == 0) {
        return -4;
    } // couldn't find a path

    status = mNavQuery->findStraightPath(StartNearest, EndNearest, PolyPath, nPathCount, StraightPath, nullptr, nullptr, &nVertCount, MAX_PATHVERT);
    if ((status & DT_FAILURE)) {
        return -5;
    } // couldn't create a path
    if (nVertCount == 0) {
        return -6;
    } // couldn't find a path

    // At this point we have our path. Skip the first point since it's where we are now
    path.resize(static_cast<unsigned long>(nVertCount - 1));
    for (int nVert = 1; nVert < nVertCount; nVert++) {
        path[nVert - 1] = {StraightPath[nVert * 3], StraightPath[(nVert * 3) + 1], StraightPath[(nVert * 3) + 2]};
    }

    return nVertCount - 1;
}

bool Awareness::projectPosition(long entityId, WFMath::Point<3>& pos, double currentServerTimestamp) const
{
    auto entityI = mObservedEntities.find(entityId);
    if (entityI != mObservedEntities.end()) {
        auto& entityEntry = entityI->second;
        pos = entityEntry->pos.data;
        auto& velocity = entityEntry->velocity.data;
        if (velocity.isValid() && velocity != WFMath::Vector<3>::ZERO()) {
            pos += (velocity * (currentServerTimestamp - entityEntry->pos.timestamp));
        }
        return true;
    }
    return false;
}

WFMath::Point<3> Awareness::projectPosition(long entityId, double currentServerTimestamp) const
{
    auto entityI = mObservedEntities.find(entityId);
    if (entityI != mObservedEntities.end()) {
        auto& entityEntry = entityI->second;
        auto pos = entityEntry->pos.data;
        auto& velocity = entityEntry->velocity.data;
        if (velocity.isValid() && velocity != WFMath::Vector<3>::ZERO()) {
            pos += (velocity * (currentServerTimestamp - entityEntry->pos.timestamp));
        }
        return pos;
    }
    return {};
}

const std::unordered_map<long, std::unique_ptr<EntityEntry>>& Awareness::getObservedEntities() const
{
    return mObservedEntities;
}


void Awareness::setAwarenessArea(const std::string& areaId, const WFMath::RotBox<2>& area, const WFMath::Segment<2>& focusLine)
{

    auto& awareAreaSet = mAwareAreas[areaId];

    std::set<std::pair<int, int>> newAwareAreaSet;

    WFMath::AxisBox<2> axisbox = area.boundingBox();

//adjust area to fit with tiles

    float tilesize = mCfg.tileSize * mCfg.cs;
    WFMath::Point<2> lowCorner = axisbox.lowCorner();
    WFMath::Point<2> highCorner = axisbox.highCorner();

    if (lowCorner.x() < mCfg.bmin[0]) {
        lowCorner.x() = mCfg.bmin[0];
    }
    if (lowCorner.y() < mCfg.bmin[2]) {
        lowCorner.y() = mCfg.bmin[2];
    }
    if (lowCorner.x() > mCfg.bmax[0]) {
        lowCorner.x() = mCfg.bmax[0];
    }
    if (lowCorner.y() > mCfg.bmax[2]) {
        lowCorner.y() = mCfg.bmax[2];
    }

    if (highCorner.x() < mCfg.bmin[0]) {
        highCorner.x() = mCfg.bmin[0];
    }
    if (highCorner.y() < mCfg.bmin[2]) {
        highCorner.y() = mCfg.bmin[2];
    }
    if (highCorner.x() > mCfg.bmax[0]) {
        highCorner.x() = mCfg.bmax[0];
    }
    if (highCorner.y() > mCfg.bmax[2]) {
        highCorner.y() = mCfg.bmax[2];
    }

    int tileMinXIndex = static_cast<int>((lowCorner.x() - mCfg.bmin[0]) / tilesize);
    int tileMaxXIndex = static_cast<int>((highCorner.x() - mCfg.bmin[0]) / tilesize);
    int tileMinZIndex = static_cast<int>((lowCorner.y() - mCfg.bmin[2]) / tilesize);
    int tileMaxZIndex = static_cast<int>((highCorner.y() - mCfg.bmin[2]) / tilesize);

//Now mark tiles
    const float tcs = mCfg.tileSize * mCfg.cs;
    const float tileBorderSize = mCfg.borderSize * mCfg.cs;

    bool wereDirtyTiles = !mDirtyAwareTiles.empty();
    for (int tx = tileMinXIndex; tx <= tileMaxXIndex; ++tx) {
        for (int tz = tileMinZIndex; tz <= tileMaxZIndex; ++tz) {
            // Tile bounds.
            WFMath::AxisBox<2> tileBounds(WFMath::Point<2>((mCfg.bmin[0] + tx * tcs) - tileBorderSize, (mCfg.bmin[2] + tz * tcs) - tileBorderSize),
                                          WFMath::Point<2>((mCfg.bmin[0] + (tx + 1) * tcs) + tileBorderSize, (mCfg.bmin[2] + (tz + 1) * tcs) + tileBorderSize));
            if (WFMath::Intersect(area, tileBounds, false) || WFMath::Contains(area, tileBounds, false)) {

                std::pair<int, int> index(tx, tz);

                newAwareAreaSet.insert(index);
                //If true we should insert in the front of the dirty tiles list.
                bool insertFront = false;
                //If true we should insert in the back of the dirty tiles list.
                bool insertBack = false;
                //If the tile was marked as dirty in the old aware tiles, retain it as such
                if (mDirtyAwareTiles.find(index) != mDirtyAwareTiles.end()) {
                    if (focusLine.isValid() && WFMath::Intersect(focusLine, tileBounds, false)) {
                        insertFront = true;
                    } else {
                        insertBack = true;
                    }
                } else if (mDirtyUnwareTiles.find(index) != mDirtyUnwareTiles.end()) {
                    //if the tile was marked as dirty in the unaware tiles we'll move it to the dirty aware collection.
                    if (focusLine.isValid() && WFMath::Intersect(focusLine, tileBounds, false)) {
                        insertFront = true;
                    } else {
                        insertBack = true;
                    }
                } else {
                    //The tile wasn't marked as dirty in any set, but it might be that it hasn't been processed before.
                    auto tile = mTileCache->getTileAt(tx, tz, 0);
                    if (!tile) {
                        if (focusLine.isValid() && WFMath::Intersect(focusLine, tileBounds, false)) {
                            insertFront = true;
                        } else {
                            insertBack = true;
                        }
                    }
                }

                if (insertFront) {
                    if (mDirtyAwareTiles.insert(index).second) {
                        mDirtyAwareOrderedTiles.push_front(index);
                    }
                } else if (insertBack) {
                    if (mDirtyAwareTiles.insert(index).second) {
                        mDirtyAwareOrderedTiles.push_back(index);
                    }
                }

                mDirtyUnwareTiles.erase(index);

                auto existingAwareTileI = awareAreaSet.find(index);
                if (existingAwareTileI == awareAreaSet.end()) {
                    //Tile wasn't part of the existing set; increase count
                    mAwareTiles[index]++;
                } else {
                    //Tile was part of the existing set. No need to increase aware count,
                    //but remove from awareAreaSet to avoid count being decreased once we're done
                    awareAreaSet.erase(existingAwareTileI);
                }
                newAwareAreaSet.insert(index);

                mActiveTileList->insert(std::move(index));
            }
        }
    }

    //All tiles that still are in awareAreaSet are those that aren't active anymore.
    //Aware count should be decreased for each one.
    returnAwareTiles(awareAreaSet);

    //Finally copy the new aware area set into the set
    awareAreaSet = std::move(newAwareAreaSet);


    debug_print(
            "Awareness area set: " << area << ". Dirty unaware tiles: " << mDirtyUnwareTiles.size() << " Dirty aware tiles: " << mDirtyAwareTiles.size() << " Aware tile count: "
                                   << mAwareTiles.size())

    if (!wereDirtyTiles && !mDirtyAwareTiles.empty()) {
        EventTileDirty();
    }
}

void Awareness::returnAwareTiles(const std::set<std::pair<int, int>>& tileset)
{
    for (auto& tileIndex : tileset) {
        auto awareEntry = mAwareTiles.find(tileIndex);
        awareEntry->second--;
        if (awareEntry->second == 0) {
            mAwareTiles.erase(awareEntry);
            if (mDirtyAwareTiles.erase(tileIndex)) {
                mDirtyAwareOrderedTiles.remove(tileIndex);
                mDirtyUnwareTiles.insert(tileIndex);
            }
        }
    }
}


void Awareness::removeAwarenessArea(const std::string& areaId)
{
    auto I = mAwareAreas.find(areaId);
    if (I == mAwareAreas.end()) {
        return;
    }

    returnAwareTiles(I->second);
}


size_t Awareness::unawareTilesInArea(const std::string& areaId) const
{
    auto I = mAwareAreas.find(areaId);
    if (I == mAwareAreas.end()) {
        return 0;
    }

    size_t count = 0;
    auto& tileSet = I->second;
    for (auto& entry : tileSet) {
        if (mDirtyAwareTiles.find(entry) == tileSet.end()) {
            ++count;
        }
    }
    return count;
}


void Awareness::rebuildTile(int tx, int ty, const std::vector<WFMath::RotBox<2>>& entityAreas)
{
    TileCacheData tiles[MAX_LAYERS];
    memset(tiles, 0, sizeof(tiles));

    int ntiles = rasterizeTileLayers(entityAreas, tx, ty, tiles, MAX_LAYERS);

    for (int j = 0; j < ntiles; ++j) {
        TileCacheData* tile = &tiles[j];

        auto* header = (dtTileCacheLayerHeader*) tile->data;
        dtTileRef tileRef = mTileCache->getTileRef(mTileCache->getTileAt(header->tx, header->ty, header->tlayer));
        if (tileRef) {
            mTileCache->removeTile(tileRef, nullptr, nullptr);
        }
        dtStatus status = mTileCache->addTile(tile->data, tile->dataSize, DT_COMPRESSEDTILE_FREE_DATA, 0); // Add compressed tiles to tileCache
        if (dtStatusFailed(status)) {
            log(WARNING, String::compose("Failed to add tile in awareness. x: %1 y: %2 Reason: %3", tx, ty, status & DT_STATUS_DETAIL_MASK));
            dtFree(tile->data);
            tile->data = 0;
            continue;
        }
    }

    {
        rmt_ScopedCPUSample(buildNavMeshTile, 0)
        dtStatus status = mTileCache->buildNavMeshTilesAt(tx, ty, mNavMesh);
        if (dtStatusFailed(status)) {
            log(WARNING, String::compose("Failed to build nav mesh tile in awareness. x: %1 y: %2 Reason: %3", tx, ty, status & DT_STATUS_DETAIL_MASK));
        }
    }

    EventTileUpdated(tx, ty);

}

WFMath::RotBox<2> Awareness::buildEntityAreas(const EntityEntry& entity)
{

    //The entity is solid (i.e. can be collided with) if it has a bbox and the "solid" property isn't set to false (or 0 as it's an int).
    if (entity.isSolid) {
        //we now have to get the location of the entity in world space
        auto& pos = entity.pos.data;
        auto& orientation = entity.orientation.data;
        auto& bbox = entity.bbox.data;

        //If it's below walkable height just skip it.
        if (bbox.highCorner().y() - bbox.lowCorner().y() < mStepHeight) {
            return {};
        }

        if (pos.isValid()) {
            WFMath::RotMatrix<2> rm;
            if (orientation.isValid()) {
                WFMath::Vector<3> xVec = WFMath::Vector<3>(1.0, 0.0, 0.0).rotate(orientation);
                auto theta = std::atan2(xVec.z(), xVec.x()); // rotation about Y

                rm.rotation(theta);
            }


            WFMath::Point<2> highCorner(bbox.highCorner().x(), bbox.highCorner().z());
            WFMath::Point<2> lowCorner(bbox.lowCorner().x(), bbox.lowCorner().z());

            //Expand the box a little so that we can navigate around it without being stuck on it.
            //We'll use the radius of the avatar.
            highCorner += WFMath::Vector<2>(mAgentRadius, mAgentRadius);
            lowCorner -= WFMath::Vector<2>(mAgentRadius, mAgentRadius);

            WFMath::RotBox<2> rotbox(WFMath::Point<2>::ZERO(), highCorner - lowCorner, WFMath::RotMatrix<2>().identity());
            rotbox.shift(WFMath::Vector<2>(lowCorner.x(), lowCorner.y()));
            if (rm.isValid()) {
                rotbox.rotatePoint(rm, WFMath::Point<2>::ZERO());
            }

            rotbox.shift(WFMath::Vector<2>(pos.x(), pos.z()));

            return rotbox;
        }
    }
    return {};
}

void Awareness::findEntityAreas(const WFMath::AxisBox<2>& extent, std::vector<WFMath::RotBox<2> >& areas)
{
    for (auto& entry : mEntityAreas) {
        auto& rotbox = entry.second;
        if (WFMath::Contains(extent, rotbox, false) || WFMath::Intersect(extent, rotbox, false)) {
            areas.push_back(rotbox);
        }
    }
}

int Awareness::rasterizeTileLayers(const std::vector<WFMath::RotBox<2>>& entityAreas, int tx, int ty, TileCacheData* tiles, int maxTiles)
{
    rmt_ScopedCPUSample(rasterizeTileLayers, 0)
    std::vector<float> vertsVector;
    std::vector<int> trisVector;

    FastLZCompressor comp;
    RasterizationContext rc;

// Tile bounds.
    const float tcs = mCfg.tileSize * mCfg.cs;

    rcConfig tcfg = mCfg;

    tcfg.bmin[0] = mCfg.bmin[0] + tx * tcs;
    tcfg.bmin[1] = mCfg.bmin[1];
    tcfg.bmin[2] = mCfg.bmin[2] + ty * tcs;
    tcfg.bmax[0] = mCfg.bmin[0] + (tx + 1) * tcs;
    tcfg.bmax[1] = mCfg.bmax[1];
    tcfg.bmax[2] = mCfg.bmin[2] + (ty + 1) * tcs;
    tcfg.bmin[0] -= tcfg.borderSize * tcfg.cs;
    tcfg.bmin[2] -= tcfg.borderSize * tcfg.cs;
    tcfg.bmax[0] += tcfg.borderSize * tcfg.cs;
    tcfg.bmax[2] += tcfg.borderSize * tcfg.cs;

//First define all vertices. Get one extra vertex in each direction so that there's no cutoff at the tile's edges.
    int heightsXMin = static_cast<int>(std::floor(tcfg.bmin[0]) - 1);
    int heightsXMax = static_cast<int>(std::ceil(tcfg.bmax[0]) + 1);
    int heightsYMin = static_cast<int>(std::floor(tcfg.bmin[2]) - 1);
    int heightsYMax = static_cast<int>(std::ceil(tcfg.bmax[2]) + 1);
    int sizeX = heightsXMax - heightsXMin;
    int sizeY = heightsYMax - heightsYMin;

    //Blit height values with 1 meter interval
    std::vector<float> heights(sizeX * sizeY);
    {
        rmt_ScopedCPUSample(blitHeights, 0)
        mHeightProvider.blitHeights(heightsXMin, heightsXMax, heightsYMin, heightsYMax, heights);
        float* heightData = heights.data();
        for (int y = heightsYMin; y < heightsYMax; ++y) {
            for (int x = heightsXMin; x < heightsXMax; ++x) {
                vertsVector.push_back(x);
                vertsVector.push_back(*heightData);
                vertsVector.push_back(y);
                heightData++;
            }
        }
    }

    //Then define the triangles
    for (int y = 0; y < (sizeY - 1); y++) {
        for (int x = 0; x < (sizeX - 1); x++) {
            int vertPtr = (y * sizeX) + x;
            //make a square, including the vertices to the right and below
            trisVector.push_back(vertPtr);
            trisVector.push_back(vertPtr + sizeX);
            trisVector.push_back(vertPtr + 1);

            trisVector.push_back(vertPtr + 1);
            trisVector.push_back(vertPtr + sizeX);
            trisVector.push_back(vertPtr + 1 + sizeX);
        }
    }

    float* verts = vertsVector.data();
    int* tris = trisVector.data();
    const int nverts = vertsVector.size() / 3;
    const int ntris = trisVector.size() / 3;

// Allocate voxel heightfield where we rasterize our input data to.
    rc.solid = rcAllocHeightfield();
    if (!rc.solid) {
        mCtx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'solid'.");
        return 0;
    }
    {
        rmt_ScopedCPUSample(rcCreateHeightfield, 0)
        if (!rcCreateHeightfield(mCtx.get(), *rc.solid, tcfg.width, tcfg.height, tcfg.bmin, tcfg.bmax, tcfg.cs, tcfg.ch)) {
            mCtx->log(RC_LOG_ERROR, "buildNavigation: Could not create solid heightfield.");
            return 0;
        }
    }

// Allocate array that can hold triangle flags.
    rc.triareas = new unsigned char[ntris];
    if (!rc.triareas) {
        mCtx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'm_triareas' (%d).", ntris / 3);
        return 0;
    }

    memset(rc.triareas, 0, ntris * sizeof(unsigned char));
    {
        rmt_ScopedCPUSample(rcMarkWalkableTriangles, 0)
        rcMarkWalkableTriangles(mCtx.get(), tcfg.walkableSlopeAngle, verts, nverts, tris, ntris, rc.triareas);
    }
    {
        rmt_ScopedCPUSample(rcRasterizeTriangles, 0)
        rcRasterizeTriangles(mCtx.get(), verts, nverts, tris, rc.triareas, ntris, *rc.solid, tcfg.walkableClimb);
    }
// Once all geometry is rasterized, we do initial pass of filtering to
// remove unwanted overhangs caused by the conservative rasterization
// as well as filter spans where the character cannot possibly stand.

//NOTE: These are disabled for now since we currently only handle a simple 2d height map
//with bounding boxes snapped to the ground. If this changes these calls probably needs to be activated.
//	rcFilterLowHangingWalkableObstacles(m_ctx, tcfg.walkableClimb, *rc.solid);
//	rcFilterLedgeSpans(m_ctx, tcfg.walkableHeight, tcfg.walkableClimb, *rc.solid);
//	rcFilterWalkableLowHeightSpans(m_ctx, tcfg.walkableHeight, *rc.solid);

    rc.chf = rcAllocCompactHeightfield();
    if (!rc.chf) {
        mCtx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'chf'.");
        return 0;
    }
    {
        rmt_ScopedCPUSample(rcBuildCompactHeightfield, 0)
        if (!rcBuildCompactHeightfield(mCtx.get(), tcfg.walkableHeight, tcfg.walkableClimb, *rc.solid, *rc.chf)) {
            mCtx->log(RC_LOG_ERROR, "buildNavigation: Could not build compact data.");
            return 0;
        }
    }

// Erode the walkable area by agent radius.
    {
        rmt_ScopedCPUSample(rcErodeWalkableArea, 0)

        if (!rcErodeWalkableArea(mCtx.get(), tcfg.walkableRadius, *rc.chf)) {
            mCtx->log(RC_LOG_ERROR, "buildNavigation: Could not erode.");
            return 0;
        }
    }

    {
        rmt_ScopedCPUSample(markAreas, 0)
        // Mark areas.
        for (auto& rotbox : entityAreas) {
            float areaVerts[3 * 4];

            areaVerts[0] = rotbox.getCorner(1).x();
            areaVerts[1] = 0;
            areaVerts[2] = rotbox.getCorner(1).y();

            areaVerts[3] = rotbox.getCorner(3).x();
            areaVerts[4] = 0;
            areaVerts[5] = rotbox.getCorner(3).y();

            areaVerts[6] = rotbox.getCorner(2).x();
            areaVerts[7] = 0;
            areaVerts[8] = rotbox.getCorner(2).y();

            areaVerts[9] = rotbox.getCorner(0).x();
            areaVerts[10] = 0;
            areaVerts[11] = rotbox.getCorner(0).y();

            rcMarkConvexPolyArea(mCtx.get(), areaVerts, 4, tcfg.bmin[1], tcfg.bmax[1], DT_TILECACHE_NULL_AREA, *rc.chf);
        }
    }
    rc.lset = rcAllocHeightfieldLayerSet();
    if (!rc.lset) {
        mCtx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'lset'.");
        return 0;
    }
    {
        rmt_ScopedCPUSample(rcBuildHeightfieldLayers, 0)
        if (!rcBuildHeightfieldLayers(mCtx.get(), *rc.chf, tcfg.borderSize, tcfg.walkableHeight, *rc.lset)) {
            mCtx->log(RC_LOG_ERROR, "buildNavigation: Could not build heighfield layers.");
            return 0;
        }
    }
    rc.ntiles = 0;
    for (int i = 0; i < rcMin(rc.lset->nlayers, MAX_LAYERS); ++i) {
        rmt_ScopedCPUSample(buildTileCache, 0)
        TileCacheData* tile = &rc.tiles[rc.ntiles++];
        const rcHeightfieldLayer* layer = &rc.lset->layers[i];

        // Store header
        dtTileCacheLayerHeader header{};
        header.magic = DT_TILECACHE_MAGIC;
        header.version = DT_TILECACHE_VERSION;

        // Tile layer location in the navmesh.
        header.tx = tx;
        header.ty = ty;
        header.tlayer = i;
        dtVcopy(header.bmin, layer->bmin);
        dtVcopy(header.bmax, layer->bmax);

        // Tile info.
        header.width = (unsigned char) layer->width;
        header.height = (unsigned char) layer->height;
        header.minx = (unsigned char) layer->minx;
        header.maxx = (unsigned char) layer->maxx;
        header.miny = (unsigned char) layer->miny;
        header.maxy = (unsigned char) layer->maxy;
        header.hmin = (unsigned short) layer->hmin;
        header.hmax = (unsigned short) layer->hmax;

        dtStatus status = dtBuildTileCacheLayer(&comp, &header, layer->heights, layer->areas, layer->cons, &tile->data, &tile->dataSize);
        if (dtStatusFailed(status)) {
            return 0;
        }
    }

// Transfer ownership of tile data from build context to the caller.
    int n = 0;
    for (int i = 0; i < rcMin(rc.ntiles, maxTiles); ++i) {
        tiles[n++] = rc.tiles[i];
        rc.tiles[i].data = nullptr;
        rc.tiles[i].dataSize = 0;
    }

    return n;
}

void Awareness::processTiles(const WFMath::AxisBox<2>& area,
                             const std::function<void(unsigned int, dtTileCachePolyMesh&, float* origin, float cellsize, float cellheight, dtTileCacheLayer& layer)>& processor) const
{
    float bmin[]{static_cast<float>(area.lowCorner().x()), -100, static_cast<float>(area.lowCorner().y())};
    float bmax[]{static_cast<float>(area.highCorner().x()), 100, static_cast<float>(area.highCorner().y())};

    dtCompressedTileRef tilesRefs[256];
    int ntiles;
    dtStatus status = mTileCache->queryTiles(bmin, bmax, tilesRefs, &ntiles, 256);
    if (status == DT_SUCCESS) {
        std::vector<const dtCompressedTile*> tiles(static_cast<unsigned long>(ntiles));
        for (int i = 0; i < ntiles; ++i) {
            tiles[i] = mTileCache->getTileByRef(tilesRefs[i]);
        }
        processTiles(tiles, processor);
    }
}

void Awareness::processTile(int tx, int ty,
                            const std::function<void(unsigned int, dtTileCachePolyMesh&, float* origin, float cellsize, float cellheight, dtTileCacheLayer& layer)>& processor) const
{
    dtCompressedTileRef tilesRefs[MAX_LAYERS];
    const int ntiles = mTileCache->getTilesAt(tx, ty, tilesRefs, MAX_LAYERS);

    std::vector<const dtCompressedTile*> tiles(static_cast<unsigned long>(ntiles));
    for (int i = 0; i < ntiles; ++i) {
        tiles[i] = mTileCache->getTileByRef(tilesRefs[i]);
    }

    processTiles(tiles, processor);
}

void Awareness::processAllTiles(
        const std::function<void(unsigned int, dtTileCachePolyMesh&, float* origin, float cellsize, float cellheight, dtTileCacheLayer& layer)>& processor) const
{
    int ntiles = mTileCache->getTileCount();
    std::vector<const dtCompressedTile*> tiles(static_cast<unsigned long>(ntiles));
    for (int i = 0; i < ntiles; ++i) {
        tiles[i] = mTileCache->getTile(i);
    }

    processTiles(tiles, processor);

}

void Awareness::processTiles(std::vector<const dtCompressedTile*> tiles,
                             const std::function<void(unsigned int, dtTileCachePolyMesh&, float* origin, float cellsize, float cellheight, dtTileCacheLayer& layer)>& processor) const
{
    rmt_ScopedCPUSample(processTiles, 0)
    struct TileCacheBuildContext
    {
        inline explicit TileCacheBuildContext(struct dtTileCacheAlloc* a) :
                layer(nullptr), lcset(nullptr), lmesh(nullptr), alloc(a)
        {
        }

        inline ~TileCacheBuildContext()
        {
            purge();
        }

        void purge()
        {
            dtFreeTileCacheLayer(alloc, layer);
            layer = nullptr;
            dtFreeTileCacheContourSet(alloc, lcset);
            lcset = nullptr;
            dtFreeTileCachePolyMesh(alloc, lmesh);
            lmesh = nullptr;
        }

        struct dtTileCacheLayer* layer;
        struct dtTileCacheContourSet* lcset;
        struct dtTileCachePolyMesh* lmesh;
        struct dtTileCacheAlloc* alloc;
    };

    dtTileCacheAlloc* talloc = mTileCache->getAlloc();
    dtTileCacheCompressor* tcomp = mTileCache->getCompressor();
    const dtTileCacheParams* params = mTileCache->getParams();

    for (const dtCompressedTile* tile : tiles) {

        talloc->reset();

        TileCacheBuildContext bc(talloc);
        const int walkableClimbVx = (int) (params->walkableClimb / params->ch);
        dtStatus status;

        // Decompress tile layer data.
        status = dtDecompressTileCacheLayer(talloc, tcomp, tile->data, tile->dataSize, &bc.layer);
        if (dtStatusFailed(status)) {
            return;
        }

        // Build navmesh
        status = dtBuildTileCacheRegions(talloc, *bc.layer, walkableClimbVx);
        if (dtStatusFailed(status)) {
            return;
        }

        bc.lcset = dtAllocTileCacheContourSet(talloc);
        if (!bc.lcset) {
            return;
        }
        status = dtBuildTileCacheContours(talloc, *bc.layer, walkableClimbVx, params->maxSimplificationError, *bc.lcset);
        if (dtStatusFailed(status)) {
            return;
        }

        bc.lmesh = dtAllocTileCachePolyMesh(talloc);
        if (!bc.lmesh) {
            return;
        }
        status = dtBuildTileCachePolyMesh(talloc, *bc.lcset, *bc.lmesh);
        if (dtStatusFailed(status)) {
            return;
        }

        processor(mTileCache->getTileRef(tile), *bc.lmesh, tile->header->bmin, params->cs, params->ch, *bc.layer);

    }
}


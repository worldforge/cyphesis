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
 */

#ifndef AWARENESS_H_
#define AWARENESS_H_

#include "RecastDetour/Recast/Include/Recast.h"

#include "rules/LocatedEntity.h"
#include "rules/Location.h"
#include "rules/ai/MemEntity.h"

#include <wfmath/axisbox.h>
#include <wfmath/rotbox.h>
#include <wfmath/point.h>

#include <sigc++/signal.h>
#include <sigc++/trackable.h>
#include <sigc++/connection.h>

#include <list>
#include <vector>
#include <set>
#include <map>
#include <unordered_map>
#include <functional>


class LocatedEntity;

class Location;

class dtNavMeshQuery;

class dtNavMesh;

class dtTileCache;

class dtTileCachePolyMesh;

class dtTileCacheLayer;

class dtCompressedTile;

class dtQueryFilter;

class dtObstacleAvoidanceQuery;

struct dtObstacleAvoidanceParams;

struct IHeightProvider;

template<typename T>
class MRUList;

struct TileCacheData;
struct InputGeometry;

enum PolyAreas
{
    POLYAREA_GROUND, POLYAREA_WATER, POLYAREA_ROAD, POLYAREA_DOOR, POLYAREA_GRASS, POLYAREA_JUMP,
};
enum PolyFlags
{
    POLYFLAGS_WALK = 0x01,      // Ability to walk (ground, grass, road)
    POLYFLAGS_SWIM = 0x02,      // Ability to swim (water).
    POLYFLAGS_DOOR = 0x04,      // Ability to move through doors.
    POLYFLAGS_JUMP = 0x08,      // Ability to jump.
    POLYFLAGS_DISABLED = 0x10,        // Disabled polygon
    POLYFLAGS_ALL = 0xffff      // All abilities.
};


/**
 * @brief Keeps track of the connections and state of a specific entity.
 *
 * We don't need to observe all entities in the same way; some are ignored, and some are moving.
 * Instances of this keep track of this information.
 */
struct EntityConnections
{
    bool isMoving;
    bool isIgnored;
};

template<typename T>
struct TimestampedProperty
{
    T data;
    double timestamp = -1.0;
};

struct EntityEntry
{
    long entityId;
    int numberOfObservers;

    TimestampedProperty<WFMath::Point<3>> pos;
    TimestampedProperty<WFMath::Vector<3>> velocity;

    TimestampedProperty<WFMath::Quaternion> orientation;
    TimestampedProperty<WFMath::Vector<3>> angular;

    TimestampedProperty<WFMath::AxisBox<3>> bbox;
    TimestampedProperty<WFMath::Vector<3>> scale;
    WFMath::AxisBox<3> scaledBbox;

    /**
     * True if this entity is owned by an actor. These entities should not be updated by sights by other actors: only the actor itself should update it.
     */
    bool isActorOwned;

    /**
     * True if the entity is moving. Moving entities should be treated as moving actors, and should not be part of the navmesh.
     */
    bool isMoving;

    /**
     * True if this entity is ignored.
     */
    bool isIgnored;

    /**
     * True if this entity is solid.
     */
    bool isSolid;
};


/**
 * @brief Handles awareness of the Avatar's surroundings for the purpose of path finding and steering.
 *
 * The Recast and Detour libraries are used to maintain a map of the surrounding world.
 * In order for this to be effective we use a concept of "awareness area". Whenever a path needs to be found the
 * awareness area first needs to be defined. This is the area within which we'll keep up to date navigational data as entities
 * are discovered or moves. The awareness area is typically an area which will encompass the terrain between the
 * avatar and the destination, together with some padding.
 *
 * Note that this class currently is optimized for a flat world with only one layer. Furthermore all entities are only
 * handled through their bounding box. As the world progresses and support for more complex geometry is added, as well
 * as more complete physics simulation, this class needs to be expanded.
 *
 * Internally this class uses a dtTileCache to manage the tiles. Since the world is dynamic we need to manage the
 * navmeshes through tiles in order to keep the resource usage down.
 *
 * #TODO: split up the part of awareness which keeps track of entities with the same parent (i.e EntityEntry) from the path finding part
 */
class Awareness
{
    public:
        /**
         * A callback function for processing tiles.
         */
        typedef std::function<void(unsigned int, dtTileCachePolyMesh&, float* origin, float cellsize, float cellheight, dtTileCacheLayer& layer)> TileProcessor;

        /**
         * @brief Ctor.
         * @param domainEntityId The id of the entity holding the domain of the awareness.
         * @param heightProvider A height provider, used for getting terrain height data.
         * @param tileSize The size, in voxels, of one side of a tile. The larger this is the longer each tile takes to generate, but the overhead of managing tiles is decreased.
         */
        Awareness(long domainEntityId,
                  float agentRadius,
                  float agentHeight,
                  float stepHeight,
                  IHeightProvider& heightProvider,
                  const WFMath::AxisBox<3>& extent,
                  int tileSize = 64);

        virtual ~Awareness();

        /**
         * @brief Sets the area of awareness.
         *
         * An optional segment can also be supplied. Any tile which intersects the segment will get processing precedence.
         * @param area The area which makes up the awareness area.
         * @param focusLine An optional segment for tile precedence.
         */
        void setAwarenessArea(const std::string& areaId, const WFMath::RotBox<2>& area, const WFMath::Segment<2>& focusLine);

        void removeAwarenessArea(const std::string& areaId);

        size_t unawareTilesInArea(const std::string& areaId) const;

        /**
         * @brief Rebuilds a dirty tile if any such exists.
         * @return The number of dirty tiles remaining.
         */
        size_t rebuildDirtyTile();

        /**
         * @brief Finds a path from the start to the finish.
         * @param start A starting position.
         * @param end A finish position.
         * @param radius The radius of the horizontal search area (kinda; it's not a circle but an axis aligned box)
         * @param path The waypoints of the path will be stored here.
         * @return The number of waypoints in the path. 0 if no path could be found. A negative values means that something went wrong.
         */
        int findPath(const WFMath::Point<3>& start, const WFMath::Point<3>& end, float radius, std::vector<WFMath::Point<3>>& path) const;

        /**
         * @brief Process the tile at the specified index.
         * @param tx X index.
         * @param ty Y index.
         * @param processor A processing callback.
         */
        void processTile(int tx, int ty, const TileProcessor& processor) const;

        /**
         * @brief Process the tiles within the specified area.
         * @param area A world area.
         * @param processor A processing callback.
         */
        void processTiles(const WFMath::AxisBox<2>& area, const TileProcessor& processor) const;

        /**
         * @brief Process all existing tiles.
         * @param processor A processing callback.
         */
        void processAllTiles(const TileProcessor& processor) const;

        /**
         * @brief Tries to avoid near obstacles.
         * @param avatarEntityId The entity id of the avatar. This is used to filter out the avatar entity itself.
         * @param position The position of the avatar.
         * @param desiredVelocity The desired velocity.
         * @param newVelocity The calculated new velocity.
         * @param currentTimestamp The current timestamp. Used to determine positions of moving entities.
         * @return True if the velocity had to be changed in order to avoid obstacles.
         */
        bool avoidObstacles(long avatarEntityId,
                            const WFMath::Point<2>& position,
                            const WFMath::Vector<2>& desiredVelocity,
                            WFMath::Vector<2>& newVelocity,
                            double currentTimestamp,
                            const WFMath::Point<2>* nextWayPoint) const;

        /**
         * @brief Prunes a tile if possible and needed.
         *
         * This removes a tile that isn't in the current awareness area, if needed.
         */
        void pruneTiles();

        /**
         * @brief Returns true if there are tiles that needs to be removed.
         *
         * Call pruneTiles() to actually remove tiles.
         * @return True if there are tiles that needs pruning.
         */
        bool needsPruning() const;

        /**
         * @brief Sets the desired tile amount.
         * @param amount The new desired tile amount.
         */
        void setDesiredTilesAmount(size_t amount);

        /**
         * Adds a new entity to the awareness.
         * @param observer The observer, i.e. the entity which currently is observing it. Multiple observers can observe one entity at the same time.
         * @param entity The entity to observe.
         * @param isDynamic True if the entity is moving, or is expected to move. This will remove it from the navmesh, and it will instead be treated as an "obstacle" when moving.
         */
        void addEntity(const MemEntity& observer, const MemEntity& entity, bool isDynamic);

        /**
         * Removes an observation of an entity.
         * Note that this does not mean that the entity automatically is removed from the awareness, as there might be other observers.
         * First when all observers have been removed will the entity be removed from the awareness.
         * @param observer The observer entity.
         * @param entity The entity to remove.
         */
        void removeEntity(const MemEntity& observer, const MemEntity& entity);

        void updateEntity(const MemEntity& observer, const MemEntity& entity, const Atlas::Objects::Entity::RootEntity& ent);

        /**
         * @brief Emitted when a tile is updated.
         * @param int Tile x index.
         * @param int Tile y index.
         */
        sigc::signal<void(int, int)> EventTileUpdated;

        /**
         * @brief Emitted when a tile is removed.
         * @param int Tile x index.
         * @param int Tile y index.
         * @param int Tile layer.
         */
        sigc::signal<void(int, int, int)> EventTileRemoved;

        /**
         * @brief Emitted when a tile has been marked as dirty.
         *
         * Any controlling code should call rebuildDirtyTile() to rebuild the dirty tiles.
         */
        sigc::signal<void()> EventTileDirty;

        float getTileSizeInMeters() const;

        /**
         * Returns true if the tile at the specified position is marked to be aware
         * (even though it might not at the moment have been rasterized)
         * @param x
         * @param y
         * @return
         */
        bool isPositionAware(float x, float y) const;

        void addObserver();

        void removeObserver();

        /**
         * @brief Marks all tiles within the area as dirty.
         *
         * Dirty tiles will be rebuilt.
         * @param area An area.
         */
        void markTilesAsDirty(const WFMath::AxisBox<2>& area);

        /**
         * @brief Projects a entity-local position relative to the domain entity of this awareness.
         * @param entityId The id of the entity. This is either the id of the domain entity, for which the pos will be unchanged, or an id of a child entity.
         * @param pos Position, to be changed.
         * @param currentServerTimestamp The current server time stamp, to calculate new positions for moving entities.
         * @return True if entity could be found.
         */
        bool projectPosition(long entityId, WFMath::Point<3>& pos, double currentServerTimestamp) const;

        WFMath::Point<3> projectPosition(long entityId, double currentServerTimestamp) const;

        const std::unordered_map<long, std::unique_ptr<EntityEntry>>& getObservedEntities() const;

        /**
         * Checks whether there are any dirty aware tiles that needs to be rebuilt.
         * @return
         */
        bool hasDirtyAwareTiles() const
        {
            return !mDirtyAwareTiles.empty();
        }

    protected:

        IHeightProvider& mHeightProvider;


        /**
         * @brief The id entity holding the domain of the awareness.
         *
         * This is the parent of all the entities contained in this awareness.
         */
        long mDomainEntityId;

        std::unique_ptr<struct LinearAllocator> mTalloc;
        std::unique_ptr<struct FastLZCompressor> mTcomp;
        std::unique_ptr<struct MeshProcess> mTmproc;

        /**
         * @brief The radius of the agents.
         */
        float mAgentRadius;

        float mStepHeight;

        /**
         * @brief The desired number of tiles for one observer.
         *
         * As the number of observers change the value of mDesiredTilesAmount is recalculated using this
         * as base.
         */
        size_t mBaseTileAmount;

        /**
         * @brief The desired amount of tiles to keep active.
         *
         * If the number of unused tiles (i.e. not part of the current awareness) exceed this number
         * any controller should prune the tiles.
         * @see pruneTiles()
         * @see needsPruning()
         */
        size_t mDesiredTilesAmount;

        /**
         * @brief The main Recast context.
         */
        std::unique_ptr<rcContext> mCtx;

        /**
         * @brief The Recast configuration.
         */
        rcConfig mCfg;

        /**
         * @brief The main Detour tile cache.
         *
         * This keeps track of all the tiles.
         */
        dtTileCache* mTileCache;

        /**
         * @brief
         */
        dtNavMesh* mNavMesh;
        dtNavMeshQuery* mNavQuery;
        std::unique_ptr<dtQueryFilter> mFilter;
        dtObstacleAvoidanceQuery* mObstacleAvoidanceQuery;
        std::unique_ptr<dtObstacleAvoidanceParams> mObstacleAvoidanceParams;

        /**
         * @brief A map of all of the tiles that currently are inside our awareness area.
         * The value corresponds to the number of observers for the specific tile.
         */
        std::map<std::pair<int, int>, unsigned int> mAwareTiles;

        /**
         * @brief A set of tiles that are dirty, but aren't in our current awareness area.
         *
         * When the awareness area is changed this will be used to check if any existing tile needs to be rebuilt.
         */
        std::set<std::pair<int, int>> mDirtyUnwareTiles;

        /**
         * @brief A set of tiles that are dirty and are in our current awareness area.
         *
         * These needs to be rebuilt as soon as possible.
         * @note The contents of the set is mirrored in mDirtyAwareOrderedTiles.
         */
        std::set<std::pair<int, int>> mDirtyAwareTiles;

        /**
         * @brief An ordered list of tiles that are dirty and are in our current awareness area.
         *
         * When rebuilding tiles we'll use the ordered list instead of the mDirtyAwareTiles set.
         * The reason is that we want to have some control of the order of tile buildings, so that those
         * tiles that are in a straight line between the entity and the destination are built first.
         * @note The contents of the set is mirrored in mDirtyAwareTiles.
         */
        std::list<std::pair<int, int>> mDirtyAwareOrderedTiles;

        /**
         * @brief The view resolved areas for each entity.
         *
         * This information is used when determining what tiles to rebuild when entities are moved.
         */
        std::map<const EntityEntry*, WFMath::RotBox<2>> mEntityAreas;

        /**
         * @brief Keeps track of all currently observed entities.
         */
        std::unordered_map<long, std::unique_ptr<EntityEntry>> mObservedEntities;

        /**
         * @brief Keeps track of all entities that are moving.
         *
         * Moving entities aren't included in the navmesh generation and updates, but are instead
         * considered when doing obstacle avoidance.
         * It's expected that moving entities should be rather small and have a uniform shape, since they
         * internally are represented as 2d circles.
         */
        std::set<const EntityEntry*> mMovingEntities;

        /**
         * @brief Keeps track of current awareness areas.
         *
         * Each awareness area is identified by an id. This keeps track of the tiles that belong to each area.
         */
        std::unordered_map<std::string, std::set<std::pair<int, int>>> mAwareAreas;

        /**
         * @brief A Most Recently Used list of active tiles.
         *
         * Whenever a tile is added to the awareness area it has it's priority increased within this list.
         * This makes sure that those tiles that are at the back of the list always are the least used ones.
         */
        std::unique_ptr<MRUList<std::pair<int, int>>> mActiveTileList;

        /**
         * @brief The number of active observers.
         *
         * This affects the desired tile count.
         */
        size_t mObserverCount;

        /**
         * Processes updates to an entity, and reacts if the position changed.
         * @param entry
         * @param entity
         * @param ent
         * @param timestamp
         * @return True if any position or size changed.
         */
        bool processEntityUpdate(EntityEntry& entry, const MemEntity& entity, const Atlas::Objects::Entity::RootEntity& ent, double timestamp);

        /**
         * @brief Rebuild the tile at the specific index.
         * @param tx X index.
         * @param ty Y index.
         * @param entityAreas A list of entities, projected as 2d rotation boxes, which affects the tile.
         */
        void rebuildTile(int tx, int ty, const std::vector<WFMath::RotBox<2>>& entityAreas);

        /**
         * @brief Calculates the 2d rotbox area of the entity and adds it to the supplied map of areas.
         * @param entity An entity.
         */
        WFMath::RotBox<2> buildEntityAreas(const EntityEntry& entity);

        /**
         * Find entity 2d rotbox areas within the supplied extent.
         * @param extent An extent in world units.
         * @param areas A vector of areas.
         */
        void findEntityAreas(const WFMath::AxisBox<2>& extent, std::vector<WFMath::RotBox<2> >& areas);

        /**
         * @brief Rasterizes the tile at the specified index.
         * @param entityAreas The entity areas that affects the tile.
         * @param tx X index.
         * @param ty Y index.
         * @param tiles Out parameter for the tiles.
         * @param maxTiles The maximum number of tile layers to create.
         * @return The number of tile layers that were created.
         */
        int rasterizeTileLayers(const std::vector<WFMath::RotBox<2>>& entityAreas, int tx, int ty, TileCacheData* tiles, int maxTiles);

        /**
         * @brief Applies the supplied processor on the supplied tiles.
         * @param tiles A collection of tile references.
         * @param processor A processor function.
         */
        void processTiles(std::vector<const dtCompressedTile*> tiles,
                          const std::function<void(unsigned int, dtTileCachePolyMesh&, float* origin, float cellsize, float cellheight, dtTileCacheLayer& layer)>& processor) const;

        /**
         * @brief Marks all tiles within an indexed area as dirty.
         *
         * Dirty tiles will be rebuilt.
         *
         * @param tileMinXIndex Min X index.
         * @param tileMaxXIndex Max X index.
         * @param tileMinYIndex Min Y index.
         * @param tileMaxYIndex Max Y index.
         */
        void markTilesAsDirty(int tileMinXIndex, int tileMaxXIndex, int tileMinYIndex, int tileMaxYIndex);

        /**
         * @brief Find the tiles affected by the supplied area.
         * @param area An area in world units.
         * @param tileMinXIndex Min X index.
         * @param tileMaxXIndex Max X index.
         * @param tileMinZIndex Min Y index.
         * @param tileMaxZIndex Max Y index.
         */
        void findAffectedTiles(const WFMath::AxisBox<2>& area, int& tileMinXIndex, int& tileMaxXIndex, int& tileMinZIndex, int& tileMaxZIndex) const;


        void returnAwareTiles(const std::set<std::pair<int, int>>& tileset);

};


#endif /* AWARENESS_H_ */

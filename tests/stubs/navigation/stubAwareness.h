// AUTOGENERATED file, created by the tool generate_stub.py, don't edit!
// If you want to add your own functionality, instead edit the stubAwareness_custom.h file.

#ifndef STUB_NAVIGATION_AWARENESS_H
#define STUB_NAVIGATION_AWARENESS_H

#include "navigation/Awareness.h"
#include "stubAwareness_custom.h"



#ifndef STUB_Awareness_Awareness
//#define STUB_Awareness_Awareness
   Awareness::Awareness(const LocatedEntity& domainEntity, float agentRadius, float agentHeight, float stepHeight, IHeightProvider& heightProvider, const WFMath::AxisBox<3>& extent, int tileSize )
    : mTileCache(nullptr),mNavMesh(nullptr),mNavQuery(nullptr),mObstacleAvoidanceQuery(nullptr)
  {
    
  }
#endif //STUB_Awareness_Awareness

#ifndef STUB_Awareness_Awareness_DTOR
//#define STUB_Awareness_Awareness_DTOR
   Awareness::~Awareness()
  {
    
  }
#endif //STUB_Awareness_Awareness_DTOR

#ifndef STUB_Awareness_setAwarenessArea
//#define STUB_Awareness_setAwarenessArea
  void Awareness::setAwarenessArea(const std::string& areaId, const WFMath::RotBox<2>& area, const WFMath::Segment<2>& focusLine)
  {
    
  }
#endif //STUB_Awareness_setAwarenessArea

#ifndef STUB_Awareness_removeAwarenessArea
//#define STUB_Awareness_removeAwarenessArea
  void Awareness::removeAwarenessArea(const std::string& areaId)
  {
    
  }
#endif //STUB_Awareness_removeAwarenessArea

#ifndef STUB_Awareness_unawareTilesInArea
//#define STUB_Awareness_unawareTilesInArea
  size_t Awareness::unawareTilesInArea(const std::string& areaId) const
  {
    return 0;
  }
#endif //STUB_Awareness_unawareTilesInArea

#ifndef STUB_Awareness_rebuildDirtyTile
//#define STUB_Awareness_rebuildDirtyTile
  size_t Awareness::rebuildDirtyTile()
  {
    return 0;
  }
#endif //STUB_Awareness_rebuildDirtyTile

#ifndef STUB_Awareness_findPath
//#define STUB_Awareness_findPath
  int Awareness::findPath(const WFMath::Point<3>& start, const WFMath::Point<3>& end, float radius, std::vector<WFMath::Point<3>>& path) const
  {
    return 0;
  }
#endif //STUB_Awareness_findPath

#ifndef STUB_Awareness_processTile
//#define STUB_Awareness_processTile
  void Awareness::processTile(int tx, int ty, const TileProcessor& processor) const
  {
    
  }
#endif //STUB_Awareness_processTile

#ifndef STUB_Awareness_processTiles
//#define STUB_Awareness_processTiles
  void Awareness::processTiles(const WFMath::AxisBox<2>& area, const TileProcessor& processor) const
  {
    
  }
#endif //STUB_Awareness_processTiles

#ifndef STUB_Awareness_processAllTiles
//#define STUB_Awareness_processAllTiles
  void Awareness::processAllTiles(const TileProcessor& processor) const
  {
    
  }
#endif //STUB_Awareness_processAllTiles

#ifndef STUB_Awareness_avoidObstacles
//#define STUB_Awareness_avoidObstacles
  bool Awareness::avoidObstacles(long avatarEntityId, const WFMath::Point<2>& position, const WFMath::Vector<2>& desiredVelocity, WFMath::Vector<2>& newVelocity, double currentTimestamp, const WFMath::Point<2>* nextWayPoint) const
  {
    return false;
  }
#endif //STUB_Awareness_avoidObstacles

#ifndef STUB_Awareness_pruneTiles
//#define STUB_Awareness_pruneTiles
  void Awareness::pruneTiles()
  {
    
  }
#endif //STUB_Awareness_pruneTiles

#ifndef STUB_Awareness_needsPruning
//#define STUB_Awareness_needsPruning
  bool Awareness::needsPruning() const
  {
    return false;
  }
#endif //STUB_Awareness_needsPruning

#ifndef STUB_Awareness_setDesiredTilesAmount
//#define STUB_Awareness_setDesiredTilesAmount
  void Awareness::setDesiredTilesAmount(size_t amount)
  {
    
  }
#endif //STUB_Awareness_setDesiredTilesAmount

#ifndef STUB_Awareness_addEntity
//#define STUB_Awareness_addEntity
  void Awareness::addEntity(const MemEntity& observer, const MemEntity& entity, bool isDynamic)
  {
    
  }
#endif //STUB_Awareness_addEntity

#ifndef STUB_Awareness_removeEntity
//#define STUB_Awareness_removeEntity
  void Awareness::removeEntity(const MemEntity& observer, const MemEntity& entity)
  {
    
  }
#endif //STUB_Awareness_removeEntity

#ifndef STUB_Awareness_updateEntityMovement
//#define STUB_Awareness_updateEntityMovement
  void Awareness::updateEntityMovement(const MemEntity& observer, const MemEntity& entity)
  {
    
  }
#endif //STUB_Awareness_updateEntityMovement

#ifndef STUB_Awareness_getTileSizeInMeters
//#define STUB_Awareness_getTileSizeInMeters
  float Awareness::getTileSizeInMeters() const
  {
    return 0;
  }
#endif //STUB_Awareness_getTileSizeInMeters

#ifndef STUB_Awareness_isPositionAware
//#define STUB_Awareness_isPositionAware
  bool Awareness::isPositionAware(float x, float y) const
  {
    return false;
  }
#endif //STUB_Awareness_isPositionAware

#ifndef STUB_Awareness_addObserver
//#define STUB_Awareness_addObserver
  void Awareness::addObserver()
  {
    
  }
#endif //STUB_Awareness_addObserver

#ifndef STUB_Awareness_removeObserver
//#define STUB_Awareness_removeObserver
  void Awareness::removeObserver()
  {
    
  }
#endif //STUB_Awareness_removeObserver

#ifndef STUB_Awareness_markTilesAsDirty
//#define STUB_Awareness_markTilesAsDirty
  void Awareness::markTilesAsDirty(const WFMath::AxisBox<2>& area)
  {
    
  }
#endif //STUB_Awareness_markTilesAsDirty

#ifndef STUB_Awareness_projectPosition
//#define STUB_Awareness_projectPosition
  bool Awareness::projectPosition(long entityId, WFMath::Point<3>& pos, double currentServerTimestamp) const
  {
    return false;
  }
#endif //STUB_Awareness_projectPosition

#ifndef STUB_Awareness_processEntityMovementChange
//#define STUB_Awareness_processEntityMovementChange
  void Awareness::processEntityMovementChange(EntityEntry& entry, const MemEntity& entity)
  {
    
  }
#endif //STUB_Awareness_processEntityMovementChange

#ifndef STUB_Awareness_rebuildTile
//#define STUB_Awareness_rebuildTile
  void Awareness::rebuildTile(int tx, int ty, const std::vector<WFMath::RotBox<2>>& entityAreas)
  {
    
  }
#endif //STUB_Awareness_rebuildTile

#ifndef STUB_Awareness_buildEntityAreas
//#define STUB_Awareness_buildEntityAreas
  WFMath::RotBox<2> Awareness::buildEntityAreas(const EntityEntry& entity)
  {
    return *static_cast<WFMath::RotBox<2>*>(nullptr);
  }
#endif //STUB_Awareness_buildEntityAreas

#ifndef STUB_Awareness_findEntityAreas
//#define STUB_Awareness_findEntityAreas
  void Awareness::findEntityAreas(const WFMath::AxisBox<2>& extent, std::vector<WFMath::RotBox<2> >& areas)
  {
    
  }
#endif //STUB_Awareness_findEntityAreas

#ifndef STUB_Awareness_rasterizeTileLayers
//#define STUB_Awareness_rasterizeTileLayers
  int Awareness::rasterizeTileLayers(const std::vector<WFMath::RotBox<2>>& entityAreas, int tx, int ty, TileCacheData* tiles, int maxTiles)
  {
    return 0;
  }
#endif //STUB_Awareness_rasterizeTileLayers

#ifndef STUB_Awareness_processTiles
//#define STUB_Awareness_processTiles
  void Awareness::processTiles(std::vector<const dtCompressedTile*> tiles, const std::function<void(unsigned int, dtTileCachePolyMesh&, float* origin, float cellsize, float cellheight, dtTileCacheLayer& layer)>& processor) const
  {
    
  }
#endif //STUB_Awareness_processTiles

#ifndef STUB_Awareness_markTilesAsDirty
//#define STUB_Awareness_markTilesAsDirty
  void Awareness::markTilesAsDirty(int tileMinXIndex, int tileMaxXIndex, int tileMinYIndex, int tileMaxYIndex)
  {
    
  }
#endif //STUB_Awareness_markTilesAsDirty

#ifndef STUB_Awareness_findAffectedTiles
//#define STUB_Awareness_findAffectedTiles
  void Awareness::findAffectedTiles(const WFMath::AxisBox<2>& area, int& tileMinXIndex, int& tileMaxXIndex, int& tileMinZIndex, int& tileMaxZIndex) const
  {
    
  }
#endif //STUB_Awareness_findAffectedTiles

#ifndef STUB_Awareness_returnAwareTiles
//#define STUB_Awareness_returnAwareTiles
  void Awareness::returnAwareTiles(const std::set<std::pair<int, int>>& tileset)
  {
    
  }
#endif //STUB_Awareness_returnAwareTiles


#endif
#include "navigation/Awareness.h"

Awareness::Awareness(const LocatedEntity& domainEntity, float agentRadius, float agentHeight, IHeightProvider& heightProvider, const WFMath::AxisBox<3>& extent, int tileSize):
mHeightProvider(heightProvider), mDomainEntity(domainEntity)
{
}

Awareness::~Awareness()
{
}

void Awareness::addObserver() {
}

void Awareness::removeObserver() {
}

void Awareness::addEntity(const MemEntity& observer, const LocatedEntity& entity, bool isDynamic)
{
}

void Awareness::removeEntity(const MemEntity& observer, const LocatedEntity& entity)
{
}

void Awareness::updateEntityMovement(const MemEntity& observer, const LocatedEntity& entity)
{
}

void Awareness::processEntityMovementChange(EntityEntry& entityEntry, const LocatedEntity& entity)
{
}

bool Awareness::avoidObstacles(long avatarEntityId, const WFMath::Point<2>& position, const WFMath::Vector<2>& desiredVelocity, WFMath::Vector<2>& newVelocity, double currentTimestamp) const
{
    return false;
}

void Awareness::markTilesAsDirty(const WFMath::AxisBox<2>& area)
{
}

void Awareness::markTilesAsDirty(int tileMinXIndex, int tileMaxXIndex, int tileMinYIndex, int tileMaxYIndex)
{
}

size_t Awareness::rebuildDirtyTile()
{
    return 0;
}

void Awareness::pruneTiles()
{
}

bool Awareness::needsPruning() const
{
    return false;
}

void Awareness::setDesiredTilesAmount(size_t amount)
{
}

float Awareness::getTileSizeInMeters() const
{
    return 0;
}

bool Awareness::isPositionAware(float x, float y) const
{
    return false;
}


void Awareness::findAffectedTiles(const WFMath::AxisBox<2>& area, int& tileMinXIndex, int& tileMaxXIndex, int& tileMinYIndex, int& tileMaxYIndex) const
{
}

int Awareness::findPath(const WFMath::Point<3>& start, const WFMath::Point<3>& end, float radius, std::list<WFMath::Point<3>>& path) const
{
    return 0;
}

bool Awareness::projectPosition(int entityId, WFMath::Point<3>& pos, double currentServerTimestamp)
{
    return false;
}


void Awareness::setAwarenessArea(const std::string& areaId, const WFMath::RotBox<2>& area, const WFMath::Segment<2>& focusLine)
{
}

void Awareness::returnAwareTiles(const std::set<std::pair<int,int>>& tileset)
{
}


void Awareness::removeAwarenessArea(const std::string& areaId)
{
}


size_t Awareness::unawareTilesInArea(const std::string& areaId) const
{
    return 0;
}


void Awareness::rebuildTile(int tx, int ty, const std::vector<WFMath::RotBox<2>>& entityAreas)
{
}

void Awareness::buildEntityAreas(const EntityEntry& entity, std::map<const EntityEntry*, WFMath::RotBox<2>>& entityAreas)
{
}

void Awareness::findEntityAreas(const WFMath::AxisBox<2>& extent, std::vector<WFMath::RotBox<2> >& areas)
{
}

int Awareness::rasterizeTileLayers(const std::vector<WFMath::RotBox<2>>& entityAreas, const int tx, const int ty, TileCacheData* tiles, const int maxTiles)
{
    return 0;
}

void Awareness::processTiles(const WFMath::AxisBox<2>& area,
        const std::function<void(unsigned int, dtTileCachePolyMesh&, float* origin, float cellsize, float cellheight, dtTileCacheLayer& layer)>& processor) const
{
}

void Awareness::processTile(const int tx, const int ty,
        const std::function<void(unsigned int, dtTileCachePolyMesh&, float* origin, float cellsize, float cellheight, dtTileCacheLayer& layer)>& processor) const
{
}

void Awareness::processAllTiles(
        const std::function<void(unsigned int, dtTileCachePolyMesh&, float* origin, float cellsize, float cellheight, dtTileCacheLayer& layer)>& processor) const
{
}

void Awareness::processTiles(std::vector<const dtCompressedTile*> tiles,
        const std::function<void(unsigned int, dtTileCachePolyMesh&, float* origin, float cellsize, float cellheight, dtTileCacheLayer& layer)>& processor) const
{
}


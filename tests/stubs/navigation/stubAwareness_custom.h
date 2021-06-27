//Add custom implementations of stubbed functions here; this file won't be rewritten when re-generating stubs.
#include "RecastDetour/Detour/Include/DetourNavMesh.h"
#include "RecastDetour/Detour/Include/DetourNavMeshQuery.h"
#include "RecastDetour/Detour/Include/DetourNavMeshBuilder.h"
#include "RecastDetour/DetourTileCache/Include/DetourTileCache.h"
#include "RecastDetour/DetourTileCache/Include/DetourTileCacheBuilder.h"
#include "RecastDetour/Detour/Include/DetourCommon.h"
#include "RecastDetour/Detour/Include/DetourObstacleAvoidance.h"
#include "navigation/AwarenessUtils.h"

template<typename TItem>
class MRUList
{
};

#ifndef STUB_Awareness_Awareness
#define STUB_Awareness_Awareness
Awareness::Awareness(long domainEntityId, float agentRadius, float agentHeight, float stepHeight, IHeightProvider& heightProvider, const WFMath::AxisBox<3>& extent, int tileSize )
    : mHeightProvider(heightProvider), mDomainEntityId(domainEntityId)
{

}
#endif //STUB_Awareness_Awareness

#ifndef STUB_Awareness_Awareness_DTOR
#define STUB_Awareness_Awareness_DTOR
Awareness::~Awareness() = default;
#endif //STUB_Awareness_Awareness_DTOR

//Add custom implementations of stubbed functions here; this file won't be rewritten when re-generating stubs.
#include <Mercator/Terrain.h>
#include <Mercator/TileShader.h>

#ifndef STUB_TerrainProperty_createShaders
#define STUB_TerrainProperty_createShaders
std::pair<std::unique_ptr<Mercator::TileShader>, std::vector<std::string>> TerrainProperty::createShaders(const Atlas::Message::ListType& surfaceList) const
{
    return {};
}
#endif //STUB_TerrainProperty_createShaders

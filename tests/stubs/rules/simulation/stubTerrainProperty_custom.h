//Add custom implementations of stubbed functions here; this file won't be rewritten when re-generating stubs.
#include <Mercator/Terrain.h>
#include <Mercator/TileShader.h>

#ifndef STUB_TerrainProperty_TerrainProperty
#define STUB_TerrainProperty_TerrainProperty
TerrainProperty::TerrainProperty(const TerrainProperty& rhs)
    : PropertyBase(rhs)
{

}

TerrainProperty::TerrainProperty()
    : PropertyBase()
{

}
#endif //STUB_TerrainProperty_TerrainProperty

#ifndef STUB_TerrainProperty_createShaders
#define STUB_TerrainProperty_createShaders
std::unique_ptr<Mercator::TileShader> TerrainProperty::createShaders(const Atlas::Message::ListType& surfaceList)
{
    return {};
}
#endif //STUB_TerrainProperty_createShaders

//Add custom implementations of stubbed functions here; this file won't be rewritten when re-generating stubs.
#ifndef STUB_TerrainProperty_TerrainProperty
#define STUB_TerrainProperty_TerrainProperty
TerrainProperty::TerrainProperty(const TerrainProperty& rhs)
    : PropertyBase(rhs),
      m_data(*(Mercator::Terrain*)0),
      m_tileShader(nullptr)
{

}

TerrainProperty::TerrainProperty()
    : PropertyBase(),
    m_data(*(Mercator::Terrain*)0),
    m_tileShader(nullptr)
{

}
#endif //STUB_TerrainProperty_TerrainProperty

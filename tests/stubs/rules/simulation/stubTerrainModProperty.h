// AUTOGENERATED file, created by the tool generate_stub.py, don't edit!
// If you want to add your own functionality, instead edit the stubTerrainModProperty_custom.h file.

#ifndef STUB_RULES_SIMULATION_TERRAINMODPROPERTY_H
#define STUB_RULES_SIMULATION_TERRAINMODPROPERTY_H

#include "rules/simulation/TerrainModProperty.h"
#include "stubTerrainModProperty_custom.h"

#ifndef STUB_TerrainModProperty_TerrainModProperty
//#define STUB_TerrainModProperty_TerrainModProperty
   TerrainModProperty::TerrainModProperty()
    : TerrainEffectorProperty()
  {
    
  }
#endif //STUB_TerrainModProperty_TerrainModProperty

#ifndef STUB_TerrainModProperty_TerrainModProperty
//#define STUB_TerrainModProperty_TerrainModProperty
   TerrainModProperty::TerrainModProperty(const TerrainModProperty& rhs)
    : TerrainEffectorProperty(rhs)
  {
    
  }
#endif //STUB_TerrainModProperty_TerrainModProperty

#ifndef STUB_TerrainModProperty_TerrainModProperty_DTOR
//#define STUB_TerrainModProperty_TerrainModProperty_DTOR
   TerrainModProperty::~TerrainModProperty()
  {
    
  }
#endif //STUB_TerrainModProperty_TerrainModProperty_DTOR

#ifndef STUB_TerrainModProperty_copy
//#define STUB_TerrainModProperty_copy
  TerrainModProperty* TerrainModProperty::copy() const
  {
    return nullptr;
  }
#endif //STUB_TerrainModProperty_copy

#ifndef STUB_TerrainModProperty_apply
//#define STUB_TerrainModProperty_apply
  void TerrainModProperty::apply(LocatedEntity&)
  {
    
  }
#endif //STUB_TerrainModProperty_apply

#ifndef STUB_TerrainModProperty_parseModData
//#define STUB_TerrainModProperty_parseModData
  std::unique_ptr<Mercator::TerrainMod> TerrainModProperty::parseModData(const WFMath::Point<3>& pos, const WFMath::Quaternion& orientation) const
  {
    return *static_cast<std::unique_ptr<Mercator::TerrainMod>*>(nullptr);
  }
#endif //STUB_TerrainModProperty_parseModData

#ifndef STUB_TerrainModProperty_getAttr
//#define STUB_TerrainModProperty_getAttr
  int TerrainModProperty::getAttr(const std::string&, Atlas::Message::Element&) const
  {
    return 0;
  }
#endif //STUB_TerrainModProperty_getAttr

#ifndef STUB_TerrainModProperty_setAttr
//#define STUB_TerrainModProperty_setAttr
  void TerrainModProperty::setAttr(const std::string&, const Atlas::Message::Element&)
  {
    
  }
#endif //STUB_TerrainModProperty_setAttr


#endif
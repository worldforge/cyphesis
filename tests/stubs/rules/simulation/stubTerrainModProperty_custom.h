//Add custom implementations of stubbed functions here; this file won't be rewritten when re-generating stubs.
#include "rules/simulation/TerrainModTranslator.h"
#include <Mercator/TerrainMod.h>

#ifndef STUB_TerrainModProperty_parseModData
#define STUB_TerrainModProperty_parseModData
std::unique_ptr<Mercator::TerrainMod> TerrainModProperty::parseModData(const WFMath::Point<3>& pos, const WFMath::Quaternion& orientation) const
{
    return {};
}
#endif //STUB_TerrainModProperty_parseModData

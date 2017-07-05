//Add custom implementations of stubbed functions here; this file won't be rewritten when re-generating stubs.
#include "common/const.h"
#include <Atlas/Objects/Anonymous.h>

#ifndef STUB_Location_Location
#define STUB_Location_Location

Location::Location() :
    m_simple(true), m_solid(true),
    m_boxSize(consts::minBoxSize),
    m_squareBoxSize(consts::minSqrBoxSize),
    m_loc(0)
{
}

Location::Location(LocatedEntity * rf, const Point3D & pos) :
    m_simple(true), m_solid(true),
    m_boxSize(consts::minBoxSize),
    m_squareBoxSize(consts::minSqrBoxSize), m_loc(rf), m_pos(pos)
{
}

Location::Location(LocatedEntity * rf):
    m_simple(true), m_solid(true),
    m_boxSize(consts::minBoxSize),
    m_squareBoxSize(consts::minSqrBoxSize), m_loc(rf)
{
}
#endif //STUB_Location_Location


const Vector3D distanceTo(const Location & self, const Location & other)
{
    return *((Vector3D*)nullptr);
}

const Point3D relativePos(const Location & self, const Location & other)
{
    return *((Point3D*)nullptr);
}

float squareDistance(const Location & self, const Location & other)
{
    return 0.0f;
}
float squareHorizontalDistance(const Location & self, const Location & other)
{
    return 0.0f;
}

float squareDistanceWithAncestor(const Location & self, const Location & other, const Location** ancestor)
{
    return 0.f;
}

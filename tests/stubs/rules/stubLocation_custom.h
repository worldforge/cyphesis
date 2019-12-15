//Add custom implementations of stubbed functions here; this file won't be rewritten when re-generating stubs.
#include "common/const.h"
#include "stubEntityLocation.h"
#include <Atlas/Objects/Anonymous.h>

#ifndef STUB_Location_Location
#define STUB_Location_Location

Location::Location(Ref<LocatedEntity> rf, const Point3D& pos) :
    EntityLocation(rf, pos), m_solid(true)
{
}

Location::Location(Ref<LocatedEntity> rf):
    EntityLocation(rf), m_solid(true)
{
}


Location::Location() :
    m_solid(true)
{
}

Location::Location(LocatedEntity * rf, const Point3D& pos) :
    EntityLocation(rf, pos), m_solid(true)
{
}

Location::Location(LocatedEntity * rf):
    EntityLocation(rf), m_solid(true)
{
}
#endif //STUB_Location_Location

#ifndef STUB_distanceTo
#define STUB_distanceTo

Vector3D distanceTo(const Location & self, const Location & other)
{
    return *((Vector3D*)nullptr);
}
#endif //STUB_distanceTo

#ifndef STUB_relativePos
#define STUB_relativePos
Point3D relativePos(const Location & self, const Location & other)
{
    return *((Point3D*)nullptr);
}
#endif //STUB_relativePos

#ifndef STUB_squareDistance
#define STUB_squareDistance
float squareDistance(const Location & self, const Location & other)
{
    return 0.0f;
}
#endif //STUB_squareDistance

#ifndef STUB_squareHorizontalDistance
#define STUB_squareHorizontalDistance
float squareHorizontalDistance(const Location & self, const Location & other)
{
    return 0.0f;
}
#endif //STUB_squareHorizontalDistance

#ifndef STUB_squareDistanceWithAncestor
#define STUB_squareDistanceWithAncestor
float squareDistanceWithAncestor(const Location & self, const Location & other, const Location** ancestor)
{
    return 0.f;
}
#endif //STUB_squareDistanceWithAncestor

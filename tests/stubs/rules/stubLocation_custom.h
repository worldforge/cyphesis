//Add custom implementations of stubbed functions here; this file won't be rewritten when re-generating stubs.
#include "common/const.h"
#include "stubEntityLocation.h"
#include "rules/LocatedEntity.h"
#include <Atlas/Objects/Anonymous.h>

#ifndef STUB_Location_Location
#define STUB_Location_Location

Location::Location(Ref<LocatedEntity> rf, const Point3D& pos) :
    EntityLocation(rf, pos)
{
}

Location::Location(Ref<LocatedEntity> rf):
    EntityLocation(rf)
{
}


Location::Location()
{
}

Location::Location(LocatedEntity * rf, const Point3D& pos) :
    EntityLocation(rf, pos)
{
}

Location::Location(LocatedEntity * rf):
    EntityLocation(rf)
{
}
#endif //STUB_Location_Location

#ifndef STUB_distanceTo
#define STUB_distanceTo

Vector3D distanceTo(const Location & self, const Location & other)
{
    return Vector3D();
}
#endif //STUB_distanceTo

#ifndef STUB_relativePos
#define STUB_relativePos
Point3D relativePos(const Location & self, const Location & other)
{
    return Point3D();
}
#endif //STUB_relativePos

#ifndef STUB_squareDistance
#define STUB_squareDistance
boost::optional<WFMath::CoordType> squareDistance(const Location & self, const Location & other)
{
    return 0.0;
}
#endif //STUB_squareDistance

#ifndef STUB_squareHorizontalDistance
#define STUB_squareHorizontalDistance
boost::optional<WFMath::CoordType> squareHorizontalDistance(const Location & self, const Location & other)
{
    return 0.0;
}
#endif //STUB_squareHorizontalDistance

#ifndef STUB_squareDistanceWithAncestor
#define STUB_squareDistanceWithAncestor
WFMath::CoordType squareDistanceWithAncestor(const Location & self, const Location & other, const Location** ancestor)
{
    return 0.0;
}
#endif //STUB_squareDistanceWithAncestor

#include "BBoxComparers.h"
#include "AttributeComparerWrapper.h"
#include "rulesets/LocatedEntity.h"

namespace EntityFilter
{
namespace Comparers
{
using namespace Atlas::Message;
//NOTE: it might be helpful to take measures against segfaults
float BBoxComparers::AreaCalculator(Element& points)
{
    auto& points_list = points.List();
    return (points_list[3].Float() - points_list[0].Float())
            * (points_list[4].Float() - points_list[1].Float());
}

float BBoxComparers::VolumeCalculator(Element& points)
{
    auto& points_list = points.List();
    return (points_list[3].Float() - points_list[0].Float())
            * (points_list[4].Float() - points_list[1].Float())
            * (points_list[5].Float() - points_list[2].Float());
}

float BBoxComparers::HeightCalculator(Element& points)
{
    auto& points_list = points.List();
    return points_list[5].Float() - points_list[2].Float();
}

float BBoxComparers::LengthCalculator(Element& points)
{
    auto& points_list = points.List();
    return points_list[4].Float() - points_list[1].Float();
}

float BBoxComparers::WidthCalculator(Element& points)
{
    auto& points_list = points.List();
    return points_list[3].Float() - points_list[0].Float();
}
}
}

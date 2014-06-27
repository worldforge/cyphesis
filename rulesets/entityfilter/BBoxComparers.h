#include "AttributeComparerWrapper.h"
#include "AttributeComparers.h"

namespace EntityFilter
{
namespace Comparers
{
///\brief a class containing a collection of functions to work with
///bbox property
///
///All bbox properties are passed here as lists(within Element) that contain 6 floats.
///The floats represent 3-D coords of two opposite corners.
///First 3 values - x, y, z coords of lower corner.
///last 3 values - x, y, z coords of upper corner.
///These are used to calculate various values
///This class should never be instantiated
class BBoxComparers {
    public:
        static float AreaCalculator(Atlas::Message::Element& points);
        static float VolumeCalculator(Atlas::Message::Element& points);
        ///\brief difference between y coords;
        static float LengthCalculator(Atlas::Message::Element& points);
        ///\brief difference between x coords;
        static float WidthCalculator(Atlas::Message::Element& points);
        ///\brief difference between z coords;
        static float HeightCalculator(Atlas::Message::Element& points);
};
}
}

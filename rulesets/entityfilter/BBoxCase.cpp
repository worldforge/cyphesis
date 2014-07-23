#include "BBoxCase.h"
#include "BBoxComparers.h"
#include "rulesets/LocatedEntity.h"

namespace EntityFilter
{
namespace Cases
{

BBoxCase::BBoxCase(const std::string& attribute,
                   const std::string& value,
                   const std::string& comp_operator) :
        m_value(std::stof(value))
{

    //We are certain to have one of these cases, since the parser had to match one.

    //Get an appropriate comparer
    if (comp_operator == "=") {
        m_comparerMethod = &Comparers::eqComparer;
    } else if (comp_operator == "!=") {
        m_comparerMethod = &Comparers::neqComparer;
    } else if (comp_operator == ">") {
        m_comparerMethod = &Comparers::gComparer;
    } else if (comp_operator == "<") {
        m_comparerMethod = &Comparers::lComparer;
    } else if (comp_operator == ">=") {
        m_comparerMethod = &Comparers::geComparer;
    } else {
        m_comparerMethod = &Comparers::leComparer;
    }

    //Get the calculator for the required properties.
    if (attribute == "volume" || attribute == "Volume") {
        m_propertyCalculator = &Comparers::BBoxComparers::VolumeCalculator;
    } else if (attribute == "height" || attribute == "Height") {
        m_propertyCalculator = &Comparers::BBoxComparers::HeightCalculator;
    } else if (attribute == "length" || attribute == "Length") {
        m_propertyCalculator = &Comparers::BBoxComparers::LengthCalculator;
    } else if (attribute == "width" || attribute == "Width") {
        m_propertyCalculator = &Comparers::BBoxComparers::WidthCalculator;
    } else {
        m_propertyCalculator = &Comparers::BBoxComparers::AreaCalculator;
    }
}
bool BBoxCase::testCase(LocatedEntity& entity)
{
    Atlas::Message::Element bbox_attr;
    if (entity.getAttr("bbox", bbox_attr) != 0) {
        return false;
    }
    return m_comparerMethod(m_propertyCalculator(bbox_attr), m_value);
}
}
}

#include "AbstractCase.h"
#include "AttributeComparerWrapper.h"
#include <Atlas/Objects/Operation.h>
namespace EntityFilter
{
namespace Cases
{
///\brief a class to handle BBox property case
///
///During initialization, appropriate methods for calculating the property and comparison
///are chosen, according to attribute and comp_operator. Value is stored as a float.
class BBoxCase : public AbstractCase {
    public:
        BBoxCase(const std::string& attribute,
                 const std::string& value,
                 const std::string& comp_operator);
        ~BBoxCase()
        {
        }
        ;
        //\brief tests BBox case for a given entity. Returns false if
        //entity doesn't have bbox property
        bool testCase(LocatedEntity& entity);
    private:
        float m_value;
        float (*m_propertyCalculator)(Atlas::Message::Element&);
        bool (*m_comparerMethod)(float, float);

};
}
}

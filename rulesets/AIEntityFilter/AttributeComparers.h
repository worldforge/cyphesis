
#ifndef RULESETS_FILTER_ATTRIBUTECOMPARERS_H_
#define RULESETS_FILTER_ATTRIBUTECOMPARERS_H_

#include "AttributeComparerWrapper.h"
#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>

#include "../MemMap.h"
#include "../MemEntity.h"
#include "../common/TypeNode.h"

namespace EntityFilter{
namespace Comparers{

//String comparer class. Only = or != operators are allowed.
class StringAttributeComparer : public AttributeComparerWrapper {
    public:
        StringAttributeComparer(const std::string &attribute_name,
                                const std::string &value_str,
                                const std::string &comp_operator);
        bool compare(LocatedEntity* entity);
    private:
        bool (*comparer_method)(std::string, std::string);
        std::string m_value;
        std::string m_attributeName;
        Atlas::Message::Element m_atlasAttr;
};


//A class used to compare numeric attributes
class NumericAttributeComparer : public AttributeComparerWrapper {
    public:
        NumericAttributeComparer(const std::string &attribute_name,
                                 const std::string &value_str,
                                 const std::string &comp_operator);
        bool compare(LocatedEntity* entity);
    private:
        //a pointer to the comparer method with the right comparison operator
        bool (*comparer_method)(float, float);
        std::string m_attributeName;
        float m_value;
        Atlas::Message::Element m_atlasAttr;

};

//Comparison functions.
template<typename ValType>
bool eqComparer(ValType val1, ValType val2)
{
    return val1 == val2;
}
template<typename ValType>
bool neqComparer(ValType val1, ValType val2)
{
    return val1 != val2;
}
template<typename ValType>
bool gComparer(ValType val1, ValType val2)
{
    return val1 > val2;
}
template<typename ValType>
bool geComparer(ValType val1, ValType val2)
{
    return val1 >= val2;
}
template<typename ValType>
bool lComparer(ValType val1, ValType val2)
{
    return val1 < val2;
}
template<typename ValType>
bool leComparer(ValType val1, ValType val2)
{
    return val1 <= val2;
}

}
}



#endif

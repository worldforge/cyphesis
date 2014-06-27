#ifndef RULESETS_FILTER_ATTRIBUTECOMPARERWRAPPER_H_
#define RULESETS_FILTER_ATTRIBUTECOMPARERWRAPPER_H_

#include "../MemMap.h"

namespace EntityFilter
{
namespace Comparers
{
///\brief Wrapper class for attribute comparers
///
///Every inheriting class must implement compare method that matches a given entity
class AttributeComparerWrapper {
    public:
        virtual bool compare(LocatedEntity& entity) = 0;
        virtual ~AttributeComparerWrapper()
        {
        }
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


#ifndef RULESETS_FILTER_ATTRIBUTECOMPARERWRAPPER_H_
#define RULESETS_FILTER_ATTRIBUTECOMPARERWRAPPER_H_

#include "../MemMap.h"
#include "../MemEntity.h"
#include "../common/TypeNode.h"


namespace EntityFilter
{
namespace Comparers
{
class AttributeComparerWrapper {
    public:
        virtual bool compare(LocatedEntity* entity) = 0;
        virtual ~AttributeComparerWrapper()
        {
        }
};
}
}

#endif

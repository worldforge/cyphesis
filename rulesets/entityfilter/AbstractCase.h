#ifndef RULESETS_FILTER_ABSTRACTCASE_H_
#define RULESETS_FILTER_ABSTRACTCASE_H_

#include "../MemMap.h"

namespace EntityFilter
{
namespace Cases
{

//This is a base class for all cases.
//After being initialized, every inheriting class must be able to
//test a condition with only entity provided as an argument.
class AbstractCase {
    public:
        virtual bool testCase(LocatedEntity& entity) = 0;
        virtual ~AbstractCase()
        {
        }
};
}
}

#endif

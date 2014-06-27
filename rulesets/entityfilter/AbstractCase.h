#ifndef RULESETS_FILTER_ABSTRACTCASE_H_
#define RULESETS_FILTER_ABSTRACTCASE_H_

#include "../MemMap.h"

namespace EntityFilter
{
namespace Cases
{

///\brief This is a base class for all cases.
///After being initialized, every inheriting class must be able to
///test a condition with only entity provided as an argument.
///
///Ideally, all cases have to do all of the decision making during the
///initialization, so that there is minimum branching or decision making
///during the actual matching.
class AbstractCase {
    public:
        //\brief this function is called to test any case for a given entity
        virtual bool testCase(LocatedEntity& entity) = 0;
        virtual ~AbstractCase()
        {
        }
};
}
}

#endif

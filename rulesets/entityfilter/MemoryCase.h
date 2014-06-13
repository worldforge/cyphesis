#include "AbstractCase.h"

#ifndef MEMORYCASE_H_
#define MEMORYCASE_H_

namespace EntityFilter
{
namespace Cases
{

class MemoryCase : public AbstractCase {
    public:
        MemoryCase(std::string memory,
                   std::string value,
                   std::string comp_operator);
        virtual ~MemoryCase()
        {
        }
        virtual bool testCase(LocatedEntity& entity);
    private:

};
}
}

#endif

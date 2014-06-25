#ifndef RULESETS_FILTER_ATTRIBUTECASES_H_
#define RULESETS_FILTER_ATTRIBUTECASES_H_

#include "AbstractCase.h"
#include "AttributeComparerWrapper.h"
#include "AttributeComparers.h"

namespace EntityFilter
{
namespace Cases
{

// a class to handle "entity.attribute" query
class EntityAttributeCase : public AbstractCase {
    public:
        EntityAttributeCase(const std::string &attribute,
                            const std::string &value,
                            const std::string &comp_operator);
        ~EntityAttributeCase();
        bool testCase(LocatedEntity& entity);
        static Comparers::AttributeComparerWrapper* getComparer(const std::string &attribute,
                                                                const std::string &value,
                                                                const std::string &comp_operator);
    private:
        Comparers::AttributeComparerWrapper* m_comparer;
        std::string m_attributeName;
        std::string m_valueStr;
};

//A class to handle "entity.type" query
class EntityTypeCase : public AbstractCase {
    public:
        EntityTypeCase(const std::string &value,
                       const std::string &comp_operator);
        ~EntityTypeCase();
        bool testCase(LocatedEntity& entity);
        static Comparers::AttributeComparerWrapper* getComparer(const std::string &value,
                                                                const std::string &comp_operator);
    private:
        Comparers::AttributeComparerWrapper* m_comparer;
        std::string m_valueStr;
};
}
}

#endif

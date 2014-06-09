/*
 * AttributeCases.h
 *
 *  Created on: Jun 8, 2014
 *      Author: ytaben
 */

#ifndef RULESETS_FILTER_ATTRIBUTECASES_H_
#define RULESETS_FILTER_ATTRIBUTECASES_H_

#include "AbstractCase.h"
#include "AttributeComparerWrapper.h"
#include "AttributeComparers.h"
#include "../MemMap.h"

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
        bool testCase(LocatedEntity* entity);
    private:
        Comparers::AttributeComparerWrapper* m_comparer;
        std::string m_attributeName;
        std::string m_valueStr;
};

//A class to handle "entity.type" query
class EntityTypeCase : public AbstractCase {
    public:
        EntityTypeCase(const std::string &value, const std::string &comp_operator);
        bool testCase(LocatedEntity* entity);
    private:
        bool (*m_comparer_method)(std::string, std::string);
        std::string m_valueStr;
};
}
}

#endif

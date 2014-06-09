

#ifndef RULESETS_FILTER_H_
#define RULESETS_FILTER_H_

#include <boost/tuple/tuple.hpp>
#include "../MemMap.h"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>


#include <boost/spirit/include/qi.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/io.hpp>
#include "ParserDefinitions.h"
#include "AttributeCases.h"
#include "AttributeComparers.h"


///\brief This class is used to search entities in NPC's memory
///using a query as a filter
///
///A query must consist of tokens separated by && operator
///a valid token indicates an object to search (memory or entity),
///a property that will be used as criteria, an operator that
///will be used for comparison and the value that will be compared
///i.e.: entity.property==val
namespace EntityFilter{


//Float comparers


namespace Comparers
{
class AttributeComparerWrapper;
class StringAttributeComparer;
class NumericAttributeComparer;
class ListAttributeComparer{

};
}

//Classes to handle various cases of subjects.

namespace Cases{
class AbstractCase;
class EntityAttributeCase;
class EntityTypeCase;
}

//This class is meant to further parse tokens into appropriate structures.
class ParsedCondition{
    public:
        ParsedCondition(const parser::condition &all_conditions);
        bool isTrue(LocatedEntity* entity);
    private:
        Cases::AbstractCase* m_case;
};


///Main Filtering class
class Filter {
    public:
        ///\brief Initialize a filter with a given query
        ///@param what query to be used for filtering
        Filter(const std::string &what);

        ///\brief Return a Vector with entities that match the query
        void search(MemEntityDict all_entities, EntityVector &res);

        //\brief Return entities that pass all conditions.
        void attributeSearchAnd(const MemEntityDict &all_entities,
                                EntityVector& res);
        //\brief Return Entities that pass at least one condition
        void attributeSearchOr(const MemEntityDict &all_entities,
                               EntityVector& res);
    private:
        std::list<parser::condition> m_conditions;
        std::list<ParsedCondition> m_parsedConditions;
        void recordCondition(const std::string &token,
                             const std::string &comp_operator,
                             int delimiter_index);
        bool testAttributeMatch(LocatedEntity *entity,
                                const std::string &attribute_name,
                                const std::string &attribute_value,
                                const std::string &comp_operator);

};


}
#endif

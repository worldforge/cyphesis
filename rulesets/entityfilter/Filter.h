#ifndef RULESETS_FILTER_H_
#define RULESETS_FILTER_H_

#include "ParserDefinitions.h"
#include "AttributeCases.h"
#include "AttributeComparers.h"
#include "../MemMap.h"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>

#include <boost/spirit/include/qi.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/io.hpp>

///\brief This class is used to search entities in NPC's memory
///using a query as a filter
///
///A query must consist of tokens separated by && operator
///a valid token indicates an object to search (memory or entity),
///a property that will be used as criteria, an operator that
///will be used for comparison and the value that will be compared
///i.e.: entity.property==val
namespace EntityFilter
{

namespace Comparers
{
class AttributeComparerWrapper;
class StringAttributeComparer;
class NumericAttributeComparer;
class ListAttributeComparer {

};
}

//Classes to handle various cases of subjects.

namespace Cases
{
class AbstractCase;
class EntityAttributeCase;
class EntityTypeCase;
}

///This class is meant to further parse tokens into appropriate structures.
class ParsedCondition {
    public:
        ParsedCondition(const parser::condition &all_conditions);
        bool isTrue(LocatedEntity& entity);
    private:
        Cases::AbstractCase* m_case;
};

///Main Filtering class
class Filter {
    public:
        ///\brief Initialize a filter with a given query
        ///@param what query to be used for filtering
        Filter(const std::string &what);

        //TODO: Allow for various LocatedEntity containers.
        ///\brief test given entity for a match
        ///@param entity - entity to be tested
        bool match(LocatedEntity& entity);
    private:
        //Theses structures are used to store conditions accounting for precedence, but should be
        //changed to more sophisticated ones later.
        std::list<parser::condition> m_conditions;
        std::list<std::list<parser::condition>> m_allConditions;
        std::list<std::list<ParsedCondition>> m_allParsedConditions;
        std::list<ParsedCondition> m_parsedConditions;
        bool check_and_block(LocatedEntity &entity,
                             std::list<ParsedCondition> conditions);

};
///\brief An Exception class to be thrown in case of an invalid query.
class InvalidQueryException : public std::exception {
    public:
        virtual const char* what() const throw ()
        {
            return "Attempted creating entity filter with invalid query";
        }
};

}

#endif

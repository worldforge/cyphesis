

#ifndef RULESETS_FILTER_H_
#define RULESETS_FILTER_H_

#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/tuple/tuple.hpp>
#include "MemMap.h"


///\brief This class is used to search entities in NPC's memory
///using a query as a filter
///
///A query must consist of tokens separated by && operator
///a valid token indicates an object to search (memory or entity),
///a property that will be used as criteria, an operator that
///will be used for comparison and the value that will be compared
///i.e.: entity.property==val

namespace parser
{


    struct condition
    {
        std::string target;
        std::string attribute;
        std::string comp_operator;
        std::string value_str;
    };
}
class Filter{
    public:
        ///\brief Initialize a filter with a given query
        ///@param what query to be used for filtering
        ///
        ///Constructor for filter parses the query and splits it into triplets with
        ///property to search for, its value and the operator to use for comparison
        Filter(const std::string &what);

        ///\brief Return a Vector with entities that match the query
        ///FIXME: Return datatype needs to be reviewed.
        EntityVector search(MemEntityDict all_entities, EntityVector &res);
    private:
        std::list<parser::condition> m_conditions;
        void recordCondition(const std::string &token,
                             const std::string &comp_operator,
                             int delimiter_index);

};
#endif

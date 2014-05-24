

#ifndef RULESETS_FILTER_H_
#define RULESETS_FILTER_H_
#endif

#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/tuple/tuple.hpp>


///\brief This class is used to search entities in NPC's memory
///using a query as a filter
///
///A query must consist of tokens separated by && operator
///a valid token indicates an object to search (memory or entity),
///a property that will be used as criteria, an operator that
///will be used for comparison and the value that will be compared
///i.e.: entity.property==val
class Filter{
    public:
        ///\brief Initialize a filter with a given query
        ///@param what query to be used for filtering
        ///
        ///Constructor for filter parses the query and splits it into triplets with
        ///property to search for, its value and the operator to use for comparison
        Filter(const std::string &what, const EntityDict &entities);

        ///\brief Return a Vector with entities that match the query
        EntityVector Search();
    private:
        std::list<boost::tuple<std::string, std::string, std::string> > conditions;
        EntityDict all_entities;
        void recordCondition(const std::string &token,
                             const std::string &comp_operator,
                             int delimiter_index);
};

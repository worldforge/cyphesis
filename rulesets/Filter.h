

#ifndef RULESETS_FILTER_H_
#define RULESETS_FILTER_H_
#endif

#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>


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
        Filter(std::string &what);
};

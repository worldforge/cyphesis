#ifndef RULESETS_FILTER_H_
#define RULESETS_FILTER_H_

#include <string>

class LocatedEntity;
///\brief This class is used to search entities in NPC's memory
///using a query as a filter
namespace EntityFilter
{
class ProviderFactory;
class QueryContext;
class Predicate;
///Main Filtering class
class Filter {
    public:
        ///\brief Initialize a filter with a given query
        ///@param what query to be used for filtering
        ///@param factory a factory to handle segments
        Filter(const std::string &what, const ProviderFactory& factory);
        ~Filter();

        ///\brief test given QueryContext for a match
        bool match(const QueryContext& context);
    private:
        //The top predicate node used for testing
        Predicate* m_predicate;
};
}
#endif

#ifndef RULESETS_FILTER_H_
#define RULESETS_FILTER_H_

#include "ParserDefinitions.h"

///\brief This class is used to search entities in NPC's memory
///using a query as a filter
namespace EntityFilter
{
///Main Filtering class
class Filter {
    public:
        ///\brief Initialize a filter with a given query
        ///@param what query to be used for filtering
        ///@param factory a pointer to a factory to handle segments
        Filter(const std::string &what, ProviderFactory* factory);
        ~Filter();

        ///\brief test given entity for a match
        ///@param entity - entity to be tested
        bool match(LocatedEntity& entity);
        ///\brief test given QueryContext for a match
        bool match(const QueryContext& context);
    private:
        //The top predicate node used for testing
        Predicate* m_predicate;
};
}
#endif

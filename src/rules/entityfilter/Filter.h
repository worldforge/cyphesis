#ifndef RULESETS_FILTER_H_
#define RULESETS_FILTER_H_

#include <string>
#include <memory>

class LocatedEntity;
///\brief This class is used to perform matches against an entity.
namespace EntityFilter {
    class ProviderFactory;

    class QueryContext;

    class Predicate;

    class Filter
    {
        public:
            ///\brief Initialize a filter with a given query
            ///@param what query to be used for filtering
            ///@param factory a factory to handle segments
            Filter(const std::string& what, const ProviderFactory& factory);

            ~Filter();

            ///\brief test given QueryContext for a match
            bool match(const QueryContext& context) const;

            const std::string& getDeclaration() const;

        private:
            const std::string m_declaration;
            //The top predicate node used for testing
            std::shared_ptr<Predicate> m_predicate;
    };
}
#endif

#include "Filter.h"
#include "ParserDefinitions.h"

using namespace boost;
namespace qi = boost::spirit::qi;
using qi::no_case;

namespace EntityFilter {
    Filter::Filter(const std::string& what, const ProviderFactory& factory)
            : m_declaration(what)
    {
        parser::query_parser<std::string::const_iterator> grammar(factory);
        //boost::spirit::qi::debug(grammar.parenthesised_predicate_g);
        auto iter_begin = what.begin();
        auto iter_end = what.end();
        bool parse_success;
        try {
            parse_success = qi::phrase_parse(iter_begin, iter_end, grammar,
                                             boost::spirit::qi::space, m_predicate);
        } catch (const std::invalid_argument& e) {
            throw std::invalid_argument(String::compose("Error when parsing '%1':\n%2", what, e.what()));
        }
        if (!(parse_success && iter_begin == iter_end)) {
            auto parsedPart = what.substr(0, iter_begin - what.begin());
            throw std::invalid_argument(String::compose("Attempted creating entity filter with invalid query. Query was '%1'.\n Parser error was at '%2'", what, parsedPart));
        }
    }

    Filter::~Filter() = default;

    bool Filter::match(const QueryContext& context) const
    {

        return m_predicate->isMatch(context);
    }

    const std::string& Filter::getDeclaration() const
    {
        return m_declaration;
    }

}

#include "Filter.h"

using namespace boost;
namespace qi = boost::spirit::qi;
using qi::no_case;

namespace EntityFilter
{
Filter::Filter(const std::string &what, ProviderFactory* factory)
{
    parser::query_parser<std::string::const_iterator> grammar(factory);
    auto iter_begin = what.begin();
    auto iter_end = what.end();

    bool parse_success = qi::phrase_parse(iter_begin, iter_end, grammar,
                                          boost::spirit::ascii::space, m_predicate);

    if (!(parse_success && iter_begin == iter_end)) {
        throw std::invalid_argument(String::compose("Attempted creating entity filter with invalid query. Query was '%1'", what));
    }
}

Filter::~Filter(){
    delete m_predicate;
}

bool Filter::match(LocatedEntity& entity)
{
    return m_predicate->isMatch(QueryContext{entity});
}

bool Filter::match(const QueryContext& context){

    return m_predicate->isMatch(context);
}
}

#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#define private public

#include "TestBase.h"

#include "rulesets/entityfilter/ParserDefinitions.h"
#include "rulesets/entityfilter/Providers.h"

#include <boost/spirit/include/qi.hpp>

using namespace EntityFilter;
using namespace boost::spirit;

///\These tests aim at verifying that entity filter parser builds
///correct predicates for given queries
class ParserTest : public Cyphesis::TestBase {
    private:
        Predicate *m_predicate;
        ProviderFactory *m_provider_factory;

        //Grammar definition for testing
        parser::query_parser<std::string::const_iterator> m_grammar;

        //A helper function to build a predicate for a given query
        Predicate* ConstructPredicate(std::string query);
    public:
        ParserTest();

        void setup();
        void teardown();

        void test_ComparisonOperators();
        void test_LogicalOperators();

};

ParserTest::ParserTest()
{
}

void ParserTest::setup()
{
    m_provider_factory = new ProviderFactory();
    m_grammar = parser::query_parser<std::string::const_iterator>(
            m_provider_factory);
}

void ParserTest::teardown()
{
    delete m_grammar;
    delete m_provider_factory;
}

void ParserTest::test_ComparisonOperators()
{
    using ComparePredicate::Comparator;
    ComparePredicate *pred;

    pred = ConstructPredicate("1 = 2");
    assert(pred->m_comparator == Comparator::EQUALS);
    delete pred;

    pred = ConstructPredicate("1 != 2");
    assert(pred->m_comparator == Comparator::NOT_EQUALS);
    delete pred;

    pred = ConstructPredicate("1 > 2");
    assert(pred->m_comparator == Comparator::GREATER);
    delete pred;

    pred = ConstructPredicate("1 < 2");
    assert(pred->m_comparator == Comparator::LESS);
    delete pred;

    pred = ConstructPredicate("1 <= 2");
    assert(pred->m_comparator == Comparator::GREATER_EQUAL);
    delete pred;

    pred = ConstructPredicate("1 >= 2");
    assert(pred->m_comparator == Comparator::LESS_EQUAL);
    delete pred;
}

Predicate* ParserTest::ConstructPredicate(std::string query)
{
    std::string what = "2 < 1";
    auto iter_begin = query.begin();
    auto iter_end = query.end();

    bool parse_success = qi::phrase_parse(iter_begin, iter_end, m_grammar,
                                          boost::spirit::ascii::space,
                                          m_predicate);
    if (!(parse_success && iter_begin == iter_end)) {
        throw std::invalid_argument(
                "Attempted creating entity filter with invalid query");
    }

}

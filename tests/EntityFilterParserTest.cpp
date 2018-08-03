#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif


#include "TestBase.h"

#include "rulesets/entityfilter/Filter.h"
#include "rulesets/entityfilter/ParserDefinitions.h"
#include "rulesets/entityfilter/Providers.h"

#include "rulesets/EntityProperty.h"
#include "rulesets/Domain.h"
#include "rulesets/AtlasProperties.h"
#include "rulesets/OutfitProperty.h"
#include "rulesets/BBoxProperty.h"
#include "common/Property.h"
#include "rulesets/BaseWorld.h"
#include "common/log.h"
#include "common/Inheritance.h"

#include "rulesets/Entity.h"
#include "common/TypeNode.h"

#include <wfmath/point.h>
#include <Atlas/Objects/Anonymous.h>

#include <cassert>

static std::map<std::string, TypeNode*> types;

using namespace EntityFilter;
using namespace boost::spirit;

///\These tests aim at verifying that entity filter parser builds
///correct predicates for given queries
class ParserTest : public Cyphesis::TestBase {
    private:
        //A helper function to build a predicate for a given query
        Predicate* ConstructPredicate(const std::string &query);
    public:
        ParserTest();

        void setup();
        void teardown();

        void test_ComparisonOperators();
        void test_LogicalOperators();
        void test_Literals();

        Inheritance* m_inheritance;
};

ParserTest::ParserTest()
{
    ADD_TEST(ParserTest::test_ComparisonOperators);
    ADD_TEST(ParserTest::test_LogicalOperators);
    ADD_TEST(ParserTest::test_Literals);
}

void ParserTest::setup()
{
    m_inheritance = new Inheritance();
}

void ParserTest::teardown()
{
    delete m_inheritance;
}

void ParserTest::test_ComparisonOperators()
{
    ComparePredicate *pred;

    pred = (ComparePredicate*)ConstructPredicate("1 = 2");
    assert(pred->m_comparator == ComparePredicate::Comparator::EQUALS);
    delete pred;

    pred = (ComparePredicate*)ConstructPredicate("1 != 2");
    assert(pred->m_comparator == ComparePredicate::Comparator::NOT_EQUALS);
    delete pred;

    pred = (ComparePredicate*)ConstructPredicate("1 > 2");
    assert(pred->m_comparator == ComparePredicate::Comparator::GREATER);
    delete pred;

    pred = (ComparePredicate*)ConstructPredicate("1 < 2");
    assert(pred->m_comparator == ComparePredicate::Comparator::LESS);
    delete pred;

    pred = (ComparePredicate*)ConstructPredicate("1 <= 2");
    assert(pred->m_comparator == ComparePredicate::Comparator::LESS_EQUAL);
    delete pred;

    pred = (ComparePredicate*)ConstructPredicate("1 >= 2");
    assert(pred->m_comparator == ComparePredicate::Comparator::GREATER_EQUAL);
    delete pred;

    pred = (ComparePredicate*)ConstructPredicate("entity.container contains 1");
    assert(pred->m_comparator == ComparePredicate::Comparator::CONTAINS);
    delete pred;

    pred = (ComparePredicate*)ConstructPredicate("1 in entity.container");
    assert(pred->m_comparator == ComparePredicate::Comparator::IN);
    delete pred;

    //Instance_of can only be created for existing types
    TypeNode* thingType = new TypeNode("thing");
    types["thing"] = thingType;
    pred = (ComparePredicate*)ConstructPredicate(
            "types.thing = entity.type");
    assert(pred->m_comparator == ComparePredicate::Comparator::EQUALS);
    delete pred;
    types["thing"] = nullptr;
    delete thingType;

    try { //Should throw an exception for nonexisting type
        pred = (ComparePredicate*)ConstructPredicate(
                "types.nonexistant = entity.type");
        assert(false);
    } catch (std::invalid_argument &e) {
    }
}

void ParserTest::test_LogicalOperators()
{
    Predicate *pred;

    pred = ConstructPredicate("1 = 2 or 3 = 4");
    assert(typeid(*pred) == typeid(OrPredicate));
    delete pred;

    pred = ConstructPredicate("1 = 2 and 3 = 4");
    assert(typeid(*pred) == typeid(AndPredicate));
    delete pred;

    pred = ConstructPredicate("!5 = 6");
    assert(typeid(*pred) == typeid(NotPredicate));

    pred = ConstructPredicate("not 7 = 8");
    assert(typeid(*pred) == typeid(NotPredicate));

    //Test precedence. not should be applied to 1 = 2, not the whole expression
    pred = ConstructPredicate("not 1 = 2 and 3 = 4");
    assert(typeid(*pred) == typeid(AndPredicate));
}

void ParserTest::test_Literals()
{
    ComparePredicate *pred;
    using Atlas::Message::Element;

    //Test int and single quote string
    pred = (ComparePredicate*)ConstructPredicate("1 = '1'");
    FixedElementProvider *lhs = (FixedElementProvider *)pred->m_lhs;
    FixedElementProvider *rhs = (FixedElementProvider *)pred->m_rhs;

    ASSERT_TRUE(lhs->m_element == Element(1));
    ASSERT_TRUE(rhs->m_element == Element("1"));
    delete pred;

    //Test double and bool
    pred = (ComparePredicate*)ConstructPredicate("1.25 = true");
    lhs = (FixedElementProvider *)pred->m_lhs;
    rhs = (FixedElementProvider *)pred->m_rhs;

    ASSERT_TRUE(lhs->m_element == Element(1.25));
    ASSERT_TRUE(rhs->m_element == Element(true));
    delete pred;

    //Test list and double quoted string
    pred = (ComparePredicate*)ConstructPredicate("[1, 2, 3] = '\"literal\"'");
    lhs = (FixedElementProvider *)pred->m_lhs;
    rhs = (FixedElementProvider *)pred->m_rhs;

    ASSERT_TRUE(lhs->m_element == Element(std::vector<Element> { 1, 2, 3 }));
    ASSERT_TRUE(rhs->m_element == Element("\"literal\""));
    delete pred;

}

Predicate* ParserTest::ConstructPredicate(const std::string &query)
{
    auto iter_begin = query.begin();
    auto iter_end = query.end();
    ProviderFactory factory{};
    parser::query_parser<std::string::const_iterator> grammar(factory);

    Predicate* pred;

    bool parse_success = qi::phrase_parse(iter_begin, iter_end, grammar,
                                          boost::spirit::ascii::space, pred);

    if (!(parse_success && iter_begin == iter_end)) {
        throw std::invalid_argument(
                "Attempted creating entity filter with invalid query");
    }

    return pred;

}

int main(int argc, char ** argv)
{
    ParserTest t;

    return t.run();
}

//Stubs

#include "stubs/common/stubVariable.h"
#include "stubs/common/stubMonitors.h"
#include "stubs/rulesets/stubDomainProperty.h"
#include "stubs/rulesets/stubDensityProperty.h"
#include "stubs/rulesets/stubScaleProperty.h"
#include "stubs/rulesets/stubAtlasProperties.h"
#include "stubs/common/stubCustom.h"
#include "stubs/common/stubRouter.h"
#include "stubs/rulesets/stubBaseWorld.h"
#include "stubs/rulesets/stubLocation.h"

#define STUB_Inheritance_getType
const TypeNode* Inheritance::getType(const std::string & parent)
{
    auto I = types.find(parent);
    if (I == types.end()) {
        return 0;
    }
    return I->second;
}

#include "stubs/common/stubInheritance.h"


void log(LogLevel lvl, const std::string & msg)
{
}

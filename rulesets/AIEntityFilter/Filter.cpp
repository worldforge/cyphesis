#include "Filter.h"
#include "../MemMap.h"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>

#include "../MemMap.h"
#include "../MemEntity.h"
#include "../common/TypeNode.h"

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/io.hpp>

#include <list>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/algorithm/string.hpp>
#include "../LocatedEntity.h"

using namespace boost;
using namespace boost::algorithm;
namespace qi = boost::spirit::qi;

namespace EntityFilter
{
void Filter::attributeSearchAnd(const MemEntityDict &all_entities,
                                EntityVector& res)
{
    if (m_conditions.empty()) {
        return;
    }
    bool pass;
    for (auto entity_pair : all_entities) {
        pass = true;
        for (auto condition : m_parsedConditions) {
            if (condition.isTrue(entity_pair.second)) {
                pass = false;
                break;
            }
        }
        if (pass) {
            res.push_back(entity_pair.second);
        }
    }
}

void Filter::attributeSearchOr(const MemEntityDict &all_entities,
                               EntityVector& res)
{
    if (m_conditions.empty()) {
        return;
    }
    for (auto entity_pair : all_entities) {
        for (auto condition : m_parsedConditions) {
            if (condition.isTrue(entity_pair.second)) {
                res.push_back(entity_pair.second);
                break;
            }
        }
    }
}

Filter::Filter(const std::string &what)
{

    parser::query_parser<std::string::const_iterator> grammar;
    auto iter_begin = what.begin();
    auto iter_end = what.end();
    qi::phrase_parse(iter_begin, iter_end, grammar, boost::spirit::ascii::space,
                     m_conditions);
    //TODO: Check if parsing is successful
    for (auto condition : m_conditions) {
        m_parsedConditions.push_back(ParsedCondition(condition));
    }

}

void Filter::search(MemEntityDict all_entities, EntityVector &res)
{

}

ParsedCondition::ParsedCondition(const parser::condition &unparsed_condition)
{

    //Determine what kind of subject we have using spirit::qi grammar rules.
    std::string subject = unparsed_condition.attribute;
    std::string::iterator iter_begin = subject.begin();
    std::string::iterator iter_end = subject.end();
    std::string parsed_object;

    //See if we have entity.type case.
    bool subject_test = qi::phrase_parse(iter_begin, iter_end,
                                         parser::type_subject_g,
                                         boost::spirit::ascii::space);
    if (subject_test && iter_begin == iter_end) {
        m_case = new Cases::EntityTypeCase(unparsed_condition.value_str,
                                           unparsed_condition.comp_operator);
        return;
    } else {
        iter_begin = subject.begin();
    }

    //See if we have entity.attribute case.
    std::string attribute;
    subject_test = qi::phrase_parse(iter_begin, iter_end,
                                    parser::attribute_subject_g,
                                    boost::spirit::ascii::space, attribute);
    if (subject_test && iter_begin == iter_end) {
        m_case = new Cases::EntityAttributeCase(
                attribute, unparsed_condition.value_str,
                unparsed_condition.comp_operator);
    }
}
bool ParsedCondition::isTrue(LocatedEntity* entity)
{
    return m_case->testCase(entity);
}
}

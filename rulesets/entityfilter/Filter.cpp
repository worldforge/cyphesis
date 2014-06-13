#include "Filter.h"
#include "ParserDefinitions.h"
#include "../MemMap.h"
#include "../MemEntity.h"
#include "../common/TypeNode.h"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>
#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/io.hpp>
#include <boost/tuple/tuple.hpp>

using namespace boost;
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
    for (auto& entity_pair : all_entities) {
        pass = true;
        for (auto& condition : m_parsedConditions) {
            if (!condition.isTrue(*entity_pair.second)) {
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
    for (auto& entity_pair : all_entities) {
        for (auto& condition : m_parsedConditions) {
            if (condition.isTrue(*entity_pair.second)) {
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
    for (auto& condition : m_conditions) {
        m_parsedConditions.push_back(ParsedCondition(condition));
    }

}

void Filter::search(MemEntityDict &all_entities, EntityVector &res)
{

}

ParsedCondition::ParsedCondition(const parser::condition &unparsed_condition)
{

    //Determine what kind of subject we have using spirit::qi grammar rules.
    std::string subject = unparsed_condition.attribute;
    std::string::iterator iter_begin = subject.begin();
    std::string::iterator iter_end = subject.end();
    std::string parsed_object;

    //grammar for "entity.type" token
    //This rule will match if we use "entity.type" token
    qi::rule<std::string::iterator, boost::spirit::ascii::space_type> type_subject_g =
            (qi::lit("Entity") | qi::lit("entity")) >> "."
                    >> (qi::lit("type") | qi::lit("Type"));

    //See if we have entity.type case.
    bool subject_test = qi::phrase_parse(iter_begin, iter_end, type_subject_g,
                                         boost::spirit::ascii::space);
    if (subject_test && iter_begin == iter_end) {
        m_case = new Cases::EntityTypeCase(unparsed_condition.value_str,
                                           unparsed_condition.comp_operator);
        return;
    } else {
        iter_begin = subject.begin();
    }

    //See if we have entity.attribute case.

    //grammar for "entity.attribute" token
    //This rule will match if we use "entity.attribute" token
    qi::rule<std::string::iterator, std::string(),
            boost::spirit::ascii::space_type> attribute_subject_g = (qi::lit(
            "Entity") | "entity") >> "."
            >> +(qi::char_ - "=" - "<" - ">" - "!");
    std::string attribute;
    subject_test = qi::phrase_parse(iter_begin, iter_end, attribute_subject_g,
                                    boost::spirit::ascii::space, attribute);
    if (subject_test && iter_begin == iter_end) {
        m_case = new Cases::EntityAttributeCase(
                attribute, unparsed_condition.value_str,
                unparsed_condition.comp_operator);
    } else {
        iter_begin = subject.begin();
    }

    //TODO: Outfit case here.

    //See if we have memory.* case.

    //grammar for "entity.*" token
    //NOTE: there may exist a better way to ignore case sensitivity.
    qi::rule<std::string::iterator, std::string(),
            boost::spirit::ascii::space_type> memory_subject_g = ("m"
            | qi::lit("M")) >> qi::lit("emory") >> "." >> +(qi::char_);
    subject_test = qi::phrase_parse(iter_begin, iter_end, attribute_subject_g,
                                    boost::spirit::ascii::space, attribute);
    if (subject_test && iter_begin == iter_end) {
        //NOTE: This is not yet implemented!
       // m_case = new Cases::MemoryCase(attribute, unparsed_condition.value_str,
       //                                unparsed_condition.comp_operator);
    }

}
bool ParsedCondition::isTrue(LocatedEntity& entity)
{
    return m_case->testCase(entity);
}
}

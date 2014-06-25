#include "Filter.h"
#include "ParserDefinitions.h"
#include "AttributeCases.h"
#include "OutfitCase.h"
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
Filter::Filter(const std::string &what)
{

    parser::query_parser<std::string::const_iterator> grammar;
    auto iter_begin = what.begin();
    auto iter_end = what.end();
//    qi::phrase_parse(iter_begin, iter_end, grammar, boost::spirit::ascii::space,
//                     m_conditions);
    qi::phrase_parse(iter_begin, iter_end, grammar, boost::spirit::ascii::space,
                     m_allConditions);
    std::list<ParsedCondition> parsed_and_block;
    for (auto& and_block : m_allConditions) {
        for (auto& condition : and_block) {
            parsed_and_block.push_back(ParsedCondition(condition));
        }
        m_allParsedConditions.push_back(parsed_and_block);
        parsed_and_block.clear();
    }
    //TODO: Check if parsing is successful
//    for (auto& condition : m_conditions) {
//        m_parsedConditions.push_back(ParsedCondition(condition));
//    }

}

//void Filter::search(std::map<long int, LocatedEntity*> all_entities, EntityVector &res)
//{
//    for (auto entity : all_entities) {
//            for (auto& and_block : m_allParsedConditions) {
//                if (check_and_block(*entity.second, and_block)){
//                    res.push_back(entity.second);
//                    break;
//                }
//            }
//        }
//}
bool Filter::check_and_block(LocatedEntity& entity,
                             std::list<ParsedCondition> conditions)
{
    if (conditions.empty()) {
        return false;
    }
    for (auto& condition : conditions) {
        if (!condition.isTrue(entity)) {
            return false;
        }
    }
    return true;
}

bool Filter::search(LocatedEntity& entity)
{
    for (auto& and_block : m_allParsedConditions) {
        if (check_and_block(entity, and_block)) {
            return true;
            break;
        }
    }
    return false;

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

    //see if we have entity.outfit.*.* case;
//    qi::rule<std::string::iterator,  std::string(), std::string(),
//            boost::spirit::ascii::space_type> outfit_subject_g = (qi::lit(
//            "Entity") | "entity") >> "."
//            >> (qi::lit("outfit") | qi::lit("Outfit")) >> "."
//            >> +(qi::char_ - ".") >> "." >> +(qi::char_ - "=" - "<" - ">" - "!");
    std::string outfit_part;
    std::string outfit_property;
    //TODO: Investigate why declared rule doesn't work
    // Grammar is defined within the function call.
    subject_test = qi::phrase_parse(
            iter_begin,
            iter_end,
            (qi::lit("Entity") | "entity") >> "."
                    >> (qi::lit("outfit") | qi::lit("Outfit")) >> "."
                    >> +(qi::char_ - ".") >> "."
                    >> +(qi::char_ - "=" - "<" - ">" - "!"),
            boost::spirit::ascii::space, outfit_part, outfit_property);
    if (subject_test && iter_begin == iter_end) {
        m_case = new Cases::OutfitCase(outfit_part, outfit_property,
                                       unparsed_condition.value_str,
                                       unparsed_condition.comp_operator);
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

    //grammar for "memory.*" token
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

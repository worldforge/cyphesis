#include "Filter.h"
#include "MemMap.h"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>

#include "MemMap.h"
#include "MemEntity.h"

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
#include "LocatedEntity.h"

using namespace boost;
using namespace boost::algorithm;



//This may need to be moved to .h file
BOOST_FUSION_ADAPT_STRUCT(
    parser::condition,
    (std::string, target)
    (std::string, attribute)
    (std::string, comp_operator)
    (std::string, value_str)
)

namespace parser
{
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;
//Parser
    template <typename Iterator>
    struct query_parser : qi::grammar<Iterator, condition(), ascii::space_type>
    {
        query_parser() : query_parser::base_type(start)
        {
            using qi::int_;
            using qi::lit;
            using qi::double_;
            using qi::lexeme;
            using ascii::char_;

            target_g %= +char_ - ".";
            comp_operator_g %= char_("!=") | char_("<=") | char_(">=") | char_("=") | char_(">") | char_("<");
            logical_operator_g %= char_("&") | char_("|");
            attribute_g %= +char_;
            value_g = +char_ >> -logical_operator_g; // needs revision. Log_operator may be consumed too soon.
            start %=
                target_g >> "." >> attribute_g >> comp_operator_g >> value_g;
        }
//Grammar rules
        qi::rule<Iterator, std::string(), ascii::space_type> target_g;
        qi::rule<Iterator, std::string(), ascii::space_type> attribute_g;
        qi::rule<Iterator, std::string(), ascii::space_type> comp_operator_g;
        qi::rule<Iterator, std::string(), ascii::space_type> logical_operator_g;
        qi::rule<Iterator, std::string(), ascii::space_type> value_g;
        qi::rule<Iterator, condition(), ascii::space_type> start;
    };
}

//This is currently unused
//void Filter::recordCondition(const std::string &token,
//                             const std::string &comp_operator,
//                             int delimiter_index)
//{
//    std::string attribute_name;
//    std::string value;
//    int dot_index = token.find(".");
//    attribute_name = token.substr(dot_index + 1,
//                            delimiter_index - dot_index - 1);
//    trim(attribute_name);
//    value = token.substr(delimiter_index + comp_operator.length());
//    trim(value);
//    conditions.emplace_back(attribute_name, value, comp_operator);
//}


Filter::Filter(const std::string &what)
{
    log(INFO, "test3");
    std::cout << "test";
    char_separator<char> token_sep("&");
    tokenizer<char_separator<char>> tokens(what, token_sep);
    parser::condition parsed_token;
    parser::query_parser<std::string::iterator> grammar;
    //Every valid token has one of delimiters and a dot separating object and its attribute
    for (auto iter : tokens) {
        std::string& token = iter;
        auto token_begin = token.begin();
        auto token_end = token.end();
        phrase_parse(token_begin, token_end, grammar, spirit::ascii::space, parsed_token);
        m_conditions.emplace_back(parsed_token);
    }
}

EntityVector Filter::search(MemEntityDict all_entities){

    EntityVector res;
    //First iteration goes through all entities

    Atlas::Message::Element attribute_message;
    //We just need the entities. Maybe there's a better way to copy them over.

    for (auto& entity_iter:all_entities){
        res.push_back(entity_iter.second);
    }

    //NOTE: This works for "OR" operator, but not "AND"
    //TODO: Make sure an entity doesn't get pushed twice.
    for (auto condition:m_conditions){
        for(auto entity_iter:all_entities){
            //Attempt to find a attribute_name. Determine the type, convert the data and compare
            //comparisons are made here using the correct operator.
            entity_iter.second->getAttr(condition.attribute, attribute_message);
            //TODO: This part can be rewritten in a better way.
            //TODO: Comparison operator has to be considered
            //NOTE: some datatypes are not recognized (map, ptr, list)
            if(entity_iter.second(condition.attribute)){
                res.push_back(entity_iter.second);
                continue;
            }

            if (attribute_message.isFloat()) {
                if (std::stof(condition.value_str) == attribute_message.asFloat()) {
                    res.push_back(entity_iter.second);
                }
                continue;
            }
            if (attribute_message.isInt()){
                if (std::stoi(condition.value_str) == attribute_message.asInt()) {
                    res.push_back(entity_iter.second);
                }
                continue;
            }
            if (attribute_message.isString()){
                if (condition.value_str == attribute_message.asString()) {
                    res.push_back(entity_iter.second);
                }
                continue;
            }
        }
    }
    return res;

}


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
    struct query_parser : qi::grammar<Iterator, std::list<condition>(), ascii::space_type>
    {
        query_parser() : query_parser::base_type(start)
        {
            using qi::int_;
            using qi::lit;
            using qi::double_;
            using qi::lexeme;
            using ascii::char_;

            //Definition of a phrase

            // "+" operator matches its operand 1 or more times. Consecutive repetitions of char_ are
            //collected into a string. char_ matches any character (except a dot in this case)
            // char_ has attribute of char, while +(char_) can have attribute of string (or other container)
            // "%=" operator makes RHS use attribute of LHS (in this case, string)
            target_g %= +(char_ - ".");
            //A list of comparison operators
            //TODO: char_ may need to be changed to string and tested.
            comp_operator_g %= char_("!=") | char_("<=") | char_(">=") | char_("=") | char_(">") | char_("<");
            //attribute_g is a string until comparison operator is found;
            attribute_g %= +(char_ - comp_operator_g);
            //A list of logical operators
            logical_operator_g %= char_("&") | char_("|");
            //Value is a string until logical operator is found
            value_g %= +(char_ - logical_operator_g);
            // ">>" operator stands for "followed by".  a >> b matches if b follows a.
            token_g %=
                    target_g >> "." >> attribute_g >> comp_operator_g >> value_g;
            // a%b matches a list of a separated by occurances of b and has attribute of vector<a>.
            start %= token_g % "&";
        }
        //Rule templates take iterator, attribute signature and skipper parser as arguments
        //Attribute of a target_g is string
        qi::rule<Iterator, std::string(), ascii::space_type> target_g;
        qi::rule<Iterator, std::string(), ascii::space_type> attribute_g;
        qi::rule<Iterator, std::string(), ascii::space_type> comp_operator_g;
        qi::rule<Iterator, std::string(), ascii::space_type> logical_operator_g;
        qi::rule<Iterator, std::string(), ascii::space_type> value_g;
        qi::rule<Iterator, condition(), ascii::space_type> token_g;
        qi::rule<Iterator, std::list<condition>(), ascii::space_type> start;
    };
}

Filter::Filter(const std::string &what)
{
    namespace qi = boost::spirit::qi;
    parser::query_parser<std::string::const_iterator> grammar;
    auto iter_begin = what.begin();
    auto iter_end = what.end();
    qi::phrase_parse(iter_begin, iter_end, grammar, boost::spirit::ascii::space, m_conditions);

}

EntityVector Filter::search(MemEntityDict all_entities, EntityVector &res){


    Atlas::Message::Element attribute_message;

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
            if(!entity_iter.second->hasAttr(condition.attribute)){
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


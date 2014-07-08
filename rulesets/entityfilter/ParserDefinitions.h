#ifndef RULESETS_FILTER_PARSERDEFINITIONS_H_
#define RULESETS_FILTER_PARSERDEFINITIONS_H_

#include "Filter.h"
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/variant/recursive_wrapper.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/io.hpp>

namespace EntityFilter
{
namespace parser
{

struct condition {
        std::string attribute;
        std::string comp_operator;
        std::string value_str;
};
}
}

//This needs to be in global scope
BOOST_FUSION_ADAPT_STRUCT(
        EntityFilter::parser::condition,
        (std::string, attribute) (std::string, comp_operator) (std::string, value_str))

namespace EntityFilter
{
namespace parser
{
namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
//Parser definitions
template<typename Iterator>
struct query_parser : qi::grammar<Iterator, std::list<std::list<condition>>(),
        ascii::space_type> {
        query_parser() :
                query_parser::base_type(phrase)
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
            comp_operator_g %= qi::string("!=") | qi::string("<=")
                    | qi::string(">=") | qi::string("==") | qi::string("!==")
                    | char_("=") | char_(">") | char_("<");
            //attribute_g is a string until comparison operator is found;
            attribute_g %= +(char_ - comp_operator_g);
            //A list of logical operators
            logical_operator_g %= char_("&") | char_("|");
            //Value is a string until logical operator is found
            value_g %= +(char_ - logical_operator_g);
            // ">>" operator stands for "followed by".  a >> b matches if b follows a.
            token_g %= attribute_g >> comp_operator_g >> value_g;
            // a%b matches a list of a separated by occurances of b and has attribute of vector<a>.

            and_block_g %= token_g % (qi::lit("and") | "&");
            or_block_g %= and_block_g % (qi::lit("or") | "|");

            phrase %= or_block_g;
        }
        //Rule templates take iterator, attribute signature and skipper parser as arguments
        //Attribute of a target_g is string
        qi::rule<Iterator, std::string(), ascii::space_type> target_g;
        qi::rule<Iterator, std::string(), ascii::space_type> attribute_g;
        qi::rule<Iterator, std::string(), ascii::space_type> comp_operator_g;
        qi::rule<Iterator, std::string(), ascii::space_type> logical_operator_g;
        qi::rule<Iterator, std::string(), ascii::space_type> value_g;
        qi::rule<Iterator, condition(), ascii::space_type> token_g;
        qi::rule<Iterator, std::list<condition>(), ascii::space_type> and_block_g;
        qi::rule<Iterator, std::list<std::list<condition>>(), ascii::space_type> or_block_g;
        qi::rule<Iterator, std::list<std::list<condition>>(), ascii::space_type> phrase;
};

//struct op_and {};
//struct op_or {};
//
//
//template<typename op_type>
//struct binop;
//typedef boost::variant<condition, boost::recursive_wrapper<binop<op_and>>,
//        boost::recursive_wrapper<binop<op_or>>> expr;
//template<typename op_type>
//struct binop {
//        binop(expr left, expr right) :
//                m_left(left), m_right(right)
//        {
//
//        }
//        expr m_left, m_right;
//};
}
}
/*
namespace EntityFilter{

    namespace parser{
template<typename Iterator>
struct query_parser2: qi::grammar<Iterator, expr(), ascii::space_type>{
        query_parser2() :
                query_parser2::base_type(expr_g)
        {
            using qi::int_;
            using qi::lit;
            using qi::double_;
            using qi::lexeme;
            using ascii::char_;
            using qi::_val;
            using qi::_1;
            using qi::_2;
            using qi::_3;
            using namespace boost::phoenix;

            //Definition of a phrase

            // "+" operator matches its operand 1 or more times. Consecutive repetitions of char_ are
            //collected into a string. char_ matches any character (except a dot in this case)
            // char_ has attribute of char, while +(char_) can have attribute of string (or other container)
            // "%=" operator makes RHS use attribute of LHS (in this case, string)
            target_g %= +(char_ - ".");
            //A list of comparison operators
            comp_operator_g %= qi::string("!=") | qi::string("<=")
                    | qi::string(">=") | qi::string("==") | qi::string("!==")
                    | char_("=") | char_(">") | char_("<");
            //attribute_g is a string until comparison operator is found;
            attribute_g %= +(char_ - comp_operator_g);
            //A list of logical operators
            logical_operator_g %= char_("&") | char_("|");
            //Value is a string until logical operator is found
            value_g %= +(char_ - logical_operator_g);
            // ">>" operator stands for "followed by".  a >> b matches if b follows a.
            parsed_token_g %= attribute_g >> comp_operator_g >> value_g;
            // a%b matches a list of a separated by occurances of b and has attribute of vector<a>.
            expr_g =
                    parsed_token_g [_val = _1] |
                    ("(" >> expr_g >> ")")[_val = _1] |
                    (expr_g >> "&" >> expr_g) [_val = construct<binop<op_and>>(_1, _2)]|
                    (expr_g >> "|" >> expr_g) [_val = construct<binop<op_or>>(_1, _2)];

        }
        qi::rule<Iterator, std::string(), ascii::space_type> target_g;
        qi::rule<Iterator, std::string(), ascii::space_type> attribute_g;
        qi::rule<Iterator, std::string(), ascii::space_type> comp_operator_g;
        qi::rule<Iterator, std::string(), ascii::space_type> logical_operator_g;
        qi::rule<Iterator, std::string(), ascii::space_type> value_g;
        qi::rule<Iterator, condition(), ascii::space_type> token_g;
        qi::rule<Iterator, expr(), ascii::space_type> expr_g;
        qi::rule<Iterator, condition(), ascii::space_type> parsed_token_g;
};

}
}
*/
#endif

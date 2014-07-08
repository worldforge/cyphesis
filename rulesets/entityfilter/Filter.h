#ifndef RULESETS_FILTER_H_
#define RULESETS_FILTER_H_

#include "ParserDefinitions.h"
#include "AttributeCases.h"
#include "AttributeComparers.h"
#include "../MemMap.h"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>


#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/variant/recursive_wrapper.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/io.hpp>
#include <cstdint>
///\brief This class is used to search entities in NPC's memory
///using a query as a filter
///
///A query must consist of tokens separated by && operator
///a valid token indicates an object to search (memory or entity),
///a property that will be used as criteria, an operator that
///will be used for comparison and the value that will be compared
///i.e.: entity.property==val
namespace EntityFilter
{

namespace Comparers
{
class AttributeComparerWrapper;
class StringAttributeComparer;
class NumericAttributeComparer;
class ListAttributeComparer {
};
}

//Classes to handle various cases of subjects.
namespace Cases
{
class AbstractCase;
class EntityAttributeCase;
class EntityTypeCase;
}

///This class is meant to further parse tokens into appropriate structures.
class ParsedCondition {
    public:
        explicit ParsedCondition(const parser::condition &all_conditions);
        bool isTrue(LocatedEntity& entity);
        parser::condition m_condition;
    private:
        Cases::AbstractCase* m_case;
};

namespace parser{

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;

//These structs are used as template parameters to distinguish between different kinds of binary ops
struct op_and {};
struct op_or {};

template<typename op_type>
struct binop;

//This is a node of an expression tree. It can either be another tree (and/or node) or a token(leaf).
//int is used since first member of variant has to have a default constructor to be initialized.
typedef boost::variant<int, ParsedCondition, boost::recursive_wrapper<binop<op_and>>,
        boost::recursive_wrapper<binop<op_or>>> expr;

//binop contains 2 more nodes. Template parameter determines the type of operation.
template<typename op_type>
struct binop {
        explicit binop(expr& left, expr& right) :
                m_left(left), m_right(right)
        {
        }
        expr m_left, m_right;
};

//The main parsing grammar.
template<typename Iterator>
struct query_ast_parser: qi::grammar<Iterator, expr(), ascii::space_type, qi::locals<expr>>{
        query_ast_parser() :
                query_ast_parser::base_type(parenthesised_expr_g)
        {
            using qi::int_;
            using qi::lit;
            using ascii::char_;
            using qi::_val;
            using qi::_1;
            using qi::_2;
            using qi::_a;
            using qi::eps;
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
            attribute_g %= +(char_ - comp_operator_g - "(" - ")");
            //A list of logical operators
            logical_operator_g %= char_("&") | char_("|");
            //Value is a string until logical operator is found
            value_g %= +(char_ - logical_operator_g - ("(") - (")"));
            // ">>" operator stands for "followed by".  a >> b matches if b follows a.
            token_g %= attribute_g >> comp_operator_g >> value_g;

            //This constructs a parsed condition that will be used as a leaf.
            //_val is the value of this rule's attribute
            parsed_token_g = token_g[_val = construct<ParsedCondition>(_1)];

            //This parses tokens within parentheses into a single expression
            expr_g =
                    //Upon matching parsed_token_g, store its value into a local variable.
                    //If it's followed by another expression - construct a binary operation, depending
                    //on the operator ("and" will be matched before "or" to allow precedence)
                    //If not - save it as an expression (eps is a parser that always succeeds)
                    parsed_token_g [_a = _1] >> (("&" >> expr_g[_val = construct<binop<op_and>>(_a, _1)])|
                                                ("|" >> expr_g[_val = construct<binop<op_or>>(_a, _1)])  |
                                                (eps [_val = _a]))                                          |
                    //if everything within parenthesis is fully parsed into an expression - consume the
                    //parentheses and make it a single expression
                    ("(" >> expr_g[_val = _1] >> ")");

            //This does the same as above, except for expressions that resulted from using parentheses
            parenthesised_expr_g =
                    expr_g [_a =_1] >> (("&" >> expr_g[_val = construct<binop<op_and>>(_a, _1)])|
                                       ("|" >> expr_g[_val = construct<binop<op_or>>(_a, _1)])  |
                                       (eps[_val = _a]))                                           |
                                    "(" >> parenthesised_expr_g [_val = _1] >> ")";

        }
        qi::rule<Iterator, std::string(), ascii::space_type> target_g;
        qi::rule<Iterator, std::string(), ascii::space_type> attribute_g;
        qi::rule<Iterator, std::string(), ascii::space_type> comp_operator_g;
        qi::rule<Iterator, std::string(), ascii::space_type> logical_operator_g;
        qi::rule<Iterator, std::string(), ascii::space_type> value_g;
        qi::rule<Iterator, parser::condition(), ascii::space_type> token_g;
        qi::rule<Iterator, expr(), ascii::space_type, qi::locals<expr>> expr_g;
        qi::rule<Iterator, expr(), ascii::space_type, qi::locals<expr>> parenthesised_expr_g;
        qi::rule<Iterator, expr(), ascii::space_type, qi::locals<expr>> parsed_token_g;
};
}

///Main Filtering class
class Filter {
    public:
        ///\brief Initialize a filter with a given query
        ///@param what query to be used for filtering
        Filter(const std::string &what);

        //TODO: Allow for various LocatedEntity containers.
        ///\brief test given entity for a match
        ///@param entity - entity to be tested
        bool match(LocatedEntity& entity);
    private:
        //This stores top node of the AST
        parser::expr m_tree;
};

///\brief An Exception class to be thrown in case of an invalid query.
class InvalidQueryException : public std::exception {
    public:
        virtual const char* what() const throw ()
        {
            return "Attempted creating entity filter with invalid query";
        }
};
}
#endif

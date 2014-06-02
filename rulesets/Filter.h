

#ifndef RULESETS_FILTER_H_
#define RULESETS_FILTER_H_

#include <boost/tuple/tuple.hpp>
#include "MemMap.h"

#include <boost/spirit/include/qi.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/io.hpp>


///\brief This class is used to search entities in NPC's memory
///using a query as a filter
///
///A query must consist of tokens separated by && operator
///a valid token indicates an object to search (memory or entity),
///a property that will be used as criteria, an operator that
///will be used for comparison and the value that will be compared
///i.e.: entity.property==val

namespace parser
{


    struct condition
    {
        std::string target;
        std::string attribute;
        std::string comp_operator;
        std::string value_str;
    };
}

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
//Parser definitions
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
            comp_operator_g %= qi::string("!=") | qi::string("<=") | qi::string(">=") | char_("=") | char_(">") | char_("<");
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

class Filter{
    public:
        ///\brief Initialize a filter with a given query
        ///@param what query to be used for filtering
        ///
        ///Constructor for filter parses the query and splits it into triplets with
        ///property to search for, its value and the operator to use for comparison
        Filter(const std::string &what);

        ///\brief Return a Vector with entities that match the query
        void search(MemEntityDict all_entities, EntityVector &res);

        //\brief Return entities that pass all conditions.
        void attributeSearchAnd(const MemEntityDict &all_entities, EntityVector& res);
        //\brief Return Entities that pass at least one condition
        void attributeSearchOr(const MemEntityDict &all_entities, EntityVector& res);
    private:
        std::list<parser::condition> m_conditions;
        void recordCondition(const std::string &token,
                             const std::string &comp_operator,
                             int delimiter_index);
        bool testAttributeMatch(LocatedEntity *entity,
                            const std::string &attribute_name,
                            const std::string &attribute_value,
                            const std::string &comp_operator);

        template <typename T>
        bool compare(T val1, T val2, std::string comp_operator){
            if (comp_operator== "=") return val1 == val2;
            if (comp_operator== "!=") return val1 != val2;
            if (comp_operator== ">") return val1 > val2;
            if (comp_operator== "<") return val1 < val2;
            if (comp_operator== ">=") return val1 >= val2;
            if (comp_operator== "<=") return val1 <= val2;
        }

};
#endif

#include "Filter.h"
#include "MemMap.h"

#include <list>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/tuple/tuple.hpp>

using namespace boost;
using namespace boost::algorithm;

typedef tuple<std::string, std::string, std::string> ConditionTuple;
typedef std::list<ConditionTuple> ConditionList;

///\brief Initialize a filter with a given query
///@param what query to be used for filtering
///
///Constructor for filter parses the query and splits it into triplets with
///property to search for, its value and the operator to use for comparison
Filter::Filter(std::string &what)
{

    char_separator<char> token_sep("&");
    char_separator<char> condition_sep("=");
    tokenizer<char_separator<char>> tokens(what, token_sep);
    ConditionList conditions;

    std::string property;
    std::string value;
    std::string comp_operator;

    //Every valid token has one of delimiters and a dot separating object and its attribute
    int delimiter_index;
    int dot_index;

    //FIXME: The duplicate code can be written in a function
    for (tokenizer<char_separator<char> >::iterator iter = tokens.begin();
            iter != tokens.end(); ++iter) {
        std::string token = std::string(*iter);
        delimiter_index = token.find("==");
        dot_index = token.find('.');
        if (delimiter_index != std::string::npos) {
            property = token.substr(token.find(".") + 1,
                                    delimiter_index - dot_index - 1);
            value = token.substr(delimiter_index + 2);
            comp_operator = token.substr(delimiter_index, 2);
            conditions.push_back(
                    ConditionTuple(property, value, comp_operator));
            continue;
        }
        delimiter_index = token.find("=>");
        if (delimiter_index != std::string::npos) {
            property = token.substr(token.find(".") + 1,
                                    delimiter_index - dot_index - 1);
            value = token.substr(delimiter_index + 2);
            comp_operator = token.substr(delimiter_index, 2);
            conditions.push_back(
                    ConditionTuple(property, value, comp_operator));
            continue;
        }
        delimiter_index = token.find("=<");
        if (delimiter_index != std::string::npos) {
            property = token.substr(token.find(".") + 1,
                                    delimiter_index - dot_index - 1);
            value = token.substr(delimiter_index + 2);
            comp_operator = token.substr(delimiter_index, 2);
            conditions.push_back(
                    ConditionTuple(property, value, comp_operator));
            continue;
        }
        delimiter_index = token.find("!=");
        if (delimiter_index != std::string::npos) {
            property = token.substr(token.find(".") + 1,
                                    delimiter_index - dot_index - 1);
            value = token.substr(delimiter_index + 2);
            comp_operator = token.substr(delimiter_index, 2);
            conditions.push_back(
                    ConditionTuple(property, value, comp_operator));
            continue;
        }
        delimiter_index = token.find(">");
        if (delimiter_index != std::string::npos) {
            property = token.substr(token.find(".") + 1,
                                    delimiter_index - dot_index - 1);
            value = token.substr(delimiter_index + 1);
            comp_operator = token.substr(delimiter_index, 1);
            conditions.push_back(
                    ConditionTuple(property, value, comp_operator));
            continue;
        }
        delimiter_index = token.find("<");
        if (delimiter_index != std::string::npos) {
            property = token.substr(token.find(".") + 1,
                                    delimiter_index - dot_index - 1);
            value = token.substr(delimiter_index + 1);
            comp_operator = token.substr(delimiter_index, 1);
            conditions.push_back(
                    ConditionTuple(property, value, comp_operator));
            continue;
        }
    }
}


#include "Filter.h"
#include "MemMap.h"

#include <list>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/algorithm/string.hpp>

using namespace boost;
using namespace boost::algorithm;

void Filter::recordCondition(const std::string &token,
                             const std::string &comp_operator,
                             int delimiter_index)
{
    std::string property;
    std::string value;
    int dot_index;
    property = token.substr(token.find(".") + 1,
                            delimiter_index - dot_index - 1);
    trim(property);
    value = token.substr(delimiter_index + comp_operator.length());
    trim(value);
    comp_operator = token.substr(delimiter_index, 2);
    trim(comp_operator);
    conditions.emplace_back(property, value, comp_operator);
}

Filter::Filter(const std::string &what, const EntityDict &entities):all_entities(entities)
{
    char_separator<char> token_sep("&");
    tokenizer<char_separator<char>> tokens(what, token_sep);

    std::string property;
    std::string value;
    std::string comp_operator;

    //Every valid token has one of delimiters and a dot separating object and its attribute
    int delimiter_index;
    int dot_index;

    for (auto iter:tokens) {
        const std::string& token = *iter;


        dot_index = token.find('.');

        delimiter_index = token.find("=>");
        if (delimiter_index != std::string::npos) {
            recordCondition(token, "=>", delimiter_index);
            continue;
        }

        delimiter_index = token.find("=<");
        if (delimiter_index != std::string::npos) {
            recordCondition(token, "=>", delimiter_index);
            continue;
        }

        delimiter_index = token.find("!=");
        if (delimiter_index != std::string::npos) {
            recordCondition(token, "!=", delimiter_index);
            continue;
        }

        delimiter_index = token.find("=");
        if (delimiter_index != std::string::npos) {
            recordCondition(token, "=", delimiter_index);
            continue;
        }

        delimiter_index = token.find(">");
        if (delimiter_index != std::string::npos) {
            recordCondition(token, ">", delimiter_index);
            continue;
        }

        delimiter_index = token.find("<");
        if (delimiter_index != std::string::npos) {
            recordCondition(token, "<", delimiter_index);
            continue;
        }
    }
}

EntityVector Filter::Search(){

    //FIXME: This can probably be rewritten using erase method when condition doesn't match
    //so that only one EntityVector is needed
    EntityVector primary_filter, secondary_filter;

    //First iteration goes through all entities
    primary_filter = all_entities;

    //This may not work with operators other than &&, but should still be useful in future.
    for (auto& condition:conditions){

        for(auto& entity_iter:primary_filter){
            //Attempt to find a property. Determine the type, convert the data and compare
            //comparisons are made here using the correct operator.
            //If an entity matches condition, it is emplaced in secondary filter
        }
        //TODO: Make sure the correct list is returned in the end.

        //next iteration goes through a smaller list
        primary_filter = secondary_filter;
        secondary_filter.clear();
    }

    return primary_filter;

}


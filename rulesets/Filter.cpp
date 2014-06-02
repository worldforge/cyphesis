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


bool Filter::testAttributeMatch(LocatedEntity *entity,
                            const std::string &attribute_name,
                            const std::string &attribute_value,
                            const std::string &comp_operator){
//
    if (!entity->hasAttr(attribute_name)) {return false;}
    Atlas::Message::Element attribute_element;
    if (entity->getAttr(attribute_name, attribute_element) != 0){return false;}

    if (attribute_element.isString()){
        return compare(attribute_element.asString(), attribute_value, comp_operator);
    }
    if (attribute_element.isFloat()){
        return compare(float(attribute_element.asFloat()), std::stof(attribute_value), comp_operator);
    }
    if (attribute_element.isInt()){
        return compare(int(attribute_element.asInt()), std::stoi(attribute_value), comp_operator);
    }
    //TODO: Implement support for list
    return false; //attribute is not int/float/str/list
}

void Filter::attributeSearchAnd(const MemEntityDict &all_entities, EntityVector& res){
    if (m_conditions.empty()){return;}
    bool pass;
    for (auto entity_pair:all_entities){
        pass = true;
        for (auto condition:m_conditions){
            if (!testAttributeMatch(entity_pair.second, condition.attribute, condition.value_str, condition.comp_operator)){
                pass = false;
                break;
            }
        }
        if (pass){res.push_back(entity_pair.second);}
    }
}

void Filter::attributeSearchOr(const MemEntityDict &all_entities, EntityVector& res){
    if (m_conditions.empty()){return;}
    for (auto entity_pair:all_entities){
        for (auto condition:m_conditions){
            if (testAttributeMatch(entity_pair.second, condition.attribute, condition.value_str, condition.comp_operator)){
                res.push_back(entity_pair.second);
                break;
            }
        }
    }
}

Filter::Filter(const std::string &what)
{
    namespace qi = boost::spirit::qi;
    parser::query_parser<std::string::const_iterator> grammar;
    auto iter_begin = what.begin();
    auto iter_end = what.end();
    qi::phrase_parse(iter_begin, iter_end, grammar, boost::spirit::ascii::space, m_conditions);

}

void Filter::search(MemEntityDict all_entities, EntityVector &res){

}


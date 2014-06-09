/*
 * AttributeCases.cpp
 *
 *  Created on: Jun 8, 2014
 *      Author: ytaben
 */


#include "AttributeCases.h"

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>

#include "../MemMap.h"
#include "../MemEntity.h"
#include "../common/TypeNode.h"
namespace qi = boost::spirit::qi;

namespace EntityFilter{
namespace Cases{

EntityTypeCase::EntityTypeCase(const std::string &value,
                               const std::string &comp_operator) :
        m_valueStr(value)
{
    if (comp_operator == "=") {
        m_comparer_method = &Comparers::eqComparer;
    } else {
        m_comparer_method = &Comparers::neqComparer;
    }
}
bool EntityTypeCase::testCase(LocatedEntity* entity)
{
    return m_comparer_method(entity->getType()->name(), m_valueStr);
}

EntityAttributeCase::EntityAttributeCase(const std::string &attribute,
                                         const std::string &value,
                                         const std::string &comp_operator) :
        m_attributeName(attribute), m_valueStr(value)
{
//Determine type of value
    //TODO: Implement support for list
    auto iter_begin = value.begin();
    auto iter_end = value.end();
    float value_float;
    bool value_check = qi::phrase_parse(iter_begin, iter_end, qi::float_,
                                        boost::spirit::ascii::space,
                                        value_float);
    if (value_check && iter_begin == iter_end) {
        m_comparer = new Comparers::NumericAttributeComparer(attribute, value, comp_operator);
        return;
    } else {
        iter_begin = value.begin();
    }

    std::string value_str;
    value_check = qi::phrase_parse(iter_begin, iter_end, +qi::char_, boost::spirit::ascii::space, value_str);
    if (value_check && iter_begin == iter_end){
        m_comparer = new Comparers::StringAttributeComparer(attribute, value, comp_operator);
    }

    //TODO: Implement support for list;
}
bool EntityAttributeCase::testCase(LocatedEntity* entity){
    return m_comparer->compare(entity);
}
}
}


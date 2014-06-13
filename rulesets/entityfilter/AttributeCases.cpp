#include "AttributeCases.h"
#include "../LocatedEntity.h"
#include "../MemMap.h"
#include "../common/TypeNode.h"

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>

namespace qi = boost::spirit::qi;

namespace EntityFilter
{
namespace Cases
{
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
        m_comparer = new Comparers::NumericAttributeComparer(attribute, value,
                                                             comp_operator);
        return;
    } else {
        iter_begin = value.begin();
    }

    std::string value_str;
    value_check = qi::phrase_parse(iter_begin, iter_end, +qi::char_,
                                   boost::spirit::ascii::space, value_str);
    if (value_check && iter_begin == iter_end) {
        m_comparer = new Comparers::StringAttributeComparer(attribute, value,
                                                            comp_operator);
    }

    //TODO: Implement support for list;
}
EntityAttributeCase::~EntityAttributeCase()
{
    delete m_comparer;
}

bool EntityAttributeCase::testCase(LocatedEntity& entity)
{
    return m_comparer->compare(entity);
}

EntityTypeCase::EntityTypeCase(const std::string &value,
                               const std::string &comp_operator) :
        m_valueStr(value)
{
    if (comp_operator == "=") {
        m_comparer = new Comparers::SoftTypeComparer(value, comp_operator);
    } else {
        m_comparer = new Comparers::StrictTypeComparer(value, comp_operator);
    }
}

EntityTypeCase::~EntityTypeCase()
{
    delete m_comparer;
}

bool EntityTypeCase::testCase(LocatedEntity& entity)
{
    return m_comparer->compare(entity);
}

}
}


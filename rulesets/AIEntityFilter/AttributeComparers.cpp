
#include "AttributeComparers.h"
#include "../MemMap.h"
#include "../MemEntity.h"
#include "../common/TypeNode.h"

namespace EntityFilter{
namespace Comparers{
StringAttributeComparer::StringAttributeComparer(const std::string &attribute_name,
                                                 const std::string &value_str,
                                                 const std::string &comp_operator) :
        m_attributeName(attribute_name), m_value(value_str)
{
    if(comp_operator == "="){
        comparer_method = &eqComparer;
    }
    else{
        comparer_method = &neqComparer;
    }
}

bool StringAttributeComparer::compare(LocatedEntity* entity)
{
    if (entity->getAttr(m_attributeName, m_atlasAttr) == 0) {
        return comparer_method(m_atlasAttr.asString(), m_value);
    } else {
        return false;
    }
}

NumericAttributeComparer::NumericAttributeComparer(const std::string &attribute_name,
                                                   const std::string &value,
                                                   const std::string &comp_operator) :
        m_attributeName(attribute_name)
{
    m_value = std::stof(value);
    if (comp_operator == "=") {
        comparer_method = &eqComparer;
    } else if (comp_operator == "!=") {
        comparer_method = &neqComparer;
    } else if (comp_operator == ">") {
        comparer_method = &gComparer;
    } else if (comp_operator == ">=") {
        comparer_method = &geComparer;
    } else if (comp_operator == "<") {
        comparer_method = &lComparer;
    } else {
        comparer_method = &leComparer;
    }

}
bool NumericAttributeComparer::compare(LocatedEntity* entity)
{
    if (entity->getAttr(m_attributeName, m_atlasAttr) == 0) {
        return comparer_method(m_atlasAttr.asFloat(), m_value);
    } else {
        return false;
    }

}

}
}

#include "AttributeComparers.h"
#include "../LocatedEntity.h"
#include "../common/TypeNode.h"

namespace EntityFilter
{
namespace Comparers
{
StringAttributeComparer::StringAttributeComparer(const std::string &attribute_name,
                                                 const std::string &value_str,
                                                 const std::string &comp_operator) :
        m_attributeName(attribute_name), m_value(value_str)
{
    if (comp_operator == "=") {
        comparer_method = &eqComparer;
    } else {
        comparer_method = &neqComparer;
    }
}

bool StringAttributeComparer::compare(LocatedEntity& entity)
{
    Atlas::Message::Element atlas_attr;
    if (entity.getAttr(m_attributeName, atlas_attr) == 0) {
        return comparer_method(atlas_attr.asString(), m_value);
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

bool NumericAttributeComparer::compare(LocatedEntity& entity)
{
    if (entity.getAttr(m_attributeName, m_atlasAttr) == 0) {
        return comparer_method(m_atlasAttr.asNum(), m_value);
    } else {
        return false;
    }

}

StrictTypeComparer::StrictTypeComparer(const std::string &type,
                                       const std::string &comp_operator) :
        m_typeName(type)
{
    if (comp_operator == "==") {
        comparer_method = &eqComparer;
    } else {
        comparer_method = &neqComparer;
    }
}

bool StrictTypeComparer::compare(LocatedEntity& entity)
{
    return comparer_method(m_typeName, entity.getType()->name());
}

SoftTypeComparer::SoftTypeComparer(const std::string &type,
                                   const std::string &comp_operator) :
        m_typeStr(type)
{
    comp_bool = comp_operator == "=";
}

bool SoftTypeComparer::compare(LocatedEntity& entity)
{
    //This is compact, but may or may not be optimal.
    return entity.getType()->isTypeOf(m_typeStr) == comp_bool;
}

}
}

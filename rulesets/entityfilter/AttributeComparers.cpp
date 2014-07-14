#include "AttributeComparers.h"
#include "../LocatedEntity.h"
#include "../common/TypeNode.h"

//FIXME: There are tons of duplicate code in this file that can be reduced
namespace EntityFilter
{
namespace Comparers
{
StringAttributeComparer::StringAttributeComparer(const std::string &attribute_name,
                                                 const std::string &value_str,
                                                 const std::string &comp_operator) :
         m_value(value_str), m_attributeName(attribute_name)
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
    Atlas::Message::Element (atlas_attr);
    if (entity.getAttr(m_attributeName, atlas_attr) == 0) {
        return comparer_method(atlas_attr.asNum(), m_value);
    } else {
        return false;
    }

}

NumericListAttributeComparer::NumericListAttributeComparer(const std::string &attribute_name,
                                                           const std::list<float> &value,
                                                           const std::string &comp_operator) :
        m_attributeName(attribute_name), m_value(value)
{
    //This code is shared with NumericAttribute comparer's constructor. It could be
    //made a function
    if (comp_operator == "=") {
        list_comparer_method = &NumericListAttributeComparer::inComparer;
        float_comparer_method = &eqComparer;
        return;
    } else if (comp_operator == "!=") {
        list_comparer_method = &NumericListAttributeComparer::inComparer;
        float_comparer_method = &neqComparer;
        return;
    } else if (comp_operator == "==") {
        float_comparer_method = &eqComparer;
    } else if (comp_operator == "!==") {
        float_comparer_method = &neqComparer;
    } else if (comp_operator == ">") {
        float_comparer_method = &gComparer;
    } else if (comp_operator == ">=") {
        float_comparer_method = &geComparer;
    } else if (comp_operator == "<") {
        float_comparer_method = &lComparer;
    } else {
        float_comparer_method = &leComparer;
    }
    list_comparer_method = &NumericListAttributeComparer::allComparer;
}

bool NumericListAttributeComparer::inComparer(Atlas::Message::Element& attribute){
    auto value_iter = m_value.begin();
    auto value_end = m_value.end();
    auto attr_iter = attribute.List().begin();
    auto attr_end = attribute.List().end();
    while (value_iter != value_end){
        for (; attr_iter != attr_end; ++attr_iter){
        if(float_comparer_method(*value_iter, attr_iter->asNum())){
            return true;
        }
        }
        ++value_iter;
    }
    return false;
}
bool NumericListAttributeComparer::allComparer(Atlas::Message::Element& attribute){
    auto value_iter = m_value.begin();
    auto value_end = m_value.end();
    auto attr_iter = attribute.List().begin();
    if (attribute.List().size() != m_value.size()) {
                   return false;
               }
    while (value_iter != value_end){
        if(!float_comparer_method(*value_iter, attr_iter->asNum())){
            return false;
        }
        ++value_iter;
        ++attr_iter;
    }
    return true;
}

bool NumericListAttributeComparer::compare(LocatedEntity& entity)
{
    Atlas::Message::Element atlas_attr;
    if (entity.getAttr(m_attributeName, atlas_attr) != 0) {
        return false;
    }
    return (this->*list_comparer_method)(atlas_attr);
}

StringListAttributeComparer::StringListAttributeComparer(const std::string& attribute_name,
                                                         const std::list<std::string>& value_list,
                                                         const std::string& comp_operator) :
        m_attributeName(attribute_name), m_value(value_list)
{
    if (comp_operator == "=") {
        list_comparer_method = &StringListAttributeComparer::inComparer;
        string_comparer_method = &eqComparer;
        return;
    } else if (comp_operator == "!=") {
        list_comparer_method = &StringListAttributeComparer::inComparer;
        string_comparer_method = &neqComparer;
        return;
    } else if (comp_operator == "==") {
        string_comparer_method = &eqComparer;
    } else if (comp_operator == "!==") {
        string_comparer_method = &neqComparer;
    } else if (comp_operator == ">") {
        string_comparer_method = &gComparer;
    } else if (comp_operator == ">=") {
        string_comparer_method = &geComparer;
    } else if (comp_operator == "<") {
        string_comparer_method = &lComparer;
    } else {
        string_comparer_method = &leComparer;
    }
    list_comparer_method = &StringListAttributeComparer::allComparer;
}

bool StringListAttributeComparer::inComparer(Atlas::Message::Element& attribute){
    auto value_iter = m_value.begin();
    auto value_end = m_value.end();
    auto attr_iter = attribute.List().begin();
    auto attr_end = attribute.List().end();
    while (value_iter != value_end){
        for (; attr_iter != attr_end; ++attr_iter){
        if(string_comparer_method(*value_iter, attr_iter->String())){
            return true;
        }
        }
        ++value_iter;
    }
    return false;
}

bool StringListAttributeComparer::allComparer(Atlas::Message::Element& attribute){
    auto value_iter = m_value.begin();
    auto value_end = m_value.end();
    auto attr_iter = attribute.List().begin();
    if (attribute.List().size() != m_value.size()) {
               return false;
           }
    while (value_iter != value_end){
        if(!string_comparer_method(*value_iter, attr_iter->String())){
            return false;
        }
        ++value_iter;
        ++attr_iter;
    }
    return true;
}
bool StringListAttributeComparer::compare(LocatedEntity& entity){
    Atlas::Message::Element atlas_attr;
        if (entity.getAttr(m_attributeName, atlas_attr) != 0) {
            return false;
        }

        return (this->*list_comparer_method)(atlas_attr);
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

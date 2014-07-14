#ifndef RULESETS_FILTER_ATTRIBUTECOMPARERS_H_
#define RULESETS_FILTER_ATTRIBUTECOMPARERS_H_

#include "AttributeComparerWrapper.h"
#include "../MemMap.h"
#include "../common/TypeNode.h"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>

namespace EntityFilter
{
namespace Comparers
{

///String comparer class. Only = or != operators are allowed.
class StringAttributeComparer : public AttributeComparerWrapper {
    public:
        StringAttributeComparer(const std::string &attribute_name,
                                const std::string &value_str,
                                const std::string &comp_operator);
        ~StringAttributeComparer()
        {
        }
        bool compare(LocatedEntity& entity);
    private:
        bool (*comparer_method)(std::string, std::string);
        std::string m_value;
        std::string m_attributeName;
};

///A class used to compare numeric attributes
class NumericAttributeComparer : public AttributeComparerWrapper {
    public:
        NumericAttributeComparer(const std::string &attribute_name,
                                 const std::string &value_str,
                                 const std::string &comp_operator);
        ~NumericAttributeComparer()
        {
        }
        bool compare(LocatedEntity& entity);
    private:
        //a pointer to the comparer method with the right comparison operator
        bool (*comparer_method)(float, float);
        std::string m_attributeName;
        float m_value;
};

///\brief class used to compare lists of numbers
///
///Using "=" or "!=" will match if any value from the input list is found in the property's list.
///Using "==" or "!==" or any other operators will apply these operators between input list
///and property's list in a manner of parallel lists
class NumericListAttributeComparer : public AttributeComparerWrapper {
    public:
        NumericListAttributeComparer(const std::string &attribute_name,
                                     const std::list<float> &value,
                                     const std::string &comp_operator);
        ~NumericListAttributeComparer()
        {
        }
        bool compare(LocatedEntity& entity);
    private:
        bool (*float_comparer_method)(float, float);
        bool (NumericListAttributeComparer::*list_comparer_method)(Atlas::Message::Element&);
        bool inComparer(Atlas::Message::Element& attribute);
        bool allComparer(Atlas::Message::Element& attribute);
        std::string m_attributeName;
        std::list<float> m_value;
};

///\brief class used to compare lists of strings
///
///Operators work in the same way as in NumericListAttributeComparer
class StringListAttributeComparer : public AttributeComparerWrapper {
    public:
        StringListAttributeComparer(const std::string &attribute_name,
                                    const std::list<std::string> &value_str,
                                    const std::string &comp_operator);
        ~StringListAttributeComparer()
        {
        }
        ;
        bool compare(LocatedEntity& entity);
    private:
        bool (*string_comparer_method)(std::string, std::string);
        bool (StringListAttributeComparer::*list_comparer_method)(Atlas::Message::Element&);
        bool inComparer(Atlas::Message::Element& attribute);
        bool allComparer(Atlas::Message::Element& attribute);
        std::string m_attributeName;
        std::list<std::string> m_value;
};

///\brief Compares only to the type of a given TypeNode
//TODO: compare type nodes instead of strings.
class StrictTypeComparer : public AttributeComparerWrapper {
    public:
        StrictTypeComparer(const std::string &type,
                           const std::string &comp_operator);
        ~StrictTypeComparer()
        {
        }
        bool compare(LocatedEntity& entity);
    private:
        std::string m_typeName;
        bool (*comparer_method)(std::string, std::string);
};
///\brief Compares to the type of a given TypeNode and its parent nodes
class SoftTypeComparer : public AttributeComparerWrapper {
    public:
        SoftTypeComparer(const std::string &type,
                         const std::string &comp_operator);
        ~SoftTypeComparer()
        {
        }
        bool compare(LocatedEntity& entity);
    private:
        std::string m_typeStr;
        bool comp_bool;

};

}
}

#endif

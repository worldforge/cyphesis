//Add custom implementations of stubbed functions here; this file won't be rewritten when re-generating stubs.

#include "common/Property_impl.h"

//The methods below are defined in Property_impl.h
#define STUB_Property_Property
#define STUB_Property_get
#define STUB_Property_add
#define STUB_Property_copy


#define STUB_PropertyCore_install
#define STUB_PropertyCore_remove
#define STUB_PropertyCore_PropertyCore
#define STUB_PropertyCore_apply
#define STUB_PropertyCore_add
#define STUB_PropertyCore_operation
#define STUB_PropertyCore_copy
#define STUB_PropertyCore_operator_EQUALS


#ifndef STUB_PropertyUtil_parsePropertyModification
#define STUB_PropertyUtil_parsePropertyModification
std::pair<ModifierType, std::string> PropertyUtil::parsePropertyModification(const std::string& propertyName)
{
    return std::make_pair(ModifierType::Default, propertyName);
}
#endif //STUB_PropertyUtil_parsePropertyModification



#ifndef STUB_PropertyUtil_isValidName
#define STUB_PropertyUtil_isValidName
bool PropertyUtil::isValidName(const std::string& name)
{
    return true;
}
#endif //STUB_PropertyUtil_isValidName


#ifndef STUB_Property_set
#define STUB_Property_set
template<>
void Property<int>::set(const Atlas::Message::Element & e)
{
    if (e.isInt()) {
       this->m_data = e.asInt();
    }
}

template<>
void Property<float>::set(const Atlas::Message::Element & e)
{
    if (e.isNum()) {
        this->m_data = e.asNum();
    }
}

template<>
void Property<double>::set(const Atlas::Message::Element & e)
{
    if (e.isNum()) {
        this->m_data = e.asNum();
    }
}

template<>
void Property<std::string>::set(const Atlas::Message::Element & e)
{
    if (e.isString()) {
        this->m_data = e.String();
    }
}

template<>
void Property<Atlas::Message::MapType>::set(const Atlas::Message::Element & e)
{
    if (e.isMap()) {
        this->m_data = e.Map();
    }
}

template<>
void Property<Atlas::Message::ListType>::set(const Atlas::Message::Element & e)
{
    if (e.isList()) {
        this->m_data = e.List();
    }
}
#endif //STUB_Property_set


#ifndef STUB_SoftProperty_SoftProperty
#define STUB_SoftProperty_SoftProperty
SoftProperty::SoftProperty(Atlas::Message::Element data)
    : PropertyBase(0), m_data(data)
{

}

#endif

#ifndef STUB_BoolProperty_get
#define STUB_BoolProperty_get
int BoolProperty::get(Atlas::Message::Element & val) const
{
    val = hasFlags(prop_flag_bool) ? 1 : 0;
    return 0;
}
#endif //STUB_BoolProperty_get

#ifndef STUB_BoolProperty_set
#define STUB_BoolProperty_set
void BoolProperty::set(const Atlas::Message::Element & val)
{
    if (val.isInt()) {
        if (val.Int() == 0) {
            removeFlags(prop_flag_bool);
        } else {
            addFlags(prop_flag_bool);
        }
    }
}
#endif //STUB_BoolProperty_set



#ifndef STUB_BoolProperty_isTrue
#define STUB_BoolProperty_isTrue
bool BoolProperty::isTrue() const
{
    return hasFlags(prop_flag_bool);
}
#endif //STUB_BoolProperty_isTrue

template
class PropertyCore<LocatedEntity>;
template class Property<int>;
template class Property<double>;
template class Property<float>;
template class Property<std::string>;
template class Property<Atlas::Message::MapType>;
template class Property<Atlas::Message::ListType>;
//template class Property<std::vector<std::string>>;

template <> const std::string Property<int>::property_atlastype = "int";
template <> const std::string Property<long>::property_atlastype = "int";
template <> const std::string Property<float>::property_atlastype = "float";
template <> const std::string Property<double>::property_atlastype = "float";
template <> const std::string Property<std::string>::property_atlastype = "string";
template <> const std::string Property<Atlas::Message::ListType>::property_atlastype = "list";
template <> const std::string Property<Atlas::Message::MapType>::property_atlastype = "map";

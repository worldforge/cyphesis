//Add custom implementations of stubbed functions here; this file won't be rewritten when re-generating stubs.

#include "common/Property_impl.h"

//The methods below are defined in Property_impl.h
#define STUB_Property_Property
#define STUB_Property_get
#define STUB_Property_add
#define STUB_Property_copy


#ifndef STUB_PropertyBase_PropertyBase
#define STUB_PropertyBase_PropertyBase
PropertyBase::PropertyBase(unsigned int flags )
    : m_flags(flags)
{

}
#endif //STUB_PropertyBase_PropertyBase

#ifndef STUB_PropertyBase_add
#define STUB_PropertyBase_add
void PropertyBase::add(const std::string & s,
                       Atlas::Message::MapType & ent) const
{
    get(ent[s]);
}

void PropertyBase::add(const std::string & s,
                       const Atlas::Objects::Entity::RootEntity & ent) const
{
}

#endif //STUB_PropertyBase_add

#ifndef STUB_PropertyBase_operation
#define STUB_PropertyBase_operation
HandlerResult PropertyBase::operation(LocatedEntity *,
                                      const Operation &,
                                      OpVector &)
{
   return OPERATION_IGNORED;
}
#endif //STUB_PropertyBase_add

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
    val = hasFlags(flag_bool) ? 1 : 0;
    return 0;
}
#endif //STUB_BoolProperty_get

#ifndef STUB_BoolProperty_set
#define STUB_BoolProperty_set
void BoolProperty::set(const Atlas::Message::Element & val)
{
    if (val.isInt()) {
        if (val.Int() == 0) {
            removeFlags(flag_bool);
        } else {
            addFlags(flag_bool);
        }
    }
}
#endif //STUB_BoolProperty_set



#ifndef STUB_BoolProperty_isTrue
#define STUB_BoolProperty_isTrue
bool BoolProperty::isTrue() const
{
    return hasFlags(flag_bool);
}
#endif //STUB_BoolProperty_isTrue

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

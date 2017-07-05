//Add custom implementations of stubbed functions here; this file won't be rewritten when re-generating stubs.

#include "common/Property_impl.h"

//The methods below are defined in Property_impl.h
#define STUB_Property_Property
#define STUB_Property_get
#define STUB_Property_add
#define STUB_Property_copy

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
#endif //STUB_Property_set


#ifndef STUB_SoftProperty_SoftProperty
#define STUB_SoftProperty_SoftProperty
SoftProperty::SoftProperty(const Atlas::Message::Element & data)
    : PropertyBase(0), m_data(data)
{

}

SoftProperty::SoftProperty()
    : PropertyBase()
{

}
#endif

template class Property<int>;
template class Property<double>;
template class Property<std::string>;
template class Property<Atlas::Message::MapType>;

#ifndef TESTS_STUBS_RULESETS_STUBPROPELPROPERTY_H_
#define TESTS_STUBS_RULESETS_STUBPROPELPROPERTY_H_


#include "rulesets/PropelProperty.h"

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Objects::Entity::RootEntity;


const std::string PropelProperty::property_name = "propel";
const std::string PropelProperty::property_atlastype = "list";

PropelProperty::PropelProperty() {
}

PropelProperty::~PropelProperty()
{
}

int PropelProperty::get(Element & val) const
{
    return -1;
}

void PropelProperty::set(const Element & val)
{
}

void PropelProperty::add(const std::string & key,
                       MapType & map) const
{
}

void PropelProperty::add(const std::string & key,
                       const RootEntity & ent) const
{
}

PropelProperty * PropelProperty::copy() const {
    return new PropelProperty(*this);
}

#endif /* TESTS_STUBS_RULESETS_STUBPROPELPROPERTY_H_ */

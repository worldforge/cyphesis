//Add custom implementations of stubbed functions here; this file won't be rewritten when re-generating stubs.
#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/SmartPtr.h>
#include "rules/Script.h"

#ifndef STUB_LocatedEntity_LocatedEntity
#define STUB_LocatedEntity_LocatedEntity

LocatedEntity::LocatedEntity(RouterId id)
    : Router(id)
    , m_seq(0)
    , m_type(nullptr), m_flags(0), m_parent(nullptr), m_contains(nullptr)
{

}
#endif //STUB_LocatedEntity_LocatedEntity

#ifndef STUB_LocatedEntity_setProperty
#define STUB_LocatedEntity_setProperty
PropertyBase* LocatedEntity::setProperty(const std::string& name, std::unique_ptr<PropertyBase> prop)
{
    auto p = prop.get();
    m_properties[name].property = std::move(prop);
    return p;
}
#endif

#ifndef STUB_LocatedEntity_getProperty
#define STUB_LocatedEntity_getProperty
const PropertyBase* LocatedEntity::getProperty(const std::string& name) const
{
    auto I = m_properties.find(name);
    if (I != m_properties.end()) {
        return I->second.property.get();
    }
    return nullptr;
}
#endif //STUB_LocatedEntity_getProperty

#ifndef STUB_LocatedEntity_setType
#define STUB_LocatedEntity_setType

void LocatedEntity::setType(const TypeNode* t)
{
    m_type = t;
}

#endif

#ifndef STUB_LocatedEntity_makeContainer
#define STUB_LocatedEntity_makeContainer

void LocatedEntity::makeContainer()
{
    if (m_contains == 0) {
        m_contains.reset(new LocatedEntitySet);
    }
}

#endif

#ifndef STUB_LocatedEntity_getAttr
#define STUB_LocatedEntity_getAttr
int LocatedEntity::getAttr(const std::string & name, Atlas::Message::Element &) const
{
    return -1;
}
#endif //STUB_LocatedEntity_getAttr

#ifndef STUB_LocatedEntity_getAttrType
#define STUB_LocatedEntity_getAttrType
int LocatedEntity::getAttrType(const std::string & name, Atlas::Message::Element &, int type) const
{
    return -1;
}
#endif //STUB_LocatedEntity_getAttrType


std::ostream& operator<<(std::ostream& s, const LocatedEntity& d)
{
    return s;
}

#ifndef STUB_LocatedEntity_createProperty
#define STUB_LocatedEntity_createProperty
std::unique_ptr<PropertyBase> LocatedEntity::createProperty(const std::string& propertyName) const
{
    return {};
}
#endif //STUB_LocatedEntity_createProperty
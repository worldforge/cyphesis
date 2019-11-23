//Add custom implementations of stubbed functions here; this file won't be rewritten when re-generating stubs.
#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/SmartPtr.h>
#include "rules/Script.h"

#ifndef STUB_LocatedEntity_LocatedEntity
#define STUB_LocatedEntity_LocatedEntity
LocatedEntity::LocatedEntity(const std::string & id, long intId)
    : Router(id, intId)
    , m_seq(0)
    , m_type(nullptr), m_flags(0), m_contains(nullptr)
{

}
#endif //STUB_LocatedEntity_LocatedEntity


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

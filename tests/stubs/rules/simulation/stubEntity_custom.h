//Add custom implementations of stubbed functions here
#include "rules/Domain.h"

#ifndef STUB_Entity_setType
#define STUB_Entity_setType
void Entity::setType(const TypeNode* t)
{
    m_type = t;
}
#endif

#ifndef STUB_Entity_destroy
#define STUB_Entity_destroy
void Entity::destroy()
{
    if (m_parent) {
        m_parent->removeChild(*this);
    }
    if (m_contains) {
        m_contains->clear();
    }
    destroyed.emit();
}
#endif

#ifndef STUB_Entity_createProperty
#define STUB_Entity_createProperty
std::unique_ptr<PropertyBase> Entity::createProperty(const std::string& propertyName) const
{
    return {};
}
#endif //STUB_Entity_createProperty
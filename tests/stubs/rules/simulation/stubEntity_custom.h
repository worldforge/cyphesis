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
    destroyed.emit();
}
#endif

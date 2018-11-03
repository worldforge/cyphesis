//Add custom implementations of stubbed functions here
#ifndef STUB_Entity_setProperty
#define STUB_Entity_setProperty
PropertyBase* Entity::setProperty(const std::string& name, PropertyBase* prop)
{
    return m_properties[name] = prop;
}

#endif

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

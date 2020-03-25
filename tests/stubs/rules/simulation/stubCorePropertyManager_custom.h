//Add custom implementations of stubbed functions here; this file won't be rewritten when re-generating stubs.

#ifndef STUB_CorePropertyManager_addProperty
#define STUB_CorePropertyManager_addProperty
std::unique_ptr<PropertyBase> CorePropertyManager::addProperty(const std::string & name) const
{
    return {};
}
#endif //STUB_CorePropertyManager_addProperty

#define STUB_CorePropertyManager_installBaseProperty
#define STUB_CorePropertyManager_installProperty

#ifndef STUB_CorePropertyManager_CorePropertyManager
#define STUB_CorePropertyManager_CorePropertyManager
CorePropertyManager::CorePropertyManager(Inheritance& inheritance)
        : m_inheritance(inheritance)
{

}
#endif //STUB_CorePropertyManager_CorePropertyManager

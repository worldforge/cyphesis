//Add custom implementations of stubbed functions here; this file won't be rewritten when re-generating stubs.


#ifndef STUB_CorePropertyManager_installBaseProperty
#define STUB_CorePropertyManager_installBaseProperty
template<typename PropertyT>
PropertyFactory<Property<PropertyT>>* CorePropertyManager::installBaseProperty(const std::string & type_name, const std::string & parent)
{
    return nullptr;
}
#endif //STUB_CorePropertyManager_installBaseProperty

#ifndef STUB_CorePropertyManager_installProperty
#define STUB_CorePropertyManager_installProperty
template<typename PropertyT>
PropertyFactory<PropertyT>* CorePropertyManager::installProperty(const std::string & type_name, const std::string & parent)
{
    return nullptr;
}
#endif //STUB_CorePropertyManager_installProperty

#ifndef STUB_CorePropertyManager_installProperty
#define STUB_CorePropertyManager_installProperty
template<typename PropertyT>
PropertyFactory<PropertyT>* CorePropertyManager::installProperty(const std::string & type_name)
{
    return nullptr;
}
#endif //STUB_CorePropertyManager_installProperty

#ifndef STUB_CorePropertyManager_installProperty
#define STUB_CorePropertyManager_installProperty
template<typename PropertyT>
PropertyFactory<PropertyT>* CorePropertyManager::installProperty()
{
    return nullptr;
}
#endif //STUB_CorePropertyManager_installProperty

#ifndef STUB_CorePropertyManager_addProperty
#define STUB_CorePropertyManager_addProperty
std::unique_ptr<PropertyBase> CorePropertyManager::addProperty(const std::string & name, int type)
{
    return {};
}
#endif //STUB_CorePropertyManager_addProperty

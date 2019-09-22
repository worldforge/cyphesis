//Add custom implementations of stubbed functions here; this file won't be rewritten when re-generating stubs.
#include "common/PropertyFactory.h"
template<> PropertyManager* Singleton<PropertyManager>::ms_Singleton = nullptr;

#ifndef STUB_PropertyManager_addProperty
#define STUB_PropertyManager_addProperty
std::unique_ptr<PropertyBase> PropertyManager::addProperty(const std::string & name, int type)
{
    return {};
}
#endif //STUB_PropertyManager_addProperty

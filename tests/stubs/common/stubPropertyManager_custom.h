//Add custom implementations of stubbed functions here; this file won't be rewritten when re-generating stubs.
PropertyManager * PropertyManager::m_instance = 0;

#ifndef STUB_PropertyManager_PropertyManager
#define STUB_PropertyManager_PropertyManager
PropertyManager::PropertyManager()
{
    assert(m_instance == 0);
    m_instance = this;
}
#endif //STUB_PropertyManager_PropertyManager


//Add custom implementations of stubbed functions here; this file won't be rewritten when re-generating stubs.
#include "rules/simulation/Task.h"

#ifndef STUB_BaseWorld_BaseWorld
#define STUB_BaseWorld_BaseWorld

BaseWorld::BaseWorld(TimeProviderFnType timeProviderFn){
}

#endif //STUB_BaseWorld_BaseWorld

#ifndef STUB_BaseWorld_getEntity
#define STUB_BaseWorld_getEntity

Ref<LocatedEntity> BaseWorld::getEntity(const std::string &id) const {
    return getEntity(strtol(id.c_str(), 0, 10));
}

Ref<LocatedEntity> BaseWorld::getEntity(long id) const
{
    auto I = m_eobjects.find(id);
    if (I != m_eobjects.end()) {
        assert(I->second);
        return I->second;
    } else {
        return 0;
    }
}
#endif //STUB_BaseWorld_getEntity

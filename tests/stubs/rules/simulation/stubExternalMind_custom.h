//Add custom implementations of stubbed functions here; this file won't be rewritten when re-generating stubs.
#include "rules/LocatedEntity.h"
#ifndef STUB_ExternalMind_ExternalMind
#define STUB_ExternalMind_ExternalMind
ExternalMind::ExternalMind(const std::string& strId, long id, Ref<LocatedEntity> entity)
    : Router(strId, id), m_link(nullptr), m_entity(entity)
{

}
#endif //STUB_ExternalMind_ExternalMind

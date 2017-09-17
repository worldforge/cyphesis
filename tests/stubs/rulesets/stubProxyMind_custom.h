//Add custom implementations of stubbed functions here; this file won't be rewritten when re-generating stubs.
#ifndef STUB_ProxyMind_ProxyMind
#define STUB_ProxyMind_ProxyMind
ProxyMind::ProxyMind(const std::string & id, long intId, LocatedEntity& ownerEntity)
    : BaseMind(id, intId), m_ownerEntity(ownerEntity)
{

}
#endif //STUB_ProxyMind_ProxyMind

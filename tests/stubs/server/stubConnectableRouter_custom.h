//Add custom implementations of stubbed functions here; this file won't be rewritten when re-generating stubs.
#ifndef STUB_ConnectableRouter_ConnectableRouter
#define STUB_ConnectableRouter_ConnectableRouter
ConnectableRouter::ConnectableRouter(const std::string & id, long intId, Connection * c )
    : Router(id, intId)
    , m_connection(nullptr)
{

}
#endif //STUB_ConnectableRouter_ConnectableRouter

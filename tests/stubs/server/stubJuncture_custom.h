//Add custom implementations of stubbed functions here; this file won't be rewritten when re-generating stubs.
#ifndef STUB_Juncture_Juncture
#define STUB_Juncture_Juncture
Juncture::Juncture(Connection * c, const std::string & id, long iid)
    : ConnectableRouter(id, iid)
    , m_connection(c)
{

}
#endif //STUB_Juncture_Juncture

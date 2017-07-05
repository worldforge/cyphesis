//Add custom implementations of stubbed functions here; this file won't be rewritten when re-generating stubs.
#ifndef STUB_Connection_Connection
#define STUB_Connection_Connection
Connection::Connection(CommSocket & commSocket, ServerRouting & svr, const std::string & addr, const std::string & id, long iid)
    : Link(commSocket, id, iid), m_server(svr)
{

}
#endif //STUB_Connection_Connection

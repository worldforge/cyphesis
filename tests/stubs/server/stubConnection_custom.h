//Add custom implementations of stubbed functions here; this file won't be rewritten when re-generating stubs.
#include "server/Account.h"

#ifndef STUB_Connection_Connection
#define STUB_Connection_Connection
Connection::Connection(CommSocket & commSocket, ServerRouting & svr, const std::string & addr, const std::string & id, long iid)
    : Link(commSocket, id, iid), m_server(svr)
{

}
#endif //STUB_Connection_Connection

#ifndef STUB_Connection_newAccount
#define STUB_Connection_newAccount
std::unique_ptr<Account> Connection::newAccount(const std::string& type, const std::string& username, const std::string& passwd, const std::string& id, long intId)
{
    return {};
}
#endif //STUB_Connection_newAccount
//Add custom implementations of stubbed functions here; this file won't be rewritten when re-generating stubs.
#ifndef STUB_Account_Account
#define STUB_Account_Account
Account::Account(Connection * conn, const std::string & username, const std::string & passwd, const std::string & id, long intId)
    : ConnectableRouter(id, intId, conn)
{

}
#endif //STUB_Account_Account

//Add custom implementations of stubbed functions here; this file won't be rewritten when re-generating stubs.
#include "rules/LocatedEntity.h"
#ifndef STUB_Account_Account
#define STUB_Account_Account
Account::Account(Connection * conn, std::string  username, std::string passwd, RouterId id)
    : ConnectableRouter(id), m_connection(conn), m_username(std::move(username)), m_password(std::move(passwd))
{

}
#endif //STUB_Account_Account

#ifndef STUB_Account_getType
#define STUB_Account_getType
const char* Account::getType() const
{
    return "account";
}
#endif //STUB_Account_getType

#ifndef STUB_Account_setConnection
#define STUB_Account_setConnection
void Account::setConnection(Connection* connection)
{
    m_connection = connection;
}
#endif //STUB_Account_setConnection

#ifndef STUB_Account_getConnection
#define STUB_Account_getConnection
Connection* Account::getConnection() const
{
    return m_connection;
}
#endif //STUB_Account_getConnection

#ifndef STUB_Account_createMind
#define STUB_Account_createMind
std::unique_ptr<ExternalMind> Account::createMind(const Ref<LocatedEntity>& entity) const
{
    return {};
}
#endif //STUB_Account_createMind
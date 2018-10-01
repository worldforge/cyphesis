//Add custom implementations of stubbed functions here; this file won't be rewritten when re-generating stubs.
#include "rulesets/LocatedEntity.h"
#ifndef STUB_Account_Account
#define STUB_Account_Account
Account::Account(Connection * conn, const std::string & username, const std::string & passwd, const std::string & id, long intId)
    : ConnectableRouter(id, intId), m_username(username), m_password(passwd)
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

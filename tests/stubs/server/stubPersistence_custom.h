#include "server/Account.h"
//Add custom implementations of stubbed functions here; this file won't be rewritten when re-generating stubs.
#ifndef STUB_Persistence_Persistence
#define STUB_Persistence_Persistence
Persistence::Persistence(Database& database)
    : m_db(database)
{

}
#endif //STUB_Persistence_Persistence

#ifndef STUB_Persistence_getAccount
#define STUB_Persistence_getAccount
std::unique_ptr<Account> Persistence::getAccount(const std::string&)
{
    return {};
}
#endif //STUB_Persistence_getAccount
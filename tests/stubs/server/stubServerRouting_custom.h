//Add custom implementations of stubbed functions here; this file won't be rewritten when re-generating stubs.
#include "server/Lobby.h"
#include "server/Account.h"

#ifndef STUB_ServerRouting_ServerRouting
#define STUB_ServerRouting_ServerRouting

ServerRouting::ServerRouting(BaseWorld& wrld, std::string ruleset, std::string name, const std::string& lId, long lIntId)
    : m_svrRuleset(ruleset), m_svrName(name),
    m_lobby(new Lobby(*this, lId, lIntId)), m_numClients(0), m_world(wrld)
{

}

#endif //STUB_ServerRouting_ServerRouting

#ifndef STUB_ServerRouting_addObject
#define STUB_ServerRouting_addObject
void ServerRouting::addObject(std::unique_ptr<ConnectableRouter> obj)
{
    m_objects[obj->getIntId()] = std::move(obj);
}
#endif //STUB_ServerRouting_addObject

#ifndef STUB_ServerRouting_addAccount
#define STUB_ServerRouting_addAccount
void ServerRouting::addAccount(std::unique_ptr<Account> a)
{
    addObject(std::move(a));
}
#endif //STUB_ServerRouting_addAccount

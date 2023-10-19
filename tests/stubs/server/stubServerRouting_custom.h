//Add custom implementations of stubbed functions here; this file won't be rewritten when re-generating stubs.
#include "server/Lobby.h"
#include "server/Account.h"

#ifndef STUB_ServerRouting_ServerRouting
#define STUB_ServerRouting_ServerRouting

ServerRouting::ServerRouting(BaseWorld& wrld, Persistence& persistence, std::string ruleset, std::string name, RouterId lobbyId, AssetsHandler assetsHandler)
    : m_svrRuleset(ruleset), m_svrName(name),
    m_lobby(new Lobby(*this, lobbyId)), m_numClients(0), m_world(wrld), m_persistence(persistence), m_assetsHandler(assetsHandler)
{

}

#endif //STUB_ServerRouting_ServerRouting

#ifndef STUB_ServerRouting_addRouter
#define STUB_ServerRouting_addRouter
void ServerRouting::addRouter(std::unique_ptr<ConnectableRouter> obj)
{
    m_routers[obj->getIntId()] = std::move(obj);
}
#endif //STUB_ServerRouting_addRouter

#ifndef STUB_ServerRouting_addAccount
#define STUB_ServerRouting_addAccount
void ServerRouting::addAccount(std::unique_ptr<Account> a)
{
    addRouter(std::move(a));
}
#endif //STUB_ServerRouting_addAccount

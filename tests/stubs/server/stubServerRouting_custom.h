//Add custom implementations of stubbed functions here; this file won't be rewritten when re-generating stubs.
#ifndef STUB_ServerRouting_ServerRouting
#define STUB_ServerRouting_ServerRouting
ServerRouting::ServerRouting(BaseWorld & wrld, const std::string & ruleset, const std::string & name, const std::string & id, long intId, const std::string & lId, long lIntId)
    : Router(id, intId),
        m_svrRuleset(ruleset), m_svrName(name),
        m_numClients(0), m_world(wrld), m_lobby(*(Lobby*)0)
{

}
#endif //STUB_ServerRouting_ServerRouting

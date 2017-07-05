//Add custom implementations of stubbed functions here; this file won't be rewritten when re-generating stubs.
#ifndef STUB_Lobby_Lobby
#define STUB_Lobby_Lobby
Lobby::Lobby(ServerRouting & serverRouting, const std::string & id, long intId)
    : Router(id, intId),
      m_server(serverRouting)
{

}
#endif //STUB_Lobby_Lobby

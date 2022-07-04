//Add custom implementations of stubbed functions here; this file won't be rewritten when re-generating stubs.
#ifndef STUB_Lobby_Lobby
#define STUB_Lobby_Lobby
Lobby::Lobby(ServerRouting & serverRouting, RouterId id)
    : Router(id),
      m_server(serverRouting)
{

}
#endif //STUB_Lobby_Lobby

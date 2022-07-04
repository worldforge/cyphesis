//Add custom implementations of stubbed functions here; this file won't be rewritten when re-generating stubs.
#ifndef STUB_CharacterClient_CharacterClient
#define STUB_CharacterClient_CharacterClient
CharacterClient::CharacterClient(RouterId mindId, const std::string & entityId, ClientConnection& c, TypeStore& typeStore)
    : BaseMind(mindId, entityId, typeStore), m_connection(c)
{

}
#endif //STUB_CharacterClient_CharacterClient

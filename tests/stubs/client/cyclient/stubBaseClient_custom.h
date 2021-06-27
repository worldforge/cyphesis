//Add custom implementations of stubbed functions here; this file won't be rewritten when re-generating stubs.
#ifndef STUB_BaseClient_BaseClient
#define STUB_BaseClient_BaseClient
BaseClient::BaseClient(boost::asio::io_context& io_context, Atlas::Objects::Factories& factories, TypeStore& typeStore):
        m_typeStore(typeStore),
    m_connection(io_context, factories),
    m_character(nullptr)
{
}
#endif //STUB_BaseClient_BaseClient

#ifndef STUB_BaseClient_extractFirstArg
#define STUB_BaseClient_extractFirstArg
Atlas::Objects::Entity::RootEntity BaseClient::extractFirstArg(Atlas::Objects::Operation::RootOperation op)
{
    return {};
}
#endif //STUB_BaseClient_extractFirstArg
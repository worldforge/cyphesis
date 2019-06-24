//Add custom implementations of stubbed functions here; this file won't be rewritten when re-generating stubs.
#ifndef STUB_BaseClient_BaseClient
#define STUB_BaseClient_BaseClient
BaseClient::BaseClient(boost::asio::io_context& io_context) :
    m_connection(io_context),
    m_character(nullptr)
{
}
#endif //STUB_BaseClient_BaseClient

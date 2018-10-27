//Add custom implementations of stubbed functions here; this file won't be rewritten when re-generating stubs.
#ifndef STUB_BaseClient_BaseClient
#define STUB_BaseClient_BaseClient
BaseClient::BaseClient(boost::asio::io_service& io_service) :
    m_connection(io_service),
    m_character(nullptr)
{
}
#endif //STUB_BaseClient_BaseClient

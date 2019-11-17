//Add custom implementations of stubbed functions here; this file won't be rewritten when re-generating stubs.
#ifndef STUB_CommPeer_CommPeer
#define STUB_CommPeer_CommPeer
CommPeer::CommPeer(const std::string& name, boost::asio::io_context& io_context, Atlas::Objects::Factories& factories)
    : CommAsioClient(name, io_context, factories), m_auth_timer(io_context)
{

}
#endif //STUB_CommPeer_CommPeer

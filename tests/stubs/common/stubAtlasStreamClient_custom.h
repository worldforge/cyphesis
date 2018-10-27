//Add custom implementations of stubbed functions here; this file won't be rewritten when re-generating stubs.
#ifndef STUB_StreamClientSocketBase_StreamClientSocketBase
#define STUB_StreamClientSocketBase_StreamClientSocketBase
StreamClientSocketBase::StreamClientSocketBase(boost::asio::io_service& io_service, std::function<void()>& dispatcher)
    : m_io_service(io_service),
      mDispatcher(dispatcher),
      m_ios(&mBuffer)
{

}
#endif //STUB_StreamClientSocketBase_StreamClientSocketBase


#ifndef STUB_TcpStreamClientSocket_TcpStreamClientSocket
#define STUB_TcpStreamClientSocket_TcpStreamClientSocket
TcpStreamClientSocket::TcpStreamClientSocket(boost::asio::io_service& io_service, std::function<void()>& dispatcher, boost::asio::ip::tcp::endpoint endpoint)
    : StreamClientSocketBase(io_service, dispatcher), m_socket(io_service)
{

}
#endif //STUB_TcpStreamClientSocket_TcpStreamClientSocket


#ifndef STUB_LocalStreamClientSocket_LocalStreamClientSocket
#define STUB_LocalStreamClientSocket_LocalStreamClientSocket
LocalStreamClientSocket::LocalStreamClientSocket(boost::asio::io_service& io_service, std::function<void()>& dispatcher, boost::asio::local::stream_protocol::endpoint endpoint)
    : StreamClientSocketBase(io_service, dispatcher), m_socket(io_service)
{

}
#endif //STUB_LocalStreamClientSocket_LocalStreamClientSocket

#ifndef STUB_AtlasStreamClient_AtlasStreamClient
#define STUB_AtlasStreamClient_AtlasStreamClient
AtlasStreamClient::AtlasStreamClient(boost::asio::io_service& io_service)
    : m_io_service(io_service)
{

}
#endif //STUB_AtlasStreamClient_AtlasStreamClient

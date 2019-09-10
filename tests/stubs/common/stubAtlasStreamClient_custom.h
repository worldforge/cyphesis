//Add custom implementations of stubbed functions here; this file won't be rewritten when re-generating stubs.
#ifndef STUB_StreamClientSocketBase_StreamClientSocketBase
#define STUB_StreamClientSocketBase_StreamClientSocketBase
StreamClientSocketBase::StreamClientSocketBase(boost::asio::io_context& io_context, std::function<void()>& dispatcher)
    : m_io_context(io_context),
      mDispatcher(dispatcher),
      m_ios(&mBuffer)
{

}
#endif //STUB_StreamClientSocketBase_StreamClientSocketBase


#ifndef STUB_TcpStreamClientSocket_TcpStreamClientSocket
#define STUB_TcpStreamClientSocket_TcpStreamClientSocket
TcpStreamClientSocket::TcpStreamClientSocket(boost::asio::io_context& io_context, std::function<void()>& dispatcher, boost::asio::ip::tcp::endpoint endpoint)
    : StreamClientSocketBase(io_context, dispatcher), m_socket(io_context)
{

}
#endif //STUB_TcpStreamClientSocket_TcpStreamClientSocket


#ifndef STUB_LocalStreamClientSocket_LocalStreamClientSocket
#define STUB_LocalStreamClientSocket_LocalStreamClientSocket
LocalStreamClientSocket::LocalStreamClientSocket(boost::asio::io_context& io_context, std::function<void()>& dispatcher, boost::asio::local::stream_protocol::endpoint endpoint)
    : StreamClientSocketBase(io_context, dispatcher), m_socket(io_context)
{

}
#endif //STUB_LocalStreamClientSocket_LocalStreamClientSocket

#ifndef STUB_AtlasStreamClient_AtlasStreamClient
#define STUB_AtlasStreamClient_AtlasStreamClient
AtlasStreamClient::AtlasStreamClient(boost::asio::io_context& io_context, const Atlas::Objects::Factories& factories)
    :    ObjectsDecoder(factories),
         m_io_context(io_context)
{

}
#endif //STUB_AtlasStreamClient_AtlasStreamClient

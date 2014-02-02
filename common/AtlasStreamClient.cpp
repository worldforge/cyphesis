// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2009 Alistair Riddoch
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include "common/AtlasStreamClient.h"
#include "common/ClientTask.h"
#include "common/system.h"

#include "common/debug.h"

#include <Atlas/Codec.h>
#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/Encoder.h>
#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/SmartPtr.h>
#include <Atlas/Net/Stream.h>

#ifdef _WIN32
#undef DATADIR
#endif // _WIN32

#include <skstream/skstream.h>
#include <skstream/skstream_unix.h>

#include <boost/asio/steady_timer.hpp>

#include <cstdio>

using Atlas::Message::Element;
using Atlas::Message::ListType;
using Atlas::Message::MapType;
using Atlas::Objects::Root;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Operation::Create;
using Atlas::Objects::Operation::Login;
using Atlas::Objects::Operation::RootOperation;

using namespace boost::asio;


StreamClientSocketBase::StreamClientSocketBase(boost::asio::io_service& io_service)
: m_io_service(io_service), m_ios(&mBuffer), m_codec(nullptr), m_encoder(nullptr), m_is_connected(false)
{
}

StreamClientSocketBase::~StreamClientSocketBase()
{
    delete m_encoder;
    delete m_codec;
}

std::iostream& StreamClientSocketBase::getIos()
{
    return m_ios;
}

int StreamClientSocketBase::negotiate(Atlas::Objects::ObjectsDecoder& decoder)
{

    Atlas::Net::StreamConnect conn("cyphesis_client", m_ios);

    while (conn.getState() == Atlas::Net::StreamConnect::IN_PROGRESS) {
      write();
      auto dataReceived = read();
      conn.poll(dataReceived > 0);
    }

    if (conn.getState() == Atlas::Net::StreamConnect::FAILED) {
        std::cerr << "Failed to negotiate" << std::endl;
        return -1;
    }

    m_codec = conn.getCodec(decoder);

    if (!m_codec) {
        return -1;
    }

    m_encoder = new Atlas::Objects::ObjectsEncoder(*m_codec);

    m_codec->streamBegin();

    do_read();

    return 0;
}

Atlas::Codec& StreamClientSocketBase::getCodec()
{
    return *m_codec;
}

Atlas::Objects::ObjectsEncoder& StreamClientSocketBase::getEncoder()
{
    return *m_encoder;
}

int StreamClientSocketBase::poll(const std::chrono::steady_clock::time_point& expireTime)
{
    bool hasExpired = false;
    steady_timer timer(m_io_service);
    timer.expires_at(expireTime);
    timer.async_wait([&](boost::system::error_code ec){
        if (!ec) {
            hasExpired = true;
        }
    });
    m_io_service.run_one();
    if (!m_is_connected) {
        return -1;
    }
    if (hasExpired) {
        return 1;
    }
    return 0;
}




TcpStreamClientSocket::TcpStreamClientSocket(boost::asio::io_service& io_service, boost::asio::ip::tcp::endpoint endpoint)
: StreamClientSocketBase(io_service), m_socket(io_service)
{
    m_socket.connect(endpoint);
    m_is_connected = true;
}

void TcpStreamClientSocket::do_read()
{
    m_socket.async_read_some(mReadBuffer.prepare(read_buffer_size),
            [this](boost::system::error_code ec, std::size_t length)
            {
                if (!ec)
                {
                    mReadBuffer.commit(length);
                    this->m_ios.rdbuf(&mReadBuffer);
                    m_codec->poll();
                    this->m_ios.rdbuf(&mBuffer);
                    this->do_read();
                }
            });
}

int TcpStreamClientSocket::read()
{
    if (!m_socket.is_open()) {
        return -1;
    }
    if (m_socket.available() > 0) {
        auto received = m_socket.read_some(mBuffer.prepare(m_socket.available()));
        if (received > 0) {
            mBuffer.commit(received);
        }
        return received;
    }
    return 0;
}

int TcpStreamClientSocket::write()
{
    if (!m_socket.is_open()) {
        return -1;
    }
    if (mBuffer.size() == 0) {
        return 0;
    }

    auto size = boost::asio::write(m_socket, mBuffer.data(),
              boost::asio::transfer_all());
    mBuffer.consume(size);
    return size;
}



LocalStreamClientSocket::LocalStreamClientSocket(boost::asio::io_service& io_service, boost::asio::local::stream_protocol::endpoint endpoint)
: StreamClientSocketBase(io_service), m_socket(io_service)
{
    m_socket.connect(endpoint);
    m_is_connected = true;
}

void LocalStreamClientSocket::do_read()
{
    m_socket.async_read_some(mReadBuffer.prepare(read_buffer_size),
            [this](boost::system::error_code ec, std::size_t length)
            {
                if (!ec)
                {
                    mReadBuffer.commit(length);
                    this->m_ios.rdbuf(&mReadBuffer);
                    m_codec->poll();
                    this->m_ios.rdbuf(&mBuffer);
                    this->do_read();
                }
            });
}

int LocalStreamClientSocket::read()
{
    if (!m_socket.is_open()) {
        return -1;
    }
    if (m_socket.available() > 0) {
        int received = m_socket.read_some(mBuffer.prepare(m_socket.available()));
        if (received > 0) {
            mBuffer.commit(received);
        }
        return received;
    }
    return 0;
}

int LocalStreamClientSocket::write()
{
    if (!m_socket.is_open()) {
        return -1;
    }
    if (mBuffer.size() == 0) {
        return 0;
    }

    auto size = boost::asio::write(m_socket, mBuffer.data(),
              boost::asio::transfer_all());
    mBuffer.consume(size);
    return size;
}



void AtlasStreamClient::output(const Element & item, int depth) const
{
    output_element(std::cout, item, depth);
}

void AtlasStreamClient::output(const Root & ent) const
{
    MapType entmap = ent->asMessage();
    MapType::const_iterator Iend = entmap.end();
    for (MapType::const_iterator I = entmap.begin(); I != Iend; ++I) {
        const Element & item = I->second;
        std::cout << std::string(spacing(), ' ') << I->first << ": ";
        output(item, 1);
        std::cout << std::endl;
    }
}

/// \brief Function call from the base class when an object arrives from the
/// server
///
/// @param obj Object that has arrived from the server
void AtlasStreamClient::objectArrived(const Root & obj)
{
    RootOperation op = Atlas::Objects::smart_dynamic_cast<RootOperation>(obj);
    if (!op.isValid()) {
        std::cerr << "ERROR: Non op object received from server"
                  << std::endl << std::flush;;
        if (!obj->isDefaultParents() && !obj->getParents().empty()) {
            std::cerr << "NOTICE: Unexpected object has parent "
                      << obj->getParents().front()
                      << std::endl << std::flush;
        }
        if (!obj->isDefaultObjtype()) {
            std::cerr << "NOTICE: Unexpected object has objtype "
                      << obj->getObjtype()
                      << std::endl << std::flush;
        }
        return;
    }

    operation(op);
}

void AtlasStreamClient::operation(const RootOperation & op)
{
    if (m_currentTask != 0) {
        OpVector res;
        m_currentTask->operation(op, res);
        OpVector::const_iterator Iend = res.end();
        for (OpVector::const_iterator I = res.begin(); I != Iend; ++I) {
            send(*I);
        }

        if (m_currentTask->isComplete()) {
            delete m_currentTask;
            m_currentTask = 0;
        }
    }

    switch (op->getClassNo()) {
        case Atlas::Objects::Operation::APPEARANCE_NO:
            appearanceArrived(op);
            break;
        case Atlas::Objects::Operation::DISAPPEARANCE_NO:
            disappearanceArrived(op);
            break;
        case Atlas::Objects::Operation::INFO_NO:
            infoArrived(op);
            break;
        case Atlas::Objects::Operation::ERROR_NO:
            errorArrived(op);
            break;
        case Atlas::Objects::Operation::SIGHT_NO:
            sightArrived(op);
            break;
        case Atlas::Objects::Operation::SOUND_NO:
            soundArrived(op);
            break;
        default:
            break;
    }
}

void AtlasStreamClient::infoArrived(const RootOperation & op)
{
    reply_flag = true;
    if (!op->isDefaultFrom()) {
        return;
    }
    if (op->isDefaultArgs() || op->getArgs().empty()) {
        std::cerr << "WARNING: Malformed account from server" << std::endl << std::flush;
        return;
    }
    if (op->isDefaultRefno()) {
        return;
    }
    if (op->getRefno() != serialNo) {
        return;
    }
    m_infoReply = op->getArgs().front();
}

void AtlasStreamClient::appearanceArrived(const RootOperation & op)
{
}

void AtlasStreamClient::disappearanceArrived(const RootOperation & op)
{
}

void AtlasStreamClient::sightArrived(const RootOperation & op)
{
}

void AtlasStreamClient::soundArrived(const RootOperation & op)
{
}

void AtlasStreamClient::loginSuccess(const Atlas::Objects::Root & arg)
{
}

/// \brief Called when an Error operation arrives
///
/// @param op Operation to be processed
void AtlasStreamClient::errorArrived(const RootOperation & op)
{
    reply_flag = true;
    error_flag = true;
    const std::vector<Root> & args = op->getArgs();
    if (args.empty()) {
        return;
    }
    const Root & arg = args.front();
    Element message_attr;
    if (arg->copyAttr("message", message_attr) == 0 && message_attr.isString()) {
        m_errorMessage = message_attr.String();
    }
}

AtlasStreamClient::AtlasStreamClient() : reply_flag(false), error_flag(false),
                                         serialNo(512), m_socket(nullptr),
                                         m_currentTask(0), m_spacing(2)
{
}

AtlasStreamClient::~AtlasStreamClient()
{
    delete m_socket;
}

void AtlasStreamClient::send(const RootOperation & op)
{
    if (m_socket == 0) {
        return;
    }

    reply_flag = false;
    error_flag = false;
    m_socket->getEncoder().streamObjectsMessage(op);
    m_socket->write();
}

int AtlasStreamClient::connect(const std::string & host, int port)
{
    delete m_socket;
    m_socket = nullptr;
    try {
        m_socket = new TcpStreamClientSocket(m_io_service, ip::tcp::endpoint(boost::asio::ip::address::from_string(host), port));
    } catch (const std::exception& e) {
        return -1;
    }
    return m_socket->negotiate(*this);

}

int AtlasStreamClient::connectLocal(const std::string & filename)
{
    delete m_socket;
    m_socket = nullptr;
    try {
        m_socket = new LocalStreamClientSocket(m_io_service, local::stream_protocol::endpoint(filename));
    } catch (const std::exception& e) {
        return -1;
    }
    return m_socket->negotiate(*this);
}

int AtlasStreamClient::cleanDisconnect()
{
    delete m_socket;
    m_socket = nullptr;
//    // Shutting down our write side will cause the server to get a HUP once
//    // it has consumed all we have left for it
//    m_ios->shutdown(true);
//    // The server will then close the socket once we have all the responses
//    while (this->poll(20, 0) == 0);
    return 0;
}


int AtlasStreamClient::login(const std::string & username,
                             const std::string & password)
{
    m_username = username;

    Login l;
    Anonymous account;
 
    account->setAttr("username", username);
    account->setAttr("password", password);
 
    l->setArgs1(account);
    l->setSerialno(newSerialNo());
 
    send(l);

    return waitForLoginResponse();
}

int AtlasStreamClient::create(const std::string & type,
                              const std::string & username,
                              const std::string & password)
{
    m_username = username;

    Create c;
    Anonymous account;

    account->setAttr("username", username);
    account->setAttr("password", password);
    account->setParents(std::list<std::string>(1, type));

    c->setArgs1(account);
    c->setSerialno(newSerialNo());

    send(c);

    return waitForLoginResponse();
}

int AtlasStreamClient::waitForLoginResponse()
{
    auto expireTime = std::chrono::steady_clock::now() + std::chrono::seconds(10);
    while (poll(expireTime) == 0) {
        if (reply_flag && !error_flag) {
            if (m_infoReply->isDefaultId()) {
               std::cerr << "Malformed reply" << std::endl << std::flush;
            } else {
                m_accountId = m_infoReply->getId();
                if (!m_infoReply->getParents().empty()) {
                    m_accountType = m_infoReply->getParents().front();
                }
                loginSuccess(m_infoReply);
                return 0;
            }
            reply_flag = false;
        }
    }

    return -1;
}

int AtlasStreamClient::poll(const std::chrono::steady_clock::time_point& timeout)
{
    int result = m_socket->poll(timeout);
    if (result == -1) {
        std::cerr << "Server disconnected" << std::endl << std::flush;
    }
    return result;
}

int AtlasStreamClient::poll(int timeOut, int msec)
{
    auto expireTime = std::chrono::steady_clock::now() + std::chrono::seconds(timeOut) + std::chrono::microseconds(msec);
    return poll(expireTime);
}

int AtlasStreamClient::runTask(ClientTask * task, const std::string & arg)
{
    assert(task != 0);

    if (m_currentTask != 0) {
        std::cout << "Busy" << std::endl << std::flush;
        return -1;
    }

    m_currentTask = task;

    OpVector res;

    m_currentTask->setup(arg, res);

    if (m_currentTask->isComplete()) {
        delete task;
        m_currentTask = 0;
        return -1;
    }

    OpVector::const_iterator Iend = res.end();
    for (OpVector::const_iterator I = res.begin(); I != Iend; ++I) {
        send(*I);
    }
    return 0;
}

int AtlasStreamClient::endTask()
{
    if (m_currentTask == 0) {
        return -1;
    }
    delete m_currentTask;
    m_currentTask = 0;
    return 0;
}

bool AtlasStreamClient::hasTask() const {
    return m_currentTask != nullptr;
}

int AtlasStreamClient::pollUntilTaskComplete()
{
    if (m_currentTask == nullptr) {
        return 1;
    }
    while (m_currentTask != nullptr) {
        auto expireTime = std::chrono::steady_clock::now() + std::chrono::milliseconds(100);
        if (poll(expireTime) == -1) {
            return -1;
        }
    }
    return 0;
}



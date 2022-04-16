#include <memory>

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
#endif // HAVE_CONFIG_H

#include "AtlasStreamClient.h"
#include "common/ClientTask.h"

#include "common/debug.h"
#include "common/log.h"
#include "compose.hpp"


#include <Atlas/Codec.h>
#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/Encoder.h>
#include <Atlas/Net/Stream.h>
#include <boost/asio/steady_timer.hpp>

#include <iostream>

using Atlas::Message::Element;
using Atlas::Message::ListType;
using Atlas::Message::MapType;
using Atlas::Objects::Root;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Operation::Create;
using Atlas::Objects::Operation::Login;
using Atlas::Objects::Operation::RootOperation;

using namespace boost::asio;

static const bool debug_flag = false;


StreamClientSocketBase::StreamClientSocketBase(boost::asio::io_context& io_context, std::function<void()>& dispatcher)
    : m_io_context(io_context),
      mDispatcher(dispatcher),
      m_ios(&mBuffer),
      m_is_connected(false)
{
}

StreamClientSocketBase::~StreamClientSocketBase() = default;

std::iostream& StreamClientSocketBase::getIos()
{
    return m_ios;
}

int StreamClientSocketBase::negotiate(Atlas::Objects::ObjectsDecoder& decoder)
{

    Atlas::Net::StreamConnect conn("cyphesis_client", m_ios, m_ios);

    while (conn.getState() == Atlas::Net::StreamConnect::IN_PROGRESS) {
        write();
        auto dataReceived = read_blocking();
        if (dataReceived > 0) {
            m_ios.peek();
        }
        conn.poll();
    }

    if (conn.getState() == Atlas::Net::StreamConnect::FAILED) {
        std::cerr << "Failed to negotiate" << std::endl;
        return -1;
    }

    m_codec = conn.getCodec(decoder);

    if (!m_codec) {
        return -1;
    }

    m_encoder = std::make_unique<Atlas::Objects::ObjectsEncoder>(*m_codec);

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

int StreamClientSocketBase::poll(const std::chrono::steady_clock::duration& duration)
{
    return poll(duration, []() -> bool { return false; });
}

int StreamClientSocketBase::poll(const std::chrono::steady_clock::duration& duration, const std::function<bool()>& exitCheckerFn)
{
    bool hasExpired = false;
    bool isCancelled = false;
    auto expirationTime = std::chrono::steady_clock::now() + duration;
    boost::asio::steady_timer timer(m_io_context);
#if BOOST_VERSION >= 106600
    timer.expires_after(duration);
#else
    timer.expires_from_now(duration);
#endif
    timer.async_wait([&](boost::system::error_code ec) {
        if (!ec) {
            hasExpired = true;
        } else {
            isCancelled = true;
        }
    });

    //We'll try to only run one handler each polling. Either our timer gets called, or one of the network handlers.
    //The reason for this loop is that when we cancel the timer we need to poll run handlers until the timer handler
    //has been run, since it references locally scoped variables.
    while (!hasExpired && !isCancelled && !exitCheckerFn() && std::chrono::steady_clock::now() < expirationTime) {
        m_io_context.run_one();
        //Check if we didn't run the timer handler; if so we should cancel it and then keep on polling until
        //it's been run.
        if (!hasExpired && !isCancelled) {
            timer.cancel();
        }
    }
    if (!m_is_connected) {
        return -1;
    }
    if (hasExpired) {
        return 1;
    }
    return 0;
}

bool StreamClientSocketBase::isConnected() const
{
    return m_is_connected;
}


TcpStreamClientSocket::TcpStreamClientSocket(boost::asio::io_context& io_context, std::function<void()>& dispatcher, boost::asio::ip::tcp::endpoint endpoint)
    : StreamClientSocketBase(io_context, dispatcher), m_socket(io_context)
{
    m_socket.connect(endpoint);
    m_is_connected = true;
}

void TcpStreamClientSocket::do_read()
{
    m_socket.async_read_some(mReadBuffer.prepare(read_buffer_size),
                             [this](boost::system::error_code ec, std::size_t length) {
                                 if (!ec) {
                                     mReadBuffer.commit(length);
                                     this->m_ios.rdbuf(&mReadBuffer);
                                     m_codec->poll();
                                     this->m_ios.rdbuf(&mBuffer);
                                     mDispatcher();
                                     this->do_read();
                                 } else {
                                     m_is_connected = false;
                                 }
                             });
}

size_t TcpStreamClientSocket::read_blocking()
{
    if (!m_socket.is_open()) {
        return 0;
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

size_t TcpStreamClientSocket::write()
{
    if (!m_socket.is_open()) {
        return 0;
    }
    if (mBuffer.size() == 0) {
        return 0;
    }

    auto size = boost::asio::write(m_socket, mBuffer.data(),
                                   boost::asio::transfer_all());
    mBuffer.consume(size);
    return size;
}


LocalStreamClientSocket::LocalStreamClientSocket(boost::asio::io_context& io_context, std::function<void()>& dispatcher, boost::asio::local::stream_protocol::endpoint endpoint)
    : StreamClientSocketBase(io_context, dispatcher), m_socket(io_context)
{
    m_socket.connect(endpoint);
    m_is_connected = true;
}

void LocalStreamClientSocket::do_read()
{
    m_socket.async_read_some(mReadBuffer.prepare(read_buffer_size),
                             [this](boost::system::error_code ec, std::size_t length) {
                                 if (!ec) {
                                     mReadBuffer.commit(length);
                                     this->m_ios.rdbuf(&mReadBuffer);
                                     m_codec->poll();
                                     this->m_ios.rdbuf(&mBuffer);
                                     mDispatcher();
                                     this->do_read();
                                 } else {
                                     m_is_connected = false;
                                 }
                             });
}

size_t LocalStreamClientSocket::read_blocking()
{
    if (!m_socket.is_open()) {
        return 0;
    }
    if (m_socket.available() > 0) {
        size_t received = m_socket.read_some(mBuffer.prepare(m_socket.available()));
        if (received > 0) {
            mBuffer.commit(received);
        }
        return received;
    }
    return 0;
}

size_t LocalStreamClientSocket::write()
{
    if (!m_socket.is_open()) {
        return 0;
    }
    if (mBuffer.size() == 0) {
        return 0;
    }

    auto size = boost::asio::write(m_socket, mBuffer.data(),
                                   boost::asio::transfer_all());
    mBuffer.consume(size);
    return size;
}


void AtlasStreamClient::output(const Element& item, size_t depth) const
{
    output_element(std::cout, item, depth);
}

void AtlasStreamClient::output(const Root& ent) const
{
    MapType entmap = ent->asMessage();
    MapType::const_iterator Iend = entmap.end();
    for (MapType::const_iterator I = entmap.begin(); I != Iend; ++I) {
        const Element& item = I->second;
        std::cout << std::string(spacing(), ' ') << I->first << ": ";
        output(item, 1);
        std::cout << std::endl;
    }
}

/// \brief Function call from the base class when an object arrives from the
/// server
///
/// @param obj Object that has arrived from the server
void AtlasStreamClient::objectArrived(Root obj)
{
    RootOperation op = Atlas::Objects::smart_dynamic_cast<RootOperation>(obj);
    if (!op.isValid()) {
        std::cerr << "ERROR: Non op object received from server"
                  << std::endl << std::flush;;
        if (!obj->isDefaultParent()) {
            std::cerr << "NOTICE: Unexpected object has parent "
                      << obj->getParent()
                      << std::endl << std::flush;
        }
        if (!obj->isDefaultObjtype()) {
            std::cerr << "NOTICE: Unexpected object has objtype "
                      << obj->getObjtype()
                      << std::endl << std::flush;
        }
        return;
    }

    mOps.push_back(std::move(op));
}

void AtlasStreamClient::dispatch()
{
    for (auto& op : mOps) {
        operation(op);
    }
    mOps.clear();
}


void AtlasStreamClient::operation(const RootOperation& op)
{
    if (debug_flag) {
        debug_print("Received:");
        debug_dump(op, std::cerr);
    }
    if (m_currentTask != nullptr) {
        OpVector res;
        m_currentTask->operation(op, res);
        OpVector::const_iterator Iend = res.end();
        for (OpVector::const_iterator I = res.begin(); I != Iend; ++I) {
            send(*I);
        }

        if (m_currentTask->isComplete()) {
            m_currentTask = nullptr;
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

void AtlasStreamClient::infoArrived(const RootOperation& op)
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

void AtlasStreamClient::appearanceArrived(const RootOperation& op)
{
}

void AtlasStreamClient::disappearanceArrived(const RootOperation& op)
{
}

void AtlasStreamClient::sightArrived(const RootOperation& op)
{
}

void AtlasStreamClient::soundArrived(const RootOperation& op)
{
}

void AtlasStreamClient::loginSuccess(const Atlas::Objects::Root& arg)
{
}

/// \brief Called when an Error operation arrives
///
/// @param op Operation to be processed
void AtlasStreamClient::errorArrived(const RootOperation& op)
{
    reply_flag = true;
    error_flag = true;
    const std::vector<Root>& args = op->getArgs();
    if (args.empty()) {
        return;
    }
    const Root& arg = args.front();
    Element message_attr;
    if (arg->copyAttr("message", message_attr) == 0 && message_attr.isString()) {
        m_errorMessage = message_attr.String();
    }
}

AtlasStreamClient::AtlasStreamClient(boost::asio::io_context& io_context, const Atlas::Objects::Factories& factories) :
    ObjectsDecoder(factories),
    m_io_context(io_context),
    reply_flag(false),
    error_flag(false),
    serialNo(512),
    m_socket(nullptr),
    m_currentTask(nullptr),
    m_spacing(2)
{
}

AtlasStreamClient::~AtlasStreamClient()
{
    //Send a Logout op to inform the server that we're shutting down cleanly.
    send(Atlas::Objects::Operation::Logout());
}

void AtlasStreamClient::send(const RootOperation& op)
{
    if (!m_socket) {
        return;
    }

    if (debug_flag) {
        debug_print("Sending:");
        debug_dump(op, std::cerr);
    }

    reply_flag = false;
    error_flag = false;
    m_socket->getEncoder().streamObjectsMessage(op);
    m_socket->write();
}

int AtlasStreamClient::connect(const std::string& host, unsigned short port)
{
    m_socket.reset();
    try {
        std::function<void()> dispatcher = [&] { this->dispatch(); };
        m_socket = std::make_unique<TcpStreamClientSocket>(m_io_context, dispatcher, ip::tcp::endpoint(boost::asio::ip::address::from_string(host), port));
    } catch (const std::exception& e) {
        return -1;
    }
    return m_socket->negotiate(*this);

}

int AtlasStreamClient::connectLocal(const std::string& filename)
{
    m_socket.reset();

    try {
        std::function<void()> dispatcher = [&] { this->dispatch(); };
        m_socket = std::make_unique<LocalStreamClientSocket>(m_io_context, dispatcher, local::stream_protocol::endpoint(filename));
    } catch (const std::exception& e) {
        log(ERROR, String::compose("Error when connecting to local socket.\n%1", e.what()));
        return -1;
    }
    return m_socket->negotiate(*this);
}

int AtlasStreamClient::cleanDisconnect()
{
    m_socket.reset();
//    // Shutting down our write side will cause the server to get a HUP once
//    // it has consumed all we have left for it
//    m_ios->shutdown(true);
//    // The server will then close the socket once we have all the responses
//    while (this->poll(20, 0) == 0);
    return 0;
}


int AtlasStreamClient::login(const std::string& username,
                             const std::string& password)
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

int AtlasStreamClient::create(const std::string& type,
                              const std::string& username,
                              const std::string& password)
{
    m_username = username;

    Create c;
    Anonymous account;

    account->setAttr("username", username);
    account->setAttr("password", password);
    account->setParent(type);

    c->setArgs1(account);
    c->setSerialno(newSerialNo());

    send(c);

    return waitForLoginResponse();
}

int AtlasStreamClient::waitForLoginResponse()
{
    while (poll(std::chrono::seconds(10)) == 0) {
        if (reply_flag && !error_flag) {
            if (m_infoReply->isDefaultId()) {
                std::cerr << "Malformed reply" << std::endl << std::flush;
            } else {
                m_accountId = m_infoReply->getId();
                m_accountType = m_infoReply->getParent();
                loginSuccess(m_infoReply);
                return 0;
            }
            reply_flag = false;
        }
    }

    return -1;
}

int AtlasStreamClient::pollOne(const std::chrono::steady_clock::duration& duration)
{
    if (!m_socket) {
        return -1;
    }
    int result = m_socket->poll(duration, [&]() -> bool { return !mOps.empty(); });
    if (result == -1) {
        std::cerr << "Server disconnected" << std::endl << std::flush;
    }
    return result;
}


int AtlasStreamClient::poll(const std::chrono::steady_clock::duration& duration)
{
    if (!m_socket) {
        return -1;
    }
    int result = m_socket->poll(duration, []() { return false; });
    if (result == -1) {
        std::cerr << "Server disconnected" << std::endl << std::flush;
    }
    return result;
}

int AtlasStreamClient::runTask(std::shared_ptr<ClientTask> task, const std::string& arg)
{
    assert(task != nullptr);

    if (m_currentTask != nullptr) {
        std::cout << "Busy" << std::endl << std::flush;
        return -1;
    }

    m_currentTask = std::move(task);

    OpVector res;

    m_currentTask->setup(arg, res);

    if (m_currentTask->isComplete()) {
        m_currentTask.reset();
        return -1;
    }

    for (auto& op : res) {
        send(op);
    }
    return 0;
}

int AtlasStreamClient::endTask()
{
    if (!m_currentTask) {
        return -1;
    }
    m_currentTask.reset();
    return 0;
}

bool AtlasStreamClient::hasTask() const
{
    return m_currentTask != nullptr;
}

int AtlasStreamClient::pollUntilTaskComplete()
{
    if (!m_currentTask) {
        return 1;
    }
    while (m_currentTask) {
        if (poll(std::chrono::milliseconds(100)) == -1) {
            return -1;
        }
    }
    return 0;
}




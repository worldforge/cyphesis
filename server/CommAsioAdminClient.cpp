/*
 Copyright (C) 2014 Erik Ogenvik

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "common/log.h"
#include "common/compose.hpp"

#include "CommAsioAdminClient.h"

#include <Atlas/Objects/Encoder.h>
#include <Atlas/Objects/RootOperation.h>
#include <Atlas/Objects/SmartPtr.h>
#include <Atlas/Net/Stream.h>

CommAsioAdminClient::CommAsioAdminClient(CommServer& commServer,
        const std::string & name,
        boost::asio::local::stream_protocol::socket socket) :
        CommSocket(commServer), mSocket(std::move(socket)), mStream(
                &mWriteBuffer), mNegotiateTimer(mSocket.get_io_service(),
                boost::posix_time::seconds(1)), m_codec(nullptr), m_encoder(
                nullptr), m_link(nullptr), m_connectTime(commServer.time())
{
    m_negotiate = new Atlas::Net::StreamAccept("cyphesis " + name, mStream);
}

CommAsioAdminClient::~CommAsioAdminClient()
{
    delete m_link;
    delete m_negotiate;
    delete m_encoder;
    delete m_codec;
}

void CommAsioAdminClient::do_read()
{
    auto self(shared_from_this());
    mSocket.async_read_some(mReadBuffer.prepare(read_buffer_size),
            [this, self](boost::system::error_code ec, std::size_t length)
            {
                if (!ec)
                {
                    mReadBuffer.commit(length);
                    mStream.rdbuf(&mReadBuffer);
                    m_codec->poll();
                    mStream.rdbuf(&mWriteBuffer);
                    dispatch();
                    //By calling do_read again we make sure that the instance
                    //doesn't go out of scope ("shared_from this"). As soon as that
                    //doesn't happen, and there's no do_write in progress, the instance
                    //will be deleted since there's no more references to it.
                    do_read();
                }
            });
}

void CommAsioAdminClient::do_write()
{
    auto self(shared_from_this());

    if (mWriteBuffer.size() != 0) {
        boost::asio::async_write(mSocket, mWriteBuffer.data(),
                [this, self](boost::system::error_code ec, std::size_t length)
                {
                    if (!ec)
                    {
                        mWriteBuffer.consume(length);
                    }
                });
    }
}

void CommAsioAdminClient::negotiate_read()
{
    auto self(shared_from_this());
    mSocket.async_read_some(mWriteBuffer.prepare(read_buffer_size),
            [this, self](boost::system::error_code ec, std::size_t length)
            {
                if (!ec)
                {
                    mWriteBuffer.commit(length);
                    if (negotiate() == 0 && m_negotiate == nullptr) {
                        negotiate_write();
                        do_read();
                    } else {
                        negotiate_write();
                        negotiate_read();
                    }
                }
            });
}

void CommAsioAdminClient::negotiate_write()
{
    auto self(shared_from_this());

    if (mWriteBuffer.size() != 0) {
        boost::asio::async_write(mSocket, mWriteBuffer.data(),
                [this, self](boost::system::error_code ec, std::size_t length)
                {
                    if (!ec)
                    {
                        mWriteBuffer.consume(length);
                    }
                });
    }
}

void CommAsioAdminClient::setup(Link * connection)
{
    // Create the server side negotiator

    m_link = connection;

    auto self(shared_from_this());
    mNegotiateTimer.async_wait([this, self](const boost::system::error_code& ec)
    {
        //If the negotiator still exists after the deadline it means that the negotation hasn't
        //completed yet; we'll consider that a "timeout".
        if (m_negotiate != nullptr) {
            mSocket.close();
        }
    });


    m_negotiate->poll(false);

    negotiate_write();
    negotiate_read();
}

int CommAsioAdminClient::negotiate()
{
    // poll and check if negotiation is complete
    m_negotiate->poll();

    if (m_negotiate->getState() == Atlas::Negotiate::IN_PROGRESS) {
        return 0;
    }

    // Check if negotiation failed
    if (m_negotiate->getState() == Atlas::Negotiate::FAILED) {
        log(NOTICE, "Failed to negotiate");
        return -1;
    }
    // Negotiation was successful

    // Get the codec that negotiation established
    m_codec = m_negotiate->getCodec(*this);

    // Acceptor is now finished with
    delete m_negotiate;
    m_negotiate = 0;

    if (m_codec == nullptr) {
        log(NOTICE, "Could not create codec during negotiation.");
        return -1;
    }
    // Create a new encoder to send high level objects to the codec
    m_encoder = new Atlas::Objects::ObjectsEncoder(*m_codec);

    assert(m_link != 0);
    m_link->setEncoder(m_encoder);

    // This should always be sent at the beginning of a session
    m_codec->streamBegin();


    return 0;
}

bool CommAsioAdminClient::timeout()
{
    return false;
}

int CommAsioAdminClient::operation(
        const Atlas::Objects::Operation::RootOperation & op)
{
    assert(m_link != 0);
    m_link->externalOperation(op, *m_link);
    return 0;
}

void CommAsioAdminClient::dispatch()
{
    DispatchQueue::const_iterator Iend = m_opQueue.end();
    for (DispatchQueue::const_iterator I = m_opQueue.begin(); I != Iend; ++I) {
        if (operation(*I) != 0) {
            return;
        }
    }
    m_opQueue.clear();
}

void CommAsioAdminClient::objectArrived(const Atlas::Objects::Root & obj)
{
    Atlas::Objects::Operation::RootOperation op =
            Atlas::Objects::smart_dynamic_cast<
                    Atlas::Objects::Operation::RootOperation>(obj);
    if (!op.isValid()) {
        const std::list<std::string> & parents = obj->getParents();
        if (parents.empty()) {
            log(ERROR, String::compose("Object of type \"%1\" with no parent "
                    "arrived from client", obj->getObjtype()));
        } else {
            log(ERROR,
                    String::compose("Object of type \"%1\" with parent "
                            "\"%2\" arrived from client", obj->getObjtype(),
                            obj->getParents().front()));
        }
        return;
    }
    m_opQueue.push_back(op);
}

void CommAsioAdminClient::idle(time_t t)
{
//    if (m_negotiate != 0) {
//        if ((t - m_connectTime) > 10) {
//            log(NOTICE, "Client disconnected because of negotiation timeout.");
//            m_clientIos.shutdown();
//        }
//    }
}

int CommAsioAdminClient::read()
{
    return 0;
}

int CommAsioAdminClient::send(
        const Atlas::Objects::Operation::RootOperation & op)
{
    if (!isOpen()) {
        log(ERROR, "Writing to closed client");
        return -1;
    }
    assert(m_encoder);
//    if (m_clientIos.fail()) {
//        return -1;
//    }
//    if (m_encoder == 0) {
//        log(ERROR, "Encoder not initialized");
//        return -1;
//    }
    m_encoder->streamObjectsMessage(op);
    return flush();
}

int CommAsioAdminClient::getFd() const
{
    return 0;
}

bool CommAsioAdminClient::isOpen() const
{
    return mSocket.is_open();
}

bool CommAsioAdminClient::eof()
{
    return !mSocket.is_open();
}

void CommAsioAdminClient::disconnect()
{
    mSocket.close();
}

int CommAsioAdminClient::flush()
{
    do_write();
    return 0;
}

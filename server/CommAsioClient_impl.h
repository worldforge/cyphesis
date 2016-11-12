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

#ifndef COMMASIOCLIENT_IMPL_H_
#define COMMASIOCLIENT_IMPL_H_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "common/log.h"
#include "common/compose.hpp"
#include "common/debug.h"

#include "CommAsioClient.h"

#include <Atlas/Objects/Encoder.h>
#include <Atlas/Objects/RootOperation.h>
#include <Atlas/Objects/SmartPtr.h>
#include <Atlas/Net/Stream.h>

#include <Atlas/Codecs/Bach.h>

#include <sstream>
#include <iostream>

static const bool comm_asio_client_debug_flag = false;

template<class ProtocolT>
CommAsioClient<ProtocolT>::CommAsioClient(const std::string & name,
        boost::asio::io_service& io_service) :
        CommSocket(io_service), mSocket(io_service), mWriteBuffer(
                new boost::asio::streambuf()), mSendBuffer(new boost::asio::streambuf()), mInStream(&mReadBuffer),
                mOutStream(mWriteBuffer), mNegotiateTimer(io_service, boost::posix_time::seconds(1)), mIsSending(false), mShouldSend(false),
                m_codec(nullptr), m_encoder(nullptr), m_negotiate(nullptr), m_link(nullptr), mName(name)
{
}

template<class ProtocolT>
CommAsioClient<ProtocolT>::~CommAsioClient()
{
    delete m_link;
    delete m_negotiate;
    delete m_encoder;
    delete m_codec;
    delete mWriteBuffer;
    delete mSendBuffer;
    try {
        mSocket.shutdown(ProtocolT::socket::shutdown_both);
    } catch (const std::exception& e) {
    }
    try {
        mSocket.close();
    } catch (const std::exception& e) {
    }
}

template<class ProtocolT>
typename ProtocolT::socket& CommAsioClient<ProtocolT>::getSocket()
{
    return mSocket;
}

template<class ProtocolT>
void CommAsioClient<ProtocolT>::do_read()
{
    auto self(this->shared_from_this());
    mSocket.async_read_some(mReadBuffer.prepare(read_buffer_size),
            [this, self](boost::system::error_code ec, std::size_t length)
            {
                if (!ec)
                {
                    mReadBuffer.commit(length);
                    m_codec->poll();
                    this->dispatch();
                    //By calling do_read again we make sure that the instance
                    //doesn't go out of scope ("shared_from this"). As soon as that
                    //doesn't happen, and there's no write in progress, the instance
                    //will be deleted since there's no more references to it.
                    this->do_read();
                } else {
                    std::stringstream ss;
                    ss << "Error when reading from socket: " << ec;
                    log(WARNING, ss.str());
                }
            });
}

template<class ProtocolT>
void CommAsioClient<ProtocolT>::write()
{
    if (mIsSending) {
        //We're already sending in the background.
        //Make that we should send again once we've completed sending.
//        std::cerr << "Delaying send." << std::endl << std::flush;
//        if (!mShouldSend) {
//            start = boost::posix_time::microsec_clock::local_time();
//        }
        mShouldSend = true;
        return;
    }

    mShouldSend = false;

    if (mWriteBuffer->size() != 0) {
        //We'll use a self reference to make sure that the client isn't deleted while sending.
        auto self(this->shared_from_this());
        //Swap places between writing buffer and sending buffer, and attach new writing to the out stream.
        std::swap(mWriteBuffer, mSendBuffer);
        mOutStream.rdbuf(mWriteBuffer);
        mIsSending = true;

        boost::asio::async_write(mSocket, *mSendBuffer,
                [this, self](boost::system::error_code ec, std::size_t length)
                {
                    mIsSending = false;
                    if (!ec)
                    {
                        mSendBuffer->consume(length);
                        //Is there data queued for transmission which we should send right away?
                        if (mShouldSend) {
//                            auto diff = boost::posix_time::microsec_clock::local_time() - start;
//                            std::cerr << "Sending delayed "<< diff.total_microseconds() <<" microseconds." << std::endl << std::flush;
                            this->write();
                        }
                    } else {
                        std::stringstream ss;
                        ss << "Error when writing to socket: " << ec;
                        log(WARNING, ss.str());
                    }
                });
    }
}

template<class ProtocolT>
void CommAsioClient<ProtocolT>::negotiate_read()
{
    auto self(this->shared_from_this());
    mSocket.async_read_some(mReadBuffer.prepare(read_buffer_size),
            [this, self](boost::system::error_code ec, std::size_t length)
            {
                if (!ec)
                {
                    mReadBuffer.commit(length);
                    if (length > 0) {
                        int negotiateResult = this->negotiate();
                        if (negotiateResult < 0) {
                            //this should remove any shared references and delete this instance
                            return;
                        }
                    }

                    //If the m_negotiate instance is removed we're done with negotiation and should start the main loop.
                    if (m_negotiate == nullptr) {
                        this->write();
                        this->do_read();
                    } else {
                        this->negotiate_write();
                        this->negotiate_read();
                    }
                }
            });
}

template<class ProtocolT>
void CommAsioClient<ProtocolT>::negotiate_write()
{
    auto self(this->shared_from_this());

    if (mWriteBuffer->size() != 0) {
        boost::asio::async_write(mSocket, mWriteBuffer->data(),
                [this, self](boost::system::error_code ec, std::size_t length)
                {
                    if (!ec)
                    {
                        mWriteBuffer->consume(length);
                    }
                });
    }
}

template<class ProtocolT>
void CommAsioClient<ProtocolT>::startAccept(Link * connection)
{
    // Create the server side negotiator
    m_negotiate = new Atlas::Net::StreamAccept("cyphesis " + mName, mInStream, mOutStream);

    m_link = connection;

    startNegotiation();
}

template<class ProtocolT>
void CommAsioClient<ProtocolT>::startConnect(Link * connection)
{
    // Create the client side negotiator
    m_negotiate = new Atlas::Net::StreamConnect("cyphesis " + mName, mInStream, mOutStream);

    m_link = connection;

    startNegotiation();
}

template<class ProtocolT>
void CommAsioClient<ProtocolT>::startNegotiation()
{

    auto self(this->shared_from_this());
    mNegotiateTimer.expires_from_now(boost::posix_time::seconds(10));
    mNegotiateTimer.async_wait([this, self](const boost::system::error_code& ec)
    {
        //If the negotiator still exists after the deadline it means that the negotation hasn't
        //completed yet; we'll consider that a "timeout".
            if (m_negotiate != nullptr) {
                log(NOTICE, "Client disconnected because of negotiation timeout.");
                mSocket.close();
            }
        });

    m_negotiate->poll(false);

    negotiate_write();
    negotiate_read();
}

template<class ProtocolT>
int CommAsioClient<ProtocolT>::negotiate()
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

template<class ProtocolT>
int CommAsioClient<ProtocolT>::operation(
        const Atlas::Objects::Operation::RootOperation & op)
{
    assert(m_link != 0);
    m_link->externalOperation(op, *m_link);
    return 0;
}

template<class ProtocolT>
void CommAsioClient<ProtocolT>::dispatch()
{
    DispatchQueue::const_iterator Iend = m_opQueue.end();
    for (DispatchQueue::const_iterator I = m_opQueue.begin(); I != Iend; ++I) {
        if (operation(*I) != 0) {
            return;
        }
    }
    m_opQueue.clear();
}

template<class ProtocolT>
void CommAsioClient<ProtocolT>::objectArrived(const Atlas::Objects::Root & obj)
{
    if (comm_asio_client_debug_flag) {
        std::stringstream debugStream;

        Atlas::Codecs::Bach debugCodec(debugStream, debugStream, *this /*dummy*/);
        Atlas::Objects::ObjectsEncoder debugEncoder(debugCodec);
        debugEncoder.streamObjectsMessage(obj);
        debugStream << std::flush;

        std::cerr << "received: " << debugStream.str() << std::endl << std::flush;
    }

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

template<class ProtocolT>
int CommAsioClient<ProtocolT>::send(
        const Atlas::Objects::Operation::RootOperation & op)
{
    if (!mSocket.is_open()) {
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

    if (comm_asio_client_debug_flag) {
        std::stringstream debugStream;

        Atlas::Codecs::Bach debugCodec(debugStream, debugStream, *this /*dummy*/);
        Atlas::Objects::ObjectsEncoder debugEncoder(debugCodec);
        debugEncoder.streamObjectsMessage(op);
        debugStream << std::flush;

        std::cerr << "sending: " << debugStream.str() << std::endl << std::flush;
    }

    m_encoder->streamObjectsMessage(op);

    return flush();
}

template<class ProtocolT>
void CommAsioClient<ProtocolT>::disconnect()
{
    mSocket.close();
}

template<class ProtocolT>
int CommAsioClient<ProtocolT>::flush()
{
    write();
    return 0;
}

#endif /* COMMASIOCLIENT_IMPL_H_ */

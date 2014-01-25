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

#ifndef COMMASIOADMINCLIENT_H_
#define COMMASIOADMINCLIENT_H_

#include "common/CommSocket.h"
#include "CommServer.h"

#include "Idle.h"
#include "common/Link.h"

#include <Atlas/Objects/Decoder.h>
#include <Atlas/Objects/ObjectsFwd.h>
#include <Atlas/Codec.h>
#include <Atlas/Negotiate.h>

#include <boost/asio.hpp>
#include <boost/asio/buffer.hpp>

#include <memory>
#include <sstream>
#include <deque>

class CommAsioAdminClient: public CommSocket,
        public Atlas::Objects::ObjectsDecoder,
        public std::enable_shared_from_this<CommAsioAdminClient>
{
    public:
        CommAsioAdminClient(CommServer& commServer, const std::string & name,
                boost::asio::local::stream_protocol::socket socket);
        virtual ~CommAsioAdminClient();

        void do_read();

        void do_write();
    public:

        void setup(Link * connection);
        int send(const Atlas::Objects::Operation::RootOperation &);

    public:
        /// \brief STL deque of pointers to operation objects.
        typedef std::deque<Atlas::Objects::Operation::RootOperation> DispatchQueue;

        virtual int getFd() const;

        virtual bool isOpen() const;

        virtual bool eof();

        virtual int read();

        virtual void dispatch();

        virtual void disconnect();

        virtual int flush();

    protected:
        boost::asio::local::stream_protocol::socket mSocket;

        boost::asio::streambuf mReadBuffer;
        boost::asio::streambuf mWriteBuffer;
        std::iostream mStream;
        boost::asio::deadline_timer mNegotiateTimer;

        enum { read_buffer_size = 16384};


/// \brief Queue of operations that have been decoded by not dispatched.
        DispatchQueue m_opQueue;
        /// \brief Atlas codec that handles encoding and decoding traffic.
        Atlas::Codec * m_codec;
        /// \brief high level encoder passes data to the codec for transmission.
        Atlas::Objects::ObjectsEncoder * m_encoder;
        /// \brief Atlas negotiator for handling codec negotiation.
        Atlas::Negotiate * m_negotiate;
        /// \brief Server side object for handling connection level operations.
        Link * m_link;
        /// \brief Time connection was opened
        time_t m_connectTime;

        /// \brief Handle socket data related to codec negotiation.
        int negotiate();

        void negotiate_read();

        void negotiate_write();


        /// \brief Add an operation to the queue.
        template<class OpType>
        void queue(const OpType &);

        bool timeout();
        int operation(const Atlas::Objects::Operation::RootOperation &);

        virtual void objectArrived(const Atlas::Objects::Root & obj);

        virtual void idle(time_t t);
};

#endif /* COMMASIOADMINCLIENT_H_ */

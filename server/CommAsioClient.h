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

#ifndef COMMASIOCLIENT_H_
#define COMMASIOCLIENT_H_

#include "common/Link.h"
#include "common/CommSocket.h"

#include <Atlas/Objects/Decoder.h>
#include <Atlas/Objects/ObjectsFwd.h>
#include <Atlas/Codec.h>
#include <Atlas/Negotiate.h>

#include <boost/asio.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/steady_timer.hpp>

#include <memory>
#include <sstream>
#include <deque>

template<typename ProtocolT>
class CommAsioClient: public Atlas::Objects::ObjectsDecoder,
        public CommSocket,
        public std::enable_shared_from_this<CommAsioClient<ProtocolT> >
{
    public:
        CommAsioClient(const std::string & name,
                boost::asio::io_service& io_service);
        virtual ~CommAsioClient();

        typename ProtocolT::socket& getSocket();

        void startAccept(Link * connection);
        void startConnect(Link * connection);
        int send(const Atlas::Objects::Operation::RootOperation &);

        /// \brief STL deque of pointers to operation objects.
        typedef std::deque<Atlas::Objects::Operation::RootOperation> DispatchQueue;

        virtual void disconnect();

        virtual int flush();

    protected:
        typename ProtocolT::socket mSocket;

        boost::asio::streambuf mReadBuffer;
        boost::asio::streambuf mWriteBuffer;
        std::iostream mStream;
        boost::asio::steady_timer mNegotiateTimer;

        enum
        {
            read_buffer_size = 16384
        };

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

        const std::string mName;

        void do_read();

        void do_write();

        void dispatch();

        void startNegotiation();

        /// \brief Handle socket data related to codec negotiation.
        int negotiate();

        void negotiate_read();

        void negotiate_write();

        int operation(const Atlas::Objects::Operation::RootOperation &);

        virtual void objectArrived(const Atlas::Objects::Root & obj);
};

#endif /* COMMASIOCLIENT_H_ */

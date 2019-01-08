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
#include <boost/asio/deadline_timer.hpp>

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
        ~CommAsioClient() override;

        typename ProtocolT::socket& getSocket();

        void startAccept(Link * connection);
        void startConnect(Link * connection);
        int send(const Atlas::Objects::Operation::RootOperation &);

        /// \brief STL deque of pointers to operation objects.
        typedef std::deque<Atlas::Objects::Operation::RootOperation> DispatchQueue;

        void disconnect() override;

        int flush() override;

        /**
         * Controls how many ops should be emitted per call to dispatch.
         */
        int mMaxOpsPerDispatch;

    protected:
        typename ProtocolT::socket mSocket;

        boost::asio::streambuf mReadBuffer;
        /**
         * A buffer into which any outgoing data is written. This is always attached to mOutStream,
         * which basically means that any Atlas op being serialized for outgoing data is written to
         * this buffer.
         */
        boost::asio::streambuf* mWriteBuffer;

        /**
         * A buffer which is used when data is being sent asynchronously.
         */
        boost::asio::streambuf* mSendBuffer;

        /**
         * The stream onto which data is received.
         */
        std::istream mInStream;

        /**
         * The stream onto which data is written when it's to be sent.
         * Note that the actual data is accessed through mWriteBuffer.
         */
        std::ostream mOutStream;

        boost::asio::deadline_timer mNegotiateTimer;

        /**
         * True if we're currently are sending/writing. No other write operations to the socket is allowed.
         */
        bool mIsSending;

        /**
         * True if we have pending data which we should send as soon as a pending async write operation has
         * completed.
         */
        bool mShouldSend;

        enum
        {
            /**
             * Arbitrary size of the read buffer.
             */
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

        void write();

        void dispatch();

        void startNegotiation();

        /// \brief Handle socket data related to codec negotiation.
        int negotiate();

        void negotiate_read();

        void negotiate_write();

        int operation(const Atlas::Objects::Operation::RootOperation &);

        void objectArrived(const Atlas::Objects::Root & obj) override;
};

#endif /* COMMASIOCLIENT_H_ */

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

#include "common/asio.h"
#include "common/asio.h"
#include "common/asio.h"
#include <boost/asio/buffer.hpp>
#include <boost/asio/steady_timer.hpp>

#include <memory>
#include <sstream>
#include <deque>

template<typename ProtocolT>
class CommAsioClient : public Atlas::Objects::ObjectsDecoder,
                       public CommSocket,
                       public std::enable_shared_from_this<CommAsioClient<ProtocolT> >
{
    public:
        CommAsioClient(std::string name,
                       boost::asio::io_context& io_context,
                       const Atlas::Objects::Factories& factories);

        ~CommAsioClient() override;

        typename ProtocolT::socket& getSocket();

        void startAccept(std::unique_ptr<Link> connection);

        void startConnect(std::unique_ptr<Link> connection);

        int send(const Atlas::Objects::Operation::RootOperation&);

        /// \brief STL deque of pointers to operation objects.
        typedef std::deque<Atlas::Objects::Operation::RootOperation> DispatchQueue;

        void disconnect() override;

        int flush() override;

        /**
         * Sets whether or not the sockets should be automatically flushed after each call to "send".
         * By default this is off, which means that calling code needs to make sure to flush the sockets
         * at suitable intervals.
         * @param autoFlush
         */
        void setAutoFlush(bool autoFlush) {
            mAutoFlush = autoFlush;
        }

    protected:
        typename ProtocolT::socket mSocket;

        boost::asio::streambuf mReadBuffer;
        /**
         * A buffer into which any outgoing data is written. This is always attached to mOutStream,
         * which basically means that any Atlas op being serialized for outgoing data is written to
         * this buffer.
         */
        std::unique_ptr<boost::asio::streambuf> mWriteBuffer;

        /**
         * A buffer which is used when data is being sent asynchronously.
         */
        std::unique_ptr<boost::asio::streambuf> mSendBuffer;

        /**
         * The stream onto which data is received.
         */
        std::istream mInStream;

        /**
         * The stream onto which data is written when it's to be sent.
         * Note that the actual data is accessed through mWriteBuffer.
         */
        std::ostream mOutStream;

        boost::asio::steady_timer mNegotiateTimer;

        /**
         * True if we're currently are sending/writing. No other write operations to the socket is allowed.
         */
        bool mIsSending;

        /**
         * True if we have pending data which we should send as soon as a pending async write operation has
         * completed.
         */
        bool mShouldSend;

        /**
         * If set to "true", the sockets are all flushed automatically whenever "send" is called.
         * By default it's off, meaning that it's up to calling code to make sure that the sockets are flushed at
         * suitable intervals.
         */
        bool mAutoFlush;

        enum
        {
            /**
             * Arbitrary size of the read buffer.
             */
                read_buffer_size = 16384
        };

        /// \brief Atlas codec that handles encoding and decoding traffic.
        std::unique_ptr<Atlas::Codec> m_codec;
        /// \brief high level encoder passes data to the codec for transmission.
        std::unique_ptr<Atlas::Objects::ObjectsEncoder> m_encoder;
        /// \brief Atlas negotiator for handling codec negotiation.
        std::unique_ptr<Atlas::Negotiate> m_negotiate;
        /// \brief Server side object for handling connection level operations.
        std::unique_ptr<Link> m_link;

        const std::string mName;

        void do_read();

        void write();

        void startNegotiation();

        /// \brief Handle socket data related to codec negotiation.
        int negotiate();

        void negotiate_read();

        void negotiate_write();

        void externalOperation(Atlas::Objects::Operation::RootOperation);

        void objectArrived(Atlas::Objects::Root obj) override;
};

#endif /* COMMASIOCLIENT_H_ */

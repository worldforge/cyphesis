// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2008 Alistair Riddoch
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

#ifndef SERVER_COMM_HTTP_CLIENT_H
#define SERVER_COMM_HTTP_CLIENT_H

#include "common/asio.h"

#include <list>
#include <string>
#include <memory>
#include <Atlas/Objects/Factories.h>

struct HttpRequestProcessor
{
    virtual void processQuery(std::ostream& body, const std::list<std::string>& headers) = 0;
};

/// \brief Handle an internet socket connected to a remote web browser.
/// \ingroup ServerSockets
class CommHttpClient : public std::enable_shared_from_this<CommHttpClient>
{
    protected:

        boost::asio::ip::tcp::socket mSocket;

        boost::asio::streambuf mBuffer;
        std::iostream mStream;

        std::string m_incoming;
        std::list<std::string> m_headers;

        HttpRequestProcessor& m_requestProcessor;

        void do_read();

        bool read();

        void write();

    public:
        CommHttpClient(const std::string& name,
                       boost::asio::io_context& io_context,
                       HttpRequestProcessor& requestProcessor);

        virtual ~CommHttpClient();

        void serveRequest();

        boost::asio::ip::tcp::socket& getSocket();

};

#endif // SERVER_COMM_HTTP_CLIENT_H

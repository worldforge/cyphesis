// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2004 Alistair Riddoch
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


#ifndef SERVER_COMM_PEER_H
#define SERVER_COMM_PEER_H

#include "common/CommAsioClient.h"

#include <sigc++/signal.h>

/// \brief Handle an internet socket connected to a remote peer server.
/// \ingroup ServerSockets
class CommPeer : public CommAsioClient<boost::asio::ip::tcp>
{
    public:
        CommPeer(const std::string& name,
                 boost::asio::io_context& io_context,
                 Atlas::Objects::Factories& factories);

        ~CommPeer() override;

        void connect(const std::string&, int);

        void connect(const boost::asio::ip::tcp::endpoint&);

        void setup(std::unique_ptr<Link>);

        sigc::signal<void()> connected;
        sigc::signal<void()> failed;

    protected:
        boost::asio::steady_timer m_auth_timer;
        std::chrono::steady_clock::time_point m_start_auth;

        void checkAuth();

};

#endif // SERVER_COMM_PEER_H

// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000,2001 Alistair Riddoch
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
#endif

#include "ObserverClient.h"

#include "CreatorClient.h"

#include "common/sockets.h"

#include <thread>

ObserverClient::ObserverClient(boost::asio::io_context& io_context, Atlas::Objects::Factories& factories, TypeStore& typeStore)
: BaseClient(io_context, factories, typeStore)
{
}

ObserverClient::~ObserverClient() = default;

int ObserverClient::setup(const std::string & account,
                          const std::string & password,
                          const std::string & avatar)
{
    if (connectLocal(client_socket_name) != 0) {
        std::cerr << "WARNING: Could not make secure connection to:"
                  << std::endl << "    " << client_socket_name
                  << std::endl;
        if (connect(m_server, client_port_num) != 0) {
            std::cerr << "WARNING: Could not make non-secure connection to: "
                      << m_server << " port " << client_port_num
                      << std::endl;
            return -1;
        }

        if (password.empty()) {
            std::cerr << "WARNING: Made non secure connection to the server."
                      << std::endl
                      << "WARNING: Attempting to login with no password."
                      << std::endl << std::flush;
        }
    }

    if (account.empty()) {
        m_player = createSystemAccount();
    } else {
        m_player = createAccount(account, password);
    }
    if (!m_player.isValid()) {
        return -1;
    }
    return 0;
}

int ObserverClient::teardown()
{
    m_connection.cleanDisconnect();
    return 0;
}

void ObserverClient::idle()
{
    std::this_thread::sleep_for(std::chrono::microseconds(100));
}

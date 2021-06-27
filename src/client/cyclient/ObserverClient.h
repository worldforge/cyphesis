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


#ifndef CLIENT_OBSERVER_CLIENT_H
#define CLIENT_OBSERVER_CLIENT_H

#include "BaseClient.h"
#include "ClientConnection.h"

/// \brief Class for the core of a client that connects to a server,
/// initialises the world and observes what occurs.
///
/// In fact the observation code has not yet been implemented, and this
/// client is mainly used to handle world initialisation.
class ObserverClient : public BaseClient {
  protected:
    std::string m_server;
  public:
    explicit ObserverClient(boost::asio::io_context& io_context, Atlas::Objects::Factories& factories, TypeStore& typeStore);
    ~ObserverClient() override;

    void setServer(const std::string & server) {
        m_server = server;
    }

    int setup(const std::string & account = "",
              const std::string & password = "",
              const std::string & avatar = "creator");
    int teardown();
    void idle() override;
};

#endif // CLIENT_OBSERVER_CLIENT_H

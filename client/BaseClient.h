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

#ifndef CLIENT_COMM_CLIENT_H
#define CLIENT_COMM_CLIENT_H

#include "ClientConnection.h"

class CreatorClient;

/// \brief Base class for classes that implement clients used to connect to a
/// cyphesis server
class BaseClient {
  protected:
    ClientConnection m_connection;
    CreatorClient * m_character;
    Atlas::Message::MapType m_player;
    std::string m_playerName;
    std::string m_playerId;

  public:
    BaseClient();
    virtual ~BaseClient();

    Atlas::Message::MapType createPlayer(const std::string & name,
                                         const std::string & pword);
    CreatorClient * createCharacter(const std::string & name);
    void handleNet();
    
    virtual void idle() = 0;

    int connectLocal(const std::string & socket = "") {
        return m_connection.connectLocal(socket);
    }

    int connect(const std::string & server = "localhost") {
        return m_connection.connect(server);
    }

    void send(const Atlas::Objects::Operation::RootOperation & op) {
        m_connection.send(op);
    }

    void run(const bool loop = true) {
        while (loop) {
            handleNet();
            idle();
        };
    }
};

#endif // CLIENT_COMM_CLIENT_H

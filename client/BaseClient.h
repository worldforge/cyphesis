// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef CLIENT_COMM_CLIENT_H
#define CLIENT_COMM_CLIENT_H

#include "ClientConnection.h"

#include "common/BaseEntity.h"

class CreatorClient;

class BaseClient {
  protected:
    ClientConnection m_connection;
    CreatorClient * m_character;
    MapType m_player;
    std::string m_playerName;
    std::string m_playerId;

  public:
    BaseClient();
    virtual ~BaseClient();

    MapType createPlayer(const std::string & name, const std::string & pword);
    CreatorClient * createCharacter(const std::string & name);
    void handleNet();
    
    virtual void idle() = 0;

    bool connectLocal(const std::string & socket = "") {
        return m_connection.connectLocal(socket);
    }

    bool connect(const std::string & server = "localhost") {
        return m_connection.connect(server);
    }

    void send(RootOperation & op) {
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

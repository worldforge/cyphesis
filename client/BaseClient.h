// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef CLIENT_COMM_CLIENT_H
#define CLIENT_COMM_CLIENT_H

#include "ClientConnection.h"

#include "common/BaseEntity.h"

class CreatorClient;

class CommClient : public BaseEntity {
  protected:
    ClientConnection connection;
    CreatorClient * character;
    Atlas::Message::Element::MapType player;
    std::string playerName;
    std::string playerId;

  public:
    CommClient();

    Atlas::Message::Element::MapType createPlayer(const std::string & name,
                                                 const std::string & pword);
    CreatorClient * createCharacter(const std::string & name);
    void handleNet();
    
    virtual void idle() = 0;

    bool connectLocal(const std::string & socket = "") {
        return connection.connectLocal(socket);
    }

    bool connect(const std::string & server = "localhost") {
        return connection.connect(server);
    }

    void send(Atlas::Objects::Operation::RootOperation & op) {
        connection.send(op);
    }

    void run(const bool loop = true) {
        while (loop) {
            handleNet();
            idle();
        };
    }
};

#endif // CLIENT_COMM_CLIENT_H

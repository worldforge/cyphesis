typedef int bad_type; // Remove this to get unset type reporting
// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include <common/BaseEntity.h>

#include "ClientConnection.h"

class Thing;

class CommClient : public BaseEntity {
  protected:
    // bad_type time;
    ClientConnection connection;
    BaseEntity * character;
    Atlas::Message::Object::MapType player;
    std::string playerName;

  public:
    CommClient();

    Atlas::Message::Object::MapType create_player(const std::string & name,
                                                  const std::string & pword);
    Thing * create_character(const std::string & name);
    void handle_net();
    
    virtual void idle() = 0;

    bool connect(const std::string & server = "localhost", int port=6767) {
        connection.connect(server);
    }
    void send(Atlas::Objects::Operation::RootOperation & op) {
        connection.send(op);
    }

    void run(const bool loop = true) {
        while (loop) {
            handle_net();
            idle();
        };
    }
};

// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef SERVER_RESTORATION_H
#define SERVER_RESTORATION_H

#include <string>

class ServerRouting;
class Database;

class Restoration {
  private:
    void restore(const std::string &, const std::string &);
  public:
    ServerRouting & server;
    Database & database;

    Restoration(ServerRouting & svr);

    void read();
};

#endif // SERVER_RESTORATION_H

// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef SERVER_RESTORATION_H
#define SERVER_RESTORATION_H

#include <string>
#include <map>

class ServerRouting;
class Database;
class Entity;

class Restoration {
  private:
    typedef Entity * (*restoreFunc)(int what_exactly);
    typedef std::map<std::string, restoreFunc> RestoreDict;
    RestoreDict m_restorers;
    
    void restore(const std::string &, const std::string &, bool create = true);
  public:
    ServerRouting & server;
    Database & database;

    Restoration(ServerRouting & svr);

    void read();
};

#endif // SERVER_RESTORATION_H

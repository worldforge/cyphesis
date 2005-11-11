// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef SERVER_RESTORATION_H
#define SERVER_RESTORATION_H

#include "common/Database.h"

#include <string>
#include <map>

class ServerRouting;
class Database;
class Entity;

/// \brief Class to orchestrate restoring the complete world state from
/// the database at startup
class Restoration {
  private:
    typedef Entity * (*restoreFunc)(const std::string &, long intId, DatabaseResult::const_iterator &);
    typedef std::map<std::string, restoreFunc> RestoreDict;
    RestoreDict m_restorers;
    
    void restore(const std::string &, const std::string &, Entity * loc = 0);
    void restoreChildren(Entity * loc);
  public:
    ServerRouting & server;
    Database & database;

    explicit Restoration(ServerRouting & svr);

    int read();
};

#endif // SERVER_RESTORATION_H

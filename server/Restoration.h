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

// $Id: Restoration.h,v 1.12 2006-10-26 00:48:15 alriddoch Exp $

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

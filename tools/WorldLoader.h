// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2009 Alistair Riddoch
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


#ifndef TOOLS_WORLD_LOADER_H
#define TOOLS_WORLD_LOADER_H

#include "common/ClientTask.h"

#include <Atlas/Objects/RootEntity.h>
#include <Atlas/Objects/SmartPtr.h>

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

#include <list>
#include <set>
#include <stack>
#include <fstream>

namespace Atlas {
  class Bridge;
  class Codec;
  class Encoder;
};

class StackEntry {
  public:
    const Atlas::Objects::Entity::RootEntity obj;
    std::string restored_id;
    std::list<std::string>::const_iterator child;

    StackEntry(const Atlas::Objects::Entity::RootEntity & o,
               const std::list<std::string>::const_iterator & c);
    explicit StackEntry(const Atlas::Objects::Entity::RootEntity & o);
};

class ObjectContext;

/// \brief Task class for dumping the world to a file
class WorldLoader : public ClientTask {
  protected:
    std::string m_account;
    boost::weak_ptr<ObjectContext> m_context;
    boost::shared_ptr<ObjectContext> m_agent_context;
    int m_lastSerialNo;
    int m_count;
    int m_updateCount;
    int m_createCount;
    std::map<std::string, Atlas::Objects::Root> m_objects;
    enum { INIT, UPDATING, CREATING, WALKING } m_state;

    std::deque<StackEntry> m_treeStack;
    std::set<std::string> m_newIds;

    void getEntity(const std::string & id, OpVector & res);
    void walk(OpVector & res);
    void create(const Atlas::Objects::Entity::RootEntity & obj,
                OpVector & res);

    void errorArrived(const Operation &, OpVector & res);
    void infoArrived(const Operation &, OpVector & res);
    void sightArrived(const Operation &, OpVector & res);
  public:
    explicit WorldLoader(const std::string & accountId,
                         const boost::shared_ptr<ObjectContext> & context);
    virtual ~WorldLoader();

    virtual void setup(const std::string & arg, OpVector & ret);
    virtual void operation(const Operation & op, OpVector & res);
};

#endif // TOOLS_WORLD_LOADER_H

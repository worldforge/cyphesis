// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef RULESETS_MEM_MAP_H
#define RULESETS_MEM_MAP_H

#include <common/types.h>

#include <string>
#include <list>

namespace Atlas {
  namespace Message {
    class Object;
  }
  namespace Objects { namespace Operation {
    class RootOperation;
  } }
}

class Entity;
class Script;
class Location;

class MemMap {
  private:
    friend class BaseMind;

    edict_t things;
    std::list<std::string> additionsById;
    std::list<std::string> addHooks;
    std::list<std::string> updateHooks;
    std::list<std::string> deleteHooks;
    Script *& script;

    inline Entity * addObject(Entity * object);
    inline void addContents(const Atlas::Message::Object::MapType & entmap);
  public:
    MemMap(Script *& s) : script(s)  { }

    inline Atlas::Objects::Operation::RootOperation * lookId();
    inline Entity * addId(const std::string & id);
    Entity * add(const Atlas::Message::Object & entity);
    inline void del(const std::string & id);
    inline Entity * get(const std::string & id);
    inline Entity * getAdd(const std::string & id);
    Entity * update(const Atlas::Message::Object & entity);
    elist_t findByType(const std::string & what);
    elist_t findByLocation(const Location & where, double radius);
    const Atlas::Message::Object asObject();
    void flushMap();

    std::list<std::string> & getAddHooks() { return addHooks; }
    std::list<std::string> & getUpdateHooks() { return updateHooks; }
    std::list<std::string> & getDeleteHooks() { return deleteHooks; }
};

#endif // RULESETS_MEM_MAP_H

// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef MEM_MAP_H
#define MEM_MAP_H

using Atlas::Message::Object;
using Atlas::Objects::Operation::RootOperation;

class Entity;
class Script;

class MemMap {
    friend class BaseMind;

    edict_t things;
    std::list<std::string> additionsById;
    Script *& script;

    inline Entity * addObject(Entity * object);
  public:
    MemMap(Script *& s) : script(s)  { }

    inline RootOperation * lookId();
    inline Entity * addId(const std::string & id);
    Entity * add(const Object & entity);
    inline void del(const std::string & id);
    inline Entity * get(const std::string & id);
    inline Entity * getAdd(const std::string & id);
    Entity * update(const Object & entity);
    elist_t findByType(const std::string & what);
    elist_t findByLocation(const Location & where, double radius);
    const Atlas::Message::Object asObject();

    void flushMap();

    std::list<std::string> addHooks;
    std::list<std::string> updateHooks;
    std::list<std::string> deleteHooks;
};

#endif // MEM_MAP_H

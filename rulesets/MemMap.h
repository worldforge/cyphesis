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

    Entity * addObject(Entity * object);
  public:
    MemMap(Script *& s) : script(s)  { }

    RootOperation * lookId();
    Entity * addId(const std::string & id);
    Entity * add(const Object & entity);
    void del(const std::string & id);
    Entity * get(const std::string & id);
    Entity * getAdd(const std::string & id);
    Entity * update(const Object & entity);
    elist_t findByType(const std::string & what);
    elist_t findByLocation(const Location & where, double radius);

    std::list<std::string> addHooks;
    std::list<std::string> updateHooks;
    std::list<std::string> deleteHooks;
};

#endif // MEM_MAP_H

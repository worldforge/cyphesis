// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#ifndef MEM_MAP_H
#define MEM_MAP_H

using Atlas::Message::Object;
using Atlas::Objects::Operation::RootOperation;

class Entity;
class Script;

class MemMap {
    friend class BaseMind;

    dict_t things;
    std::list<string> additionsById;
    Script *& script;

    Entity * addObject(Entity * object);
  public:
    MemMap(Script *& s) : script(s)  { }

    RootOperation * lookId();
    Entity * addId(const string & id);
    Entity * add(const Object & entity);
    void del(const string & id);
    Entity * get(const string & id);
    //Entity * __getitem__(const string & id) // operator[] perhaps?
    Entity * getAdd(const string & id);
    Entity * update(const Object & entity);
    list<Entity *> findByType(const string & what);
    list<Entity *> findByLocation(const Location & where, double radius);

    list<string> addHooks;
    list<string> updateHooks;
    list<string> deleteHooks;
};

#endif /* MEM_MAP_H */

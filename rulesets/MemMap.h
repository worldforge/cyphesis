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

    fdict_t things;
    std::list<string> additions_by_id;
    Script * script;

    Entity * add_object(Entity * object);
  public:
    MemMap() : script(NULL)  { }

    int set_script(Script * scrpt) {
        script = scrpt;
        return(scrpt == NULL ? -1 : 0);
    }

    // void script_hook(const string & method, Entity * object);

    RootOperation * look_id();
    Entity * add_id(const string & id);
    Entity * add(const Object & entity);
    void _delete(const string & id);
    Entity * get(const string & id);
    //Entity * __getitem__(const string & id) // operator[] perhaps?
    Entity * get_add(const string & id);
    Entity * update(const Object & entity);
    list<Entity *> find_by_type(const string & what);
    list<Entity *> find_by_location(const Location & where, double radius);

    list<string> add_hooks;
    list<string> update_hooks;
    list<string> delete_hooks;
    // add_hooks?             // Function pointers to trigger stuff
    // update_hooks?          // This one is required to sort out ownership
    // delete_hooks?          // This one is required to make sure we know
                              // something has gone.
};

#endif /* MEM_MAP_H */

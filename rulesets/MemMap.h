#ifndef MEM_MAP_H
#define MEM_MAP_H

using Atlas::Message::Object;
using Atlas::Objects::Operation::RootOperation;

#include <Python.h>

class Thing;

class MemMap {
    friend class BaseMind;

    fdict_t things;
    std::list<string> additions_by_id;
    PyObject * script_object;

    Thing * add_object(Thing * object);
  public:
    MemMap() : script_object(NULL)  { }

    int set_object(PyObject * obj) {
        script_object = obj;
        return(obj == NULL ? -1 : 0);
    }

    void script_hook(const string & method, Thing * object);

    RootOperation * look_id();
    Thing * add_id(const string & id);
    Thing * add(const Object & entity);
    void _delete(const string & id);
    Thing * get(const string & id);
    //bad_type __getitem__(bad_type id)
    Thing * get_add(const string & id);
    Thing * update(const Object & entity);
    list<Thing *> find_by_type(const string & what);
    list<Thing *> find_by_location(const Location & where, double radius);

    list<string> add_hooks;
    list<string> update_hooks;
    list<string> delete_hooks;
    // add_hooks?             // Function pointers to trigger stuff
    // update_hooks?          // This one is required to sort out ownership
    // delete_hooks?          // This one is required to make sure we know
                              // something has gone.
};

#endif /* MEM_MAP_H */

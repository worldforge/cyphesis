#ifndef MEM_MAP_H
#define MEM_MAP_H

using Atlas::Message::Object;
using Atlas::Objects::Operation::RootOperation;

class Thing;

class MemMap {
    std::map<string, Thing *> things;
    std::list<string> additions_by_id;
  public:
    MemMap() { }
    Thing * add_object(Thing * object);
    RootOperation * look_id();
    Thing * add_id(const string & id);
    Thing * add(const Object & entity);
    void _delete(const string & id);
    Thing * get(const string & id);
    //bad_type __getitem__(bad_type id)
    Thing * get_add(const string & id);
    Thing * update(const Object & entity);
    //bad_type find_by_location(bad_type location, bad_type radius=0.0)
    //bad_type find_by_type(bad_type what)

    // add_hooks?             // Function pointers to trigger stuff
    // update_hooks?          // This one is required to sort out ownership
    // delete_hooks?          // This one is required to make sure we know
                              // something has gone.
};

#endif /* MEM_MAP_H */

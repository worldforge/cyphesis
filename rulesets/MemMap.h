#ifndef MEM_MAP_H
#define MEM_MAP_H

typedef int bad_type; // Remove this to get unset type reporting

#define None 0 // Remove this to deal with un-initialied vars

class Thing;

class MemMap {
  public:
    MemMap() { }
    Thing * add_object(Thing * object);
    RootOperation * look_id();
    Thing * add_id(const string & id);
    Thing * add(Object & entity);
    void _delete(const string & id);
    Thing * get(const string & id);
    //bad_type __getitem__(bad_type id)
    Thing * get_add(const string & id);
    Thing * update(Object & entity);
    //bad_type find_by_location(bad_type location, bad_type radius=0.0)
    //bad_type find_by_type(bad_type what)

    std::map<string, Thing *> things;
    std::list<string> additions_by_id;
    // add_hooks?             // Function pointers to trigger stuff
    // update_hooks?          // This one is required to sort out ownership
    // delete_hooks?          // This one is required to make sure we know
                              // something has gone.
};

#endif /* MEM_MAP_H */

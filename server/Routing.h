#ifndef ROUTING_H
#define ROUTING_H

typedef int bad_type; // Remove this to get unset type reporting

#include <map>

#include "OOG_Thing.h"

class Routing : public OOG_Thing {
public:
    dict_t objects;
    fdict_t fobjects;
    cid_t next_id;

    Routing();
    virtual ~Routing() { }
    bad_type check_operation(bad_type op);
    virtual BaseEntity * add_object(BaseEntity * obj);
    BaseEntity * del_object(BaseEntity * obj);
    BaseEntity * get_object(cid_t id);
    BaseEntity * get_object(string fid);
    BaseEntity * find_object(cid_t id);
};

#endif /* ROUTING_H */

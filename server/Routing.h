#ifndef ROUTING_H
#define ROUTING_H

typedef int bad_type; // Remove this to get unset type reporting

#include <map>

#include "OOG_Thing.h"

class Routing : public OOG_Thing {
public:
    //dict_t objects;
    fdict_t fobjects;
    int next_id;

    Routing();
    virtual ~Routing() { }
    virtual BaseEntity * add_object(BaseEntity * obj);
    virtual void del_object(BaseEntity * obj);
    BaseEntity * get_object(const string & fid);
    BaseEntity * find_object(const string & fid);
};

#endif /* ROUTING_H */

#ifndef ROUTING_H
#define ROUTING_H

#include <map>

#include "OOG_Thing.h"

class Routing : public OOG_Thing {
public:
    fdict_t fobjects;

    Routing() { }
    ~Routing() { }

    BaseEntity * add_object(BaseEntity * obj) {
        fobjects[obj->fullid]=obj;
        return obj;
    }

    void del_object(BaseEntity * obj) {
        fobjects.erase(obj->fullid);
        delete obj;
    }

    BaseEntity * get_object(const string & fid) {
        return Routing::fobjects[fid];
    }

    BaseEntity * find_object(const string & fid) {
        return Routing::fobjects[fid];
    }
};

#endif /* ROUTING_H */

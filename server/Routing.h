#ifndef ROUTING_H
#define ROUTING_H

typedef int bad_type; // Remove this to get unset type reporting

#include <map>

#include "OOG_Thing.h"

typedef std::map<cid_t, BaseEntity *> dict_t;
typedef std::pair<cid_t, BaseEntity *> idpair_t;

class Routing : public OOG_Thing {
public:
    dict_t objects;

    Routing();
    bad_type check_operation(bad_type op);
    BaseEntity * add_object(BaseEntity * obj, bad_type ent);
    BaseEntity * del_object(BaseEntity * obj);
    BaseEntity * get_object(cid_t id);
    BaseEntity * find_object(cid_t id);
};

#endif /* ROUTING_H */

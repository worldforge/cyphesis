#include "Routing.h"


Routing::Routing()
{
    //Routing::base_init(kw);
}

bad_type Routing::check_operation(bad_type op)
{
    //if (op.id=="check") {
        return Routing::error(op,"Check operation not supported");
    //}
}

BaseEntity * Routing::add_object(BaseEntity * obj, bad_type ent=None)
{
    if (ent) {
        //obj=object_from_entity(obj,ent);
    }
    Routing::objects[obj->id]=obj;
    return obj;
}

BaseEntity * Routing::del_object(BaseEntity * obj)
{
    delete Routing::objects[obj->id];
}

BaseEntity * Routing::get_object(cid_t id)
{
    return Routing::objects[id];
}

BaseEntity * Routing::find_object(cid_t id)
{
    return Routing::objects[id];
}

#include <Atlas/Message/Object.h>
#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/Operation/Login.h>

#include "Routing.h"


Routing::Routing() : next_id(0)
{
    //Routing::base_init(kw);
}

bad_type Routing::check_operation(bad_type op)
{
    //if (op.id=="check") {
        //return Routing::error(op,"Check operation not supported");
    //}
    return None;
}

BaseEntity * Routing::add_object(BaseEntity * obj, bad_type ent=None)
{
    objects[obj->id]=obj;
    fobjects[obj->fullid]=obj;
    return obj;
}

BaseEntity * Routing::del_object(BaseEntity * obj)
{
    delete objects[obj->id];
    return(NULL);
    // Is the return type wrong here?
}

BaseEntity * Routing::get_object(cid_t id)
{
    return Routing::objects[id];
}

BaseEntity * Routing::get_object(string fid)
{
    return Routing::fobjects[fid];
}


BaseEntity * Routing::find_object(cid_t id)
{
    return Routing::objects[id];
}

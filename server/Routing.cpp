#include <Atlas/Message/Object.h>
#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/Operation/Login.h>

#include "Routing.h"


Routing::Routing() : next_id(0) { }

BaseEntity * Routing::add_object(BaseEntity * obj)
{
    //objects[obj->id]=obj;
    fobjects[obj->fullid]=obj;
    return obj;
}

void Routing::del_object(BaseEntity * obj)
{
    fobjects.erase(obj->fullid);
    delete obj;
}

//BaseEntity * Routing::get_object(cid_t id)
//{
    //return Routing::objects[id];
//}

BaseEntity * Routing::get_object(const string & fid)
{
    return Routing::fobjects[fid];
}

BaseEntity * Routing::find_object(const string & fid)
{
    return Routing::fobjects[fid];
}


//BaseEntity * Routing::find_object(cid_t id)
//{
    //return Routing::objects[id];
//}

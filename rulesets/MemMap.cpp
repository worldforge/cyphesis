#include <Atlas/Message/Object.h>
#include <Atlas/Objects/Operation/Login.h>
#include <Atlas/Objects/Operation/Look.h>

using Atlas::Objects::Operation::Look;

#include "Thing.h"
#include "MemMap.h"


Thing * MemMap::add_object(Thing * object)
{
    things[object->fullid] = object;
    //for (/*hook in MemMap::add_hooks*/) {
        //hook(object);
    //}
    return object;
}

RootOperation * MemMap::look_id()
{
    if (additions_by_id.size() != 0) {
        string id = additions_by_id.front();
        additions_by_id.pop_front();
        Look * l = new Look();
        *l = Look::Instantiate();
        Object::MapType m;
        m["id"] = Object(id);
        l->SetArgs(Object::ListType(1, Object(m)));
        return l;
    }
    return(NULL);
}

Thing * MemMap::add_id(const string & id)
{
    additions_by_id.push_back(id);
    Object::MapType m;
    m["id"] = Object(string(id));
    Object obj(m);
    return add(obj);
}

Thing * MemMap::add(const Object & entity)
{
    if (!entity.IsMap()) {
        return NULL;
    }
    Object::MapType entmap = entity.AsMap();
    if (entmap.find("id") == entmap.end()) {
        return NULL;
    }
    if (MemMap::get(entmap["id"].AsString())) {
        return MemMap::update(entity);
    }
    Thing * thing = new Thing;
    if (entmap.find("name") != entmap.end() && entmap["name"].IsString()) {
        thing->name = entmap["name"].AsString();
    }
    thing->merge(entmap);
    return add_object(thing);
}

void MemMap::_delete(const string & id)
{
    if (things.find(id) != things.end()) {
        Thing * obj = things[id];
        things.erase(id);
        //for (/*hook in MemMap::delete_hooks*/) {
            //hook(obj);
        //}
    }
}

Thing * MemMap::get(const string & id)
{
    if (things.find(id) != things.end()) {
        return things[id];
    }
    return(NULL);
}

#if 0 
bad_type MemMap::__getitem__(bad_type id)
{
    return MemMap::things[id];
}
#endif

Thing * MemMap::get_add(const string & id)
{
    Thing * obj = MemMap::get(id);
    if (obj != NULL) {
        return obj;
    }
    return add_id(id);
}

Thing * MemMap::update(const Object & entity)
{
    if (!entity.IsMap()) {
        return NULL;
    }
    Object::MapType entmap = entity.AsMap();
    if (entmap.find("id") == entmap.end()) {
        return NULL;
    }
    string & id = entmap["id"].AsString();
    if (things.find(id) != things.end()) {
        return add(entity);
    }
    Thing * thing = things["id"];
    // I am not sure what the deal is with all the "needTrueValue stuff
    // below yet. FIXME find out exactly what is required.
    if (entmap.find("name") != entmap.end() && entmap["name"].IsString()) {
        thing->name = entmap["name"].AsString();
    }
    thing->merge(entmap);
    //needTrueValue=["type","contains","instance","id","location","stamp"];
    //for (/*(key,value) in entity.__dict__.items()*/) {
        //if (value or not key in needTrueValue) {
            //setattr(obj,key,value);
        //}
    //}
    //for (/*hook in MemMap::update_hooks*/) {
        //hook(obj);
    //}
    return thing;
}

#if 0 
bad_type MemMap::find_by_location(bad_type location, bad_type radius=0.0)
{
    res=[];
    for (/*p in MemMap::things.values()*/) {
        if (p.location and location.parent==p.location.parent) {
            d=location.coordinates.distance(p.location.coordinates);
            if (d<=radius) {
                res.append(d,p);
            }
        }
    }
    res.sort();
    return res;
}

bad_type MemMap::find_by_type(bad_type what)
{
    res=[];
    for (/*thing in MemMap::things.values()*/) {
        if (thing.type!=[]) {
            if (thing.type[0]==what) {
                res.append(thing);
            }
        }
    }
    return res;
}
#endif

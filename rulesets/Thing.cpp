#include <Atlas/Message/Object.h>
#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/Operation/Login.h>
#include <Atlas/Objects/Operation/Create.h>
#include <Atlas/Objects/Operation/Sight.h>
#include <Atlas/Objects/Operation/Set.h>
#include <Atlas/Objects/Operation/Delete.h>
#include <Atlas/Objects/Operation/Move.h>

#include "Thing.h"
#include "Character.h"

#include <server/WorldRouter.h>

#include <common/const.h>


Thing::Thing() : description("Some Thing"), mode("birth"),
		status(1.0), weight(-1), age(0.0), is_character(0)
{
    name=string("Foo");
}

void Thing::addObject(Message::Object * obj)
{
    Message::Object::MapType & omap = obj->AsMap();
    omap["name"] = Message::Object(name);
    BaseEntity::addObject(obj);
}

RootOperation * Thing::send_world(RootOperation * msg)
{
    return world->message(*msg, this);
}

RootOperation * Thing::Operation(const Setup & op)
{
    RootOperation * tick = new Tick;
    tick->SetTo(fullid);
    return(tick);
}

RootOperation * Thing::Operation(const Tick & op)
{
    return(NULL);
}

RootOperation * Thing::Operation(const Create & op)
{
    const Message::Object::ListType & args=op.GetArgs();
    if (args.size() == 0) {
       return(NULL);
    }
    try {
        Message::Object::MapType ent = args.front().AsMap();
        if (ent.find("parents") == ent.end()) {
            return error(op, "Object to be created has no type");
        }
        Message::Object::ListType & parents = ent["parents"].AsList();
        string type = parents.front().AsString();
        Thing * obj = (Thing *)world->add_object(type,ent);
        if (!obj->location) {
            obj->location=location;
            obj->location.velocity=Vector3D(0,0,0);
        }
        //if (obj->location.parent->contains.find(obj->fullid) !=
                            //obj->location.parent->contains.end()) {
            //obj->location.parent->contains.append(obj);
        //}
        //log.debug(3,"Created: "+str(obj)+" now: "+str(Thing::world.objects));
        Create * c = new Create(op);
        list<Message::Object> args2(1,obj->asObject());
        c->SetArgs(args2);
        RootOperation * s = new Sight();
        list<Message::Object> args3(1,c->AsObject());
        s->SetArgs(args3);
        return(s);
    }
    catch (Message::WrongTypeException) {
        return error(op, "Malformed object to be created\n");
    }
    return(NULL);
}

RootOperation * Thing::Operation(const Delete & op)
{
    world->del_object(this);
    //log.debug(3,"Deleted: "+str(this)+" now: "+str(Thing::world.objects));
    RootOperation * sight = new Sight;
    list<Message::Object> args(1,op.AsObject());
    sight->SetArgs(args);
    return(sight);
}

RootOperation * Thing::Operation(const Move & op)
{
    const Message::Object::ListType & args=op.GetArgs();
    if (args.size() == 0) {
       return(NULL);
    }
    try {
        Message::Object::MapType ent = args.front().AsMap();
        if (ent.find("parent") == ent.end()) {
            return(error(op, "Move location has no parent"));
        }
        if (location.parent->fullid!=ent["parent"].AsString()) {
            //location.parent.contains.remove(this);
            //ent.location.parent.contains.append(this);
        }
        string parent=ent["parent"].AsString();
        if (world->server->id_dict.find(parent) == world->server->id_dict.end()) {
            return(error(op, "Move location parent invalid"));
        }
        location.parent=world->server->id_dict[parent];
        if (ent.find("pos") == ent.end()) {
            return(error(op, "Move location has no position"));
        }
        Message::Object::ListType vector = ent["pos"].AsList();
        if (vector.size()!=3) {
            return(error(op, "Move location pos is malformed"));
        }
        int x = vector.front().AsInt();
        vector.pop_front();
        int y = vector.front().AsInt();
        vector.pop_front();
        int z = vector.front().AsInt();
        location.coords = Vector3D(x, y, z);
        if (ent.find("velocity") == ent.end()) {
            return(error(op, "Move location has no"));
        }
        vector.clear();
        vector = ent["velocity"].AsList();
        if (vector.size()!=3) {
            return(error(op, "Move location pos is malformed"));
        }
        x = vector.front().AsInt();
        vector.pop_front();
        y = vector.front().AsInt();
        vector.pop_front();
        z = vector.front().AsInt();
        location.velocity = Vector3D(x, y, z);

        double speed_ratio;
        if (!(location.velocity)) {
            speed_ratio = 0.0;
        } else {
            speed_ratio = location.velocity.mag()/consts::base_velocity;
        }
        if (speed_ratio > 0.5) {
            mode = string("running");
        } else if (speed_ratio > 0.0) {
            mode = string("walking");
        } else {
            mode = string("standing");
        }
        ent["mode"] = Message::Object(mode);
        RootOperation * s = new Sight;
        Message::Object::ListType args2(1,op.AsObject());
        s->SetArgs(args2);
        return(s); //+ res + res2;
        // I think it might be wise to send a set indicating we have changed
        // modes
    }
    catch (Message::WrongTypeException) {
        return(error(op, "Malformed object to be moved\n"));
    }
    return(NULL);
}

RootOperation * Thing::Operation(const Set & op)
{
    //ent=op[0];
    //needTrueValue=["type","contains","instance","id","location","stamp"];
    //for (/*(key,value) in ent.__dict__.items()*/) {
        //if (not key in Thing::attributes) {
            //return Thing::error(op,"Illegal attribute in set_operation:"+key);
        //}
        //if (value or not key in needTrueValue) {
            //setattr(this,key,value);
        //}
    //}
    //opSight=Operation("sight",op);
    //if (Thing::status<0.0) {
        //; //oops, we stopped existing...;
        //opDestroy=Operation("delete",Entity(this),to=this);
        //return Message(opSight,opDestroy);
    //}
    //return opSight;
    return(NULL);
}

Thing * ThingFactory::new_thing(const string & type,const Message::Object & ent)
{
    if (!ent.IsMap()) {
         cout << "Entity is not a map" << endl << flush;
    }
    Message::Object::MapType entmap = ent.AsMap();
    Thing * thing = new Thing();
    if (type.size() != 0) {
        thing = new Character();
    } else {
        thing = new Thing();
    }
    if (entmap.find("name") != entmap.end() && entmap["name"].IsString()) {
        thing->name = entmap["name"].AsString();
    } else {
        cout << "Got no name" << endl << flush;
    }
    return(thing);
}

#include <Atlas/Message/Object.h>
#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/Operation/Login.h>

#include "Thing.h"

#include <server/WorldRouter.h>


Thing::Thing() : description("Some Thing"), mode("birth"),
		status(1.0), age(0.0)
{
    name=string("Foo");
}

void Thing::addObject(Message::Object * obj)
{
    Message::Object::MapType & omap = obj->AsMap();
    omap["name"] = Message::Object(name);
    BaseEntity::addObject(obj);
}

bad_type Thing::send_world(bad_type msg)
{
    //return world->message(msg, this);
}

bad_type Thing::setup_operation(bad_type op)
{
    //return Operation("tick",to=this);
    return None;
}

bad_type Thing::tick_operation(bad_type op)
{
    return None;
}

bad_type Thing::create_operation(bad_type op)
{
    //ent=op[0];
    //if (len(ent.type)!=1) {
        //return Thing::error(op,"Type field should contain exactly one type");
    //}
    //type=string.capitalize(ent.type[0]);
    //type_class=get_thing_class(type,"Thing");
    //obj=Thing::world.add_object(type_class,ent);
    //if (not obj.location) {
        //log.debug(3,"??????? no obj.location: "+str(Thing::id)+" "+str(obj.id));
        //if (hasattr(ent,"copy")) {
            //log.debug(3,str(ent.copy.id)+" "+str(ent.copy.location));
        //}
        //obj.location=Thing::location.copy();
        //obj.location.velocity=Vector3D(0,0,0);
    //}
    //if (not obj in obj.location.parent.contains) {
        //obj.location.parent.contains.append(obj);
    //}
    //log.debug(3,"Created: "+str(obj)+" now: "+str(Thing::world.objects));
    //op[0]=obj.as_entity();
    //return Operation("sight",op);
    return None;
}

bad_type Thing::delete_operation(bad_type op)
{
    //Thing::world.del_object(this);
    //log.debug(3,"Deleted: "+str(this)+" now: "+str(Thing::world.objects));
    //return Operation("sight",op);
}

bad_type Thing::move_operation(bad_type op)
{
    //ent=op[0];
    //if (Thing::location.parent!=ent.location.parent) {
        //Thing::location.parent.contains.remove(this);
        //ent.location.parent.contains.append(this);
    //}
    //Thing::location=ent.location;
    //if (type(Thing::location.velocity)==NoneType) {
        //speed_ratio = 0.0;
    //}
    //else {
        //speed_ratio = Thing::location.velocity.mag()/const.base_velocity;
    //}
    //if (speed_ratio > 0.5) {
        //Thing::mode = "running";
    //}
    //else if (speed_ratio > 0.0) {
        //Thing::mode = "walking";
    //}
    //else {
        //Thing::mode = "standing";
    //}
    //ent.mode = Thing::mode;
    //if (const.enable_ranges) {
        //log.debug(1,"="*60);
        //log.debug(1,"%s %s %s" % (this,Thing::location.parent.id,Thing::location.coordinates));
        //res = Thing::world.update_all_ranges(this);
        //res2 = Thing::world.collision(this);
    //}
    //return Operation("sight",op) ; //+ res + res2;
    return None;
}

bad_type Thing::set_operation(bad_type op)
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
    return None;
}

Thing * ThingFactory::new_thing(const string & type,const Message::Object & ent)
{
    if (!ent.IsMap()) {
         cout << "Entity is not a map" << endl << flush;
    }
    Message::Object::MapType entmap = ent.AsMap();
    Thing * thing = new Thing();
    if (entmap.find("name") != entmap.end() && entmap["name"].IsString()) {
        thing->name = entmap["name"].AsString();
    } else {
        cout << "Got no name" << endl << flush;
    }
    return(thing);
}

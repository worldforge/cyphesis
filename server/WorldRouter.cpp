#include <Atlas/Message/Object.h>
#include <Atlas/Net/Stream.h>
#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/Encoder.h>
#include <Atlas/Message/DecoderBase.h>


#include "WorldRouter.h"




WorldRouter::WorldRouter(ServerRouting * srvr) : server(srvr), next_id(0)
{
    //WorldRouter::base_init(kw);
    //WorldRouter::operation_queue=avl_tree();
    WorldRouter::real_time=time(NULL);
    WorldRouter::server->id_dict[id]=this;
    WorldRouter::objects[id]=this;
    //WorldRouter::illegal_thing =
                         //server.id_dict["illegal"]=
                         //objects["illegal"]=Thing(id="illegal",name="illegal");
    //WorldRouter::illegal_thing.deleted=1;
    //WorldTime tmp_date("612-1-1 08:57:00");
    //This structure is used to tell libatlas about stuff
    //world_info.time.s=tmp_date.seconds();
    //world_info.coordinates_conversion_class=Vector3D;
    //world_info.seconds2string=seconds2string;
    //world_info.string2DateTime=WorldTime;
}

cid_t WorldRouter::get_id(char * name)
{
    next_id++;
    return(next_id);
}

bad_type WorldRouter::add_object(BaseEntity * obj, bad_type ent=None)
{
    //if (ent) {
        //obj=object_from_entity(obj,ent);
    //}
    obj->id=WorldRouter::get_id(obj->name);
    server->id_dict[obj->id]=WorldRouter::objects[obj->id]=obj;
    if (!obj->location) {
        obj->location=Location(this, Vector3D(0,0,0));
    }
    if (!obj->location.parent) {
        obj->location.parent=this;
    }
    if (obj->location.parent==this /*&& contains[obj->id]*/) {
        // Nasty kludge
        contains.push_back(obj);
        contains.unique();
    }
    obj->world=this;
    //res=find_range(obj, "visible", const.sight_range);
    //message(res,self);
    //find_range(obj, "audible", const.hearing_range);
    if (obj->omnipresent) {
        omnipresent_list.push_back(obj);
    }
    //op=Operation("setup",to=obj);
    //op.time.sadd=-0.01;
    //WorldRouter::add_operation_to_queue(op,self);
    //return obj;
}

void WorldRouter::del_object(BaseEntity * obj)
{
    contains.remove(obj);
    omnipresent_list.remove(obj);
    perceptives.remove(obj);

    // This code used to replace the endtry in object with illegal_object
    // I don't know is this is necessary
    //cid_t tid=obj->id;
    //dict_t::const_iterator I;
    //if ((I=objects.find(tid))!=objects.end()) {
        //objects.erase(I);
    //}
    objects.erase(obj->id);
}

bad_type WorldRouter::is_object_deleted(BaseEntity * obj)
{
    // THis code used to check with object[obj->id] was illegal_thing
    // so I am not sure if this is the right way to do it.
    return (objects.find(obj->id)!=objects.end());
}

bad_type WorldRouter::message(bad_type msg, BaseEntity * obj)
{
    apply_to_operation(&WorldRouter::add_operation_to_queue,msg,obj);
}

BaseEntity * WorldRouter::get_operation_place(bad_type op)
{
#ifdef BLUE_MOON
    if (len(op)) {
        arg0=op[0];
        if (arg0.get_name()=="op") {
            if (len(arg0)) {
                ent=arg0[0];
            }
            else {
                return;
            }
        } else {
            ent=arg0;
        }
        if (objects.find(ent.id)!=object.end()) {
            return objects[ent.id];
        }
    }
    return(NULL);
#endif
}

bad_type WorldRouter::operation(bad_type op)
{
// THis is currently a whole world of shit I can't get into right now.
// procrastination is the name of the game here.
#ifdef BLUE_MOON
    if (op.to and op.to!='all') {
        if ((op.to!=WorldRouter::or op.id=="look") and not op.to.deleted) {
            WorldRouter::debug(op,"world.operation: "+`op.to`+" "+op.to.__class__.__name__);
            if (WorldRouter::queue_fp) {
                WorldRouter::queue_fp.write("operation: %i, to: %s\n" % (op.no,op.to.id));
                WorldRouter::queue_fp.flush();
            }
            if (op.to==self) {
                res=WorldRouter::look_operation(op);
            }
            else {
                res=op.to.operation(op);
            }
            if (op.id=="delete") {
                id=op.to.id;
                WorldRouter::server.id_dict[id]=WorldRouter::illegal_thing;
                op.to.destroy();
                op.to.id=id;
                op.to.deleted=1;
            }
            WorldRouter::message(res,op.to);
        }
    }
    else {
        save_to=op.to;
        op_place_obj = WorldRouter::get_operation_place(op);
        if not const.enable_ranges or \;
           op.to=='all' or \;
           op.id not in ["sight","disappear","appear","sound"] or \;
           not op_place_obj:;
            if (op.id=="sight" and op[0].id=="create") {
              op.to = WorldRouter::objects[op[0][0].id];
              WorldRouter::operation(op);
            }
            target_list = WorldRouter::perceptives.values();
        else {
            target_list = WorldRouter::omnipresent_list[:];
            if (op.id in ["sight","disappear","appear"]) {
                for (/*obj in op_place_obj->visible.values()*/) {
                    if (obj not in target_list) {
                        target_list.append(obj);
                    }
                }
            }
            else if (op.id=="sound") {
                for (/*obj in op_place_obj->audible.values()*/) {
                    if (obj not in target_list) {
                        target_list.append(obj);
                    }
                }
            }
            else {
                raise WorldException, "Why that op.id here? (%s)" % op.id;
            }
        }
        try {
            for (/*obj in target_list*/) {
                op.to=obj;
                WorldRouter::operation(op);
            }
        }
        finally:;
            op.to=save_to;
    }
#endif
}

bad_type WorldRouter::look_operation(bad_type op)
{
#if 0
    print("Adding " + op.from_.id + " to perceptives");
    WorldRouter::perceptives[op.from_.id]=op.from_;
    return BaseEntity.look_operation(self, op);
#endif
}

bad_type WorldRouter::print_queue(bad_type msg)
{
#if 0
    if (const.debug_level<3) {
        return "";
    }
    s=[];
    for (/*op in WorldRouter::operation_queue*/) {
        s.append(str(op.id)+" "+str(op.time.s)+" "+str(op.time.sadd)+" "+str(op.no));
    }
    return str(msg)+"\n"+string.join(s);
#endif
}

bad_type WorldRouter::add_operation_to_queue(bad_type op, BaseEntity * obj)
{
#if 0
    if (op.from_ == "cheat" and not init.security_flag) {
        op.from_ = op.to;
    }
    else {
        op.from_ = obj;
    }
    if (WorldRouter::queue_fp) {
        WorldRouter::queue_fp.write("add_operation_to_queue:\n"+str(op)+"\n");
        WorldRouter::queue_fp.flush();
    }
    WorldRouter::debug(op,"world.add_operation_to_queue");
    WorldRouter::operation_queue.insert(op);
    if (op.id=="error" and const.debug_level>=1) {
        log.inform("error operation in World.add_operation_to_queue",op);
    }
    log.debug(3,WorldRouter::print_queue("added!!"));
    log.debug(4,"",op);
#endif
}

bad_type WorldRouter::get_operation_from_queue()
{
#if 0
    if (not WorldRouter::operation_queue) {
        return None;
    }
    op=WorldRouter::operation_queue[0];
    if (op.time>world_info.time.s) {
        return None;
    }
    WorldRouter::operation_queue.remove(op);
    op.time.s=op.time.s+op.time.sadd;
    op.time.sadd=0.0;
    log.debug(3,WorldRouter::print_queue("removed!!"));
    if (WorldRouter::queue_fp) {
        WorldRouter::queue_fp.write("get_operation_from_queue:\n"+str(op)+"\n");
        WorldRouter::queue_fp.flush();
    }
    return op;
#endif
}

bad_type WorldRouter::find_range(BaseEntity * obj, bad_type attribute, bad_type range, bad_type generate_messages=0)
{
#if 0
    if (not hasattr(obj,"get_xyz")) {
        setattr(obj,attribute,{});
        return;
    }
    if (generate_messages) {
        res=Message();
    }
    coords=obj->get_xyz();
    d={};
    for (/*o in WorldRouter::objects.values()*/) {
        if (hasattr(o,"get_xyz")) {
            c=o.get_xyz();
            if (c.distance(coords)<=range and not d.has_key(o.id)) {
                d[o.id]=o;
                if (generate_messages) {
                    res.append(Operation("appear",Entity(o.id)));
                }
                try {
                    other_visible=getattr(o,attribute);
                }
                catch (AttributeError) {
                    other_visible={};
                    setattr(o,attribute,other_visible);
                }
                other_visible[obj->id]=obj;
            }
        }
    }
    setattr(obj,attribute,d);
    if (generate_messages) {
        return res;
    }
#endif
}

bad_type WorldRouter::update_all_ranges(BaseEntity * obj)
{
#if 0
    WorldRouter::update_range(obj, "audible", const.hearing_range);
    return WorldRouter::update_range(obj, "visible", const.sight_range, 1);
#endif
}

bad_type WorldRouter::update_range(BaseEntity * obj, bad_type attribute, bad_type range, bad_type generate_messages=0)
{
#if 0
    if (generate_messages) {
        res=Message();
    }
    coords=obj->get_xyz();
    d=getattr(obj,attribute);
    for (/*(k,v) in d.items()*/) {
        if (v.get_xyz().distance(coords)>range) {
            del d[k];
            d2 = getattr(v,attribute);
            del d2[obj->id];
            if (generate_messages) {
                res.append(Operation("disappear",Entity(k)));
            }
        }
    }
    res2=WorldRouter::find_range(obj,attribute,range,generate_messages);
    if (0 and attribute=="visible") {
        d=getattr(obj,attribute);
        log.debug(1,str(d.keys()));
        log.debug(1,str(range));
        dist_list=[];
        for (/*o in WorldRouter::objects.values()*/) {
            c=o.get_xyz();
            dist=c.distance(coords);
            if (o.location==None) {
                d_str="%s %s" % (o,o.location);
            }
            else {
                d_str="%s %s %s" % (o,o.location.parent,o.location.coordinates);
            }
            dist_list.append(dist,d_str);
            if (dist<=range and not d.has_key(o.id)) {
                foo;
            }
        }
        dist_list.sort();
        for (/*item in dist_list*/) {
            log.debug(1,`item`);
        }
    }
    if (generate_messages) {
        return res + res2;
    }
#endif
}

bad_type WorldRouter::collision(BaseEntity * obj)
{
#if 0
    res = Message();
    coords=obj->get_xyz();
    if (obj->location.parent==self) {
        d = getattr(obj,"audible",{});
        for (/*o in d.values()*/) {
            c=o.get_xyz();
            if (o!=obj and c.distance(coords)<=const.collision_range) {
                pass ; //CHEAT!: set here new location;
            }
        }
    }
    return res;
#endif
}

bad_type WorldRouter::execute_code(bad_type code)
{
#if 0
    exec(code);
#endif
}

bad_type WorldRouter::eval_code(bad_type code)
{
#if 0
    return eval(code);
#endif
}

bad_type WorldRouter::save(bad_type filename)
{
#if 0
    persistence.save_world(self, filename);
#endif
}

bad_type WorldRouter::load(bad_type filename)
{
#if 0
    persistence.load_world(self, filename);
#endif
}

bad_type WorldRouter::update_time()
{
#if 0
    new_time=time();
    world_info.time.s=world_info.time.s+\;
                       const.time_multiplier*(new_time-WorldRouter::real_time);
    WorldRouter::real_time=new_time;
#endif
}

bad_type WorldRouter::get_time()
{
#if 0
    return WorldTime(world_info.time.s);
#endif
}

bad_type WorldRouter::idle()
{
#if 0
    WorldRouter::update_time();
    op=WorldRouter::get_operation_from_queue();
    if (not op) {
        return 0;
    }
    WorldRouter::operation(op);
    return 1;
#endif
}

#include <Atlas/Message/Object.h>
#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/Operation/Login.h>
#include <Atlas/Objects/Operation/Look.h>

#include "WorldRouter.h"

#include <rulesets/Thing.h>
#include <common/WorldInfo.h>

extern "C" {
    #include <stdio.h>
}


WorldRouter::WorldRouter(ServerRouting * srvr) : server(srvr)
{
    //id = 0;
    fullid = "world_0";
    update_time();
    // real_time=time(NULL);
    // world_info::time = time(NULL);
    server->id_dict[fullid]=this;
    //objects[id]=this;
    fobjects[fullid]=this;
    illegal_thing = new Thing();
    illegal_thing->fullid = "illegal";
    illegal_thing->name = "illegal";
    illegal_thing->deleted = 1;
    //WorldTime tmp_date("612-1-1 08:57:00");
    //This structure is used to tell libatlas about stuff
    //world_info.time.s=tmp_date.seconds();
    //world_info.coordinates_conversion_class=Vector3D;
    //world_info.seconds2string=seconds2string;
    //world_info.string2DateTime=WorldTime;
}

string WorldRouter::get_id(string & name)
{
    string full_id;
    char * buf = (char *)malloc(strlen(name.c_str()) + 32);
    next_id++;
    if (buf) {
        sprintf(buf, "%s_%d", name.c_str(), next_id);
        full_id = string(buf);
    } else {
        cout << "BARRRRF" << endl << flush;
    }
    return(full_id);
}

BaseEntity * WorldRouter::add_object(BaseEntity * obj)
{
    obj->fullid=get_id(obj->name);
    server->id_dict[obj->fullid]=fobjects[obj->fullid]=obj;
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
    Setup * s = new Setup();
    *s = Setup::Instantiate();
    s->SetTo(obj->fullid);
    s->SetFutureSeconds(-0.1);
    add_operation_to_queue(*s, this);
    return (obj);
}

BaseEntity * WorldRouter::add_object(const string & type,
                                     const Message::Object & ent)
{
    BaseEntity * obj;
    obj = ThingFactory::new_thing(type, ent);
    return add_object(obj);
}

void WorldRouter::del_object(BaseEntity * obj)
{
    contains.remove(obj);
    omnipresent_list.remove(obj);
    perceptives.remove(obj);

    //objects.erase(obj->id);
    //objects[obj->id] = illegal_thing;
    fobjects[obj->fullid] = illegal_thing;
}

bad_type WorldRouter::is_object_deleted(BaseEntity * obj)
{
    //return (objects.find(obj->id)!=objects.end());
    return find_object(obj->fullid)->fullid=="illegal";
}

RootOperation * WorldRouter::message(const RootOperation & msg)
{
    cout << "FATAL: Wrong type of WorldRouter message function called" << endl << flush;
    // You may eventually want to remove this as it causes a deliberate segfault
    return(*(RootOperation **)NULL);
}
RootOperation * WorldRouter::message(RootOperation & msg, BaseEntity * obj)
{
    add_operation_to_queue(msg, obj);
    return(NULL);
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
    return NULL;
}

RootOperation * WorldRouter::operation(const RootOperation * op)
{
    RootOperation * res = NULL;
    const RootOperation & op_ref = *op;
    string to = op_ref.GetTo();
    cout << "WorldRouter::operation {" << to << "}" << endl << flush;
    op_no_t op_type = op_enumerate(op);

    cout << 0 << flush;
    if ((to.size() != 0) && (to!="all")) {
        cout << 1 << flush;
        if (fobjects.find(to) == fobjects.end()) {
            cout << "FATAL: Op has invalid to" << endl << flush;
            return(*(RootOperation **)NULL);
        }
        cout << 2 << flush;
        BaseEntity * d_to = fobjects[to];
        if ((to != fullid) || (op_type == OP_LOOK)) {
            cout << 3 << flush;
            if (to == fullid) {
                res = ((BaseEntity *)this)->Operation((Look &)op_ref);
            } else {
                res = d_to->operation(op_ref);
            }
            if (op_type == OP_DELETE) {
                d_to->destroy();
                d_to->deleted=1;
            }
            if (res) {
                message(*res, d_to);
            }
        }
    } else {
        cout << 4 << flush;
        RootOperation newop = op_ref;
        std::list<BaseEntity *>::iterator I;
        for(I = perceptives.begin(); I != perceptives.end(); I++) {
            newop.SetTo((*I)->fullid);
            operation(&newop);
        }
    }
                
    return(NULL);
}

RootOperation * WorldRouter::operation(const RootOperation & op)
{
    return(operation(&op));
}

RootOperation * WorldRouter::Operation(const Look & op)
{
    cout << "WorldRouter::Operation(Look)" << endl << flush;
    string from = op.GetFrom();
    if (fobjects.find(from) == fobjects.end()) {
        cout << "FATAL: Op has invalid from" << endl << flush;
        return(*(RootOperation **)NULL);
    } else {
        cout << "Adding [" << from << "] to perceptives" << endl << flush;
        perceptives.push_back(fobjects[from]);
    }
    return(BaseEntity::Operation(op));
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
    return(None);
}

void WorldRouter::add_operation_to_queue(RootOperation & op, BaseEntity * obj)
{
    if (op.GetFrom() == "cheat") {
        op.SetFrom(op.GetTo());
    } else {
        op.SetFrom(obj->fullid);
    }
    double t = double(time(NULL));
    t = t + op.GetFutureSeconds();
    op.SetSeconds(t);
    op.SetFutureSeconds(0.0);
    std::list<RootOperation *>::iterator I;
    int i = 0;
    for(I = operation_queue.begin();
        (I != operation_queue.end()) && ((*I)->GetSeconds() < t) ; I++,i++);
    operation_queue.insert(I, &op);
    cout << i << " operation added to queue" << endl << flush;
        
        
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

RootOperation * WorldRouter::get_operation_from_queue()
{
    std::list<RootOperation *>::iterator I = operation_queue.begin();
    if (I == operation_queue.end()) {
        return(NULL);
    }
    if ((*I)->GetSeconds() > real_time) {
        return(NULL);
    }
    cout << "pulled op off queue" << endl << flush;
    RootOperation * op = (*I);
    operation_queue.pop_front();
    return(op);
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
    return None;
}

bad_type WorldRouter::update_all_ranges(BaseEntity * obj)
{
#if 0
    WorldRouter::update_range(obj, "audible", const.hearing_range);
    return WorldRouter::update_range(obj, "visible", const.sight_range, 1);
#endif
    return None;
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
    return None;
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
    return None;
}

bad_type WorldRouter::execute_code(bad_type code)
{
#if 0
    exec(code);
#endif
    return None;
}

bad_type WorldRouter::eval_code(bad_type code)
{
#if 0
    return eval(code);
#endif
    return None;
}

bad_type WorldRouter::save(bad_type filename)
{
#if 0
    persistence.save_world(self, filename);
#endif
    return None;
}

bad_type WorldRouter::load(bad_type filename)
{
#if 0
    persistence.load_world(self, filename);
#endif
    return None;
}

bad_type WorldRouter::update_time()
{
#if 0
    new_time=time();
    world_info.time.s=world_info.time.s+\;
                       const.time_multiplier*(new_time-WorldRouter::real_time);
    WorldRouter::real_time=new_time;
#endif
    world_info::time = time(NULL);
    real_time = world_info::time;
    return None;
}

bad_type WorldRouter::get_time()
{
#if 0
    return WorldTime(world_info.time.s);
#endif
    return None;
}

bad_type WorldRouter::idle()
{
    update_time();
    RootOperation * op;
    while ((op = get_operation_from_queue()) != NULL) {
        cout << "OP" << endl << flush;
        operation(op);
    }
    if (op==NULL) {
        return(0);
    }
    return(1);
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

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

static int debug_server = 1;


WorldRouter::WorldRouter(ServerRouting * srvr) : server(srvr)
{
    fullid = "world_0";
    update_time();
    server->id_dict[fullid]=this;
    fobjects[fullid]=this;
    perceptives.push_back(this);
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
        debug_server && cout << "BARRRRF" << endl << flush;
    }
    return(full_id);
}

Thing * WorldRouter::add_object(Thing * obj)
{
    debug_server && cout << "WorldRouter::add_object(Thing *)" << endl << flush;
    obj->fullid=get_id(obj->name);
    server->id_dict[obj->fullid]=fobjects[obj->fullid]=obj;
    if (!obj->location) {
        debug_server && cout << "set loc " << this  << endl << flush;
        obj->location=Location(this, Vector3D(0,0,0));
        debug_server && cout << "loc set with parent " << obj->location.parent->fullid << endl << flush;
    }
    if (NULL == obj->location.parent) {
        debug_server && cout << "set parent" << endl << flush;
        obj->location.parent=this;
    }
    if (obj->location.parent==this /*&& contains[obj->id]*/) {
        debug_server && cout << "loc is world" << endl << flush;
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

Thing * WorldRouter::add_object(const string & typestr,
                                     const Message::Object & ent)
{
    debug_server && cout << "WorldRouter::add_object(string, ent)" << endl << flush;
    Thing * obj;
    obj = thing_factory.new_thing(typestr, ent, this);
    return add_object(obj);
}

void WorldRouter::del_object(BaseEntity * obj)
{
    contains.remove(obj);
    omnipresent_list.remove(obj);
    perceptives.remove(obj);

    fobjects[obj->fullid] = illegal_thing;
}

bad_type WorldRouter::is_object_deleted(BaseEntity * obj)
{
    return find_object(obj->fullid)->fullid=="illegal";
}

oplist WorldRouter::message(const RootOperation & msg)
{
    debug_server && cout << "FATAL: Wrong type of WorldRouter message function called" << endl << flush;
    // You may eventually want to remove this as it causes a deliberate segfault
    //return(*(RootOperation **)NULL);
}

oplist WorldRouter::message(RootOperation & msg, BaseEntity * obj)
{
    add_operation_to_queue(msg, obj);
    oplist res;
    return(res);
}

BaseEntity * WorldRouter::get_operation_place(const RootOperation & op)
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

oplist WorldRouter::operation(const RootOperation * op)
{
    oplist res;
    const RootOperation & op_ref = *op;
    string to = op_ref.GetTo();
    debug_server && cout << "WorldRouter::operation {" << to << "}" << endl << flush;
    op_no_t op_type = op_enumerate(op);

    debug_server && cout << 0 << flush;
    if ((to.size() != 0) && (to!="all")) {
        debug_server && cout << 1 << flush;
        if (fobjects.find(to) == fobjects.end()) {
            debug_server && cout << "FATAL: Op has invalid to" << endl << flush;
            //return(*(RootOperation **)NULL);
        }
        debug_server && cout << 2 << flush;
        BaseEntity * d_to = fobjects[to];
        if ((to != fullid) || (op_type == OP_LOOK)) {
            debug_server && cout << 3 << flush;
            if (to == fullid) {
                res = ((BaseEntity *)this)->Operation((Look &)op_ref);
            } else {
                res = d_to->operation(op_ref);
            }
            if (op_type == OP_DELETE) {
                d_to->destroy();
                d_to->deleted=1;
            }
            while (res.size() != 0) {
                RootOperation * ro = res.front();
                message(*ro, d_to);
                res.pop_front();
            }
        }
    } else {
        debug_server && cout << 4 << flush;
        RootOperation newop = op_ref;
        std::list<BaseEntity *>::iterator I;
        for(I = perceptives.begin(); I != perceptives.end(); I++) {
            newop.SetTo((*I)->fullid);
            operation(&newop);
        }
    }
                
    oplist res2;
    return(res2);
}

oplist WorldRouter::operation(const RootOperation & op)
{
    return(operation(&op));
}

oplist WorldRouter::Operation(const Look & op)
{
    debug_server && cout << "WorldRouter::Operation(Look)" << endl << flush;
    string from = op.GetFrom();
    if (fobjects.find(from) == fobjects.end()) {
        debug_server && cout << "FATAL: Op has invalid from" << endl << flush;
        //return(*(RootOperation **)NULL);
    } else {
        debug_server && cout << "Adding [" << from << "] to perceptives" << endl << flush;
        perceptives.push_back(fobjects[from]);
        perceptives.unique();
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
    debug_server && cout << i << " operation added to queue" << endl << flush;
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
    debug_server && cout << "pulled op off queue" << endl << flush;
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

void WorldRouter::update_time()
{
    // This is still lots simpler than the version in cyphesis-py
    world_info::time = time(NULL);
    real_time = world_info::time;
}

bad_type WorldRouter::get_time()
{
#if 0
    return WorldTime(world_info.time.s);
#endif
    return None;
}

int WorldRouter::idle()
{
    update_time();
    RootOperation * op;
    while ((op = get_operation_from_queue()) != NULL) {
        operation(op);
    }
    if (op==NULL) {
        return(0);
    }
    return(1);
}

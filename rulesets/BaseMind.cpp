#include <Atlas/Message/Object.h>
#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/Entity/RootEntity.h>
#include <Atlas/Objects/Operation/Login.h>
#include <Atlas/Objects/Operation/Sight.h>
#include <Atlas/Objects/Operation/Sound.h>
#include <Atlas/Objects/Operation/Create.h>
#include <Atlas/Objects/Operation/Delete.h>
#include <Atlas/Objects/Operation/Move.h>
#include <Atlas/Objects/Operation/Set.h>
#include <Atlas/Objects/Operation/Touch.h>

#include <common/utility.h>

#include "BaseMind.h"


BaseMind::BaseMind(string & id, string & body_name)
{
    //map=MemMap();
    fullid = id;
    name = body_name;
    map.add_object(this);
    //BaseMind::time=WorldTime();
    //if (const.debug_level>=1) {
        //BaseMind::log_fp=open("mind_"+BaseMind::id+".log","w");
    //}
    //else {
        //BaseMind::log_fp=None;
    //}
}

oplist BaseMind::Sight_Operation(const Sight & op, Login & sub_op)
{
    oplist res;
    if (script_Operation("sight_login", op, res, &sub_op) != 0) {
        return(res);
    }
    return(res);
}

oplist BaseMind::Sight_Operation(const Sight & op, Create & sub_op)
{
    oplist res;
    if (script_Operation("sight_create", op, res, &sub_op) != 0) {
        return(res);
    }
    const Object::ListType & args = sub_op.GetArgs();
    if (args.size() == 0) {
        cout << " no args!" << endl << flush;
        return(res);
    }
    Object obj = args.front();
    Root * arg = utility::Object_asRoot(obj);
    if (arg->GetObjtype() == "object") {
        map.add(arg->AsObject());
    }
    return(res);
}

oplist BaseMind::Sight_Operation(const Sight & op, Delete & sub_op)
{
    oplist res;
    if (script_Operation("sight_delete", op, res, &sub_op) != 0) {
        return(res);
    }
    const Object::ListType & args = sub_op.GetArgs();
    if (args.size() == 0) {
        cout << " no args!" << endl << flush;
        return(res);
    }
    Object obj = args.front();
    if (obj.IsString()) {
        map._delete(obj.AsString());
    } else {
        Root * arg = utility::Object_asRoot(obj);
        if (arg->GetObjtype() == "object") {
            map._delete(arg->GetId());
        }
    }
    return(res);
}

oplist BaseMind::Sight_Operation(const Sight & op, Move & sub_op)
{
    cout << "BaseMind::Sight_Operation(Sight, Move)" << endl << flush;
    oplist res;
    if (script_Operation("sight_move", op, res, &sub_op) != 0) {
        return(res);
    }
    const Object::ListType & args = sub_op.GetArgs();
    if (args.size() == 0) {
        cout << " no args!" << endl << flush;
        return(res);
    }
    Object obj = args.front();
    Root * arg = utility::Object_asRoot(obj);
    if (arg->GetObjtype() == "object") {
        map.update(arg->AsObject());
    }
    return(res);
}

oplist BaseMind::Sight_Operation(const Sight & op, Set & sub_op)
{
    oplist res;
    if (script_Operation("sight_set", op, res, &sub_op) != 0) {
        return(res);
    }
    const Object::ListType & args = sub_op.GetArgs();
    if (args.size() == 0) {
        cout << " no args!" << endl << flush;
        return(res);
    }
    Object obj = args.front();
    Root * arg = utility::Object_asRoot(obj);
    if (arg->GetObjtype() == "object") {
        map.update(arg->AsObject());
    }
    return(res);
}

oplist BaseMind::Sight_Operation(const Sight & op, Touch & sub_op)
{
    oplist res;
    if (script_Operation("sight_touch", op, res, &sub_op) != 0) {
        return(res);
    }
    return(res);
}

oplist BaseMind::Sight_Operation(const Sight & op, RootOperation & sub_op)
{
    cout << "BaseMind::Sight_Operation(Sight, RootOperation)" << endl << flush;
    oplist res;
    if (script_Operation("sight_undefined", op, res, &sub_op) != 0) {
        return(res);
    }
    return(res);
}

oplist BaseMind::Sound_Operation(const Sound & op, Talk & sub_op)
{
    cout << "BaseMind::Sound_Operation(Sound, Talk)" << endl << flush;
    oplist res;
    if (script_Operation("sound_talk", op, res, &sub_op) != 0) {
        return(res);
    }
    return(res);
}

oplist BaseMind::Sound_Operation(const Sound & op, RootOperation & sub_op)
{
    cout << "BaseMind::Sound_Operation(Sound, RootOperation)" << endl << flush;
    oplist res;
    if (script_Operation("sound_undefined", op, res, &sub_op) != 0) {
        return(res);
    }
    return(res);
}

oplist BaseMind::call_sound_operation(const Sound & op, RootOperation & sub_op)
{
    oplist res;
    op_no_t op_no = op_enumerate(&sub_op);
    SUB_OP_SWITCH(op, op_no, res, Sound_, sub_op)
    return(res);
}

oplist BaseMind::Operation(const Sound & op)
{
    // Deliver argument to Sound_ things
    oplist res;
    if (script_Operation("sound", op, res) != 0) {
        return(res);
    }
    const Object::ListType & args = op.GetArgs();
    if (args.size() == 0) {
        cout << " no args!" << endl << flush;
        return(res);
    }
    Object obj = args.front();
    Root * op2 = utility::Object_asRoot(obj);
    if (op2->GetObjtype() == "op") {
        cout << " args is an op!" << endl << flush;
        res = call_sound_operation(op, *(RootOperation *)op2);
    }
    return(res);
}

oplist BaseMind::call_sight_operation(const Sight & op, RootOperation & sub_op)
{
    oplist res;
    op_no_t op_no = op_enumerate(&sub_op);
    SUB_OP_SWITCH(op, op_no, res, Sight_, sub_op)
    return(res);
}

oplist BaseMind::Operation(const Sight & op)
{
    cout << "BaseMind::Operation(Sight)" << endl << flush;
    // Deliver argument to Sight_ things
    oplist(res);
    if (script_Operation("sight", op, res) != 0) {
        cout << " its in the script" << endl << flush;
        return(res);
    }
    const Object::ListType & args = op.GetArgs();
    if (args.size() == 0) {
        cout << " no args!" << endl << flush;
        return(res);
    }
    Object obj = args.front();
    Root * op2 = utility::Object_asRoot(obj);
    if (op2->GetObjtype() == "op") {
        cout << " args is an op!" << endl << flush;
        res = call_sight_operation(op, *(RootOperation *)op2);
        //string & op2type = op2->GetParents().front().AsString();
        //string subop = "sight_" + op2type;
        //script_Operation(subop, op, res, (RootOperation *)op2);
    } else if (op2->GetObjtype() == "object") {
        cout << " arg is an entity!" << endl << flush;
        map.add(obj);
    }
    return(res);
}

#if 0
RootOperation * BaseMind::get_op_name_and_sub(RootOperation & op, string & name)
{
    event_name = op.id;
    sub_op = op;
    while (len(sub_op) and sub_op[0].get_name()=="op") {
        sub_op = sub_op[0];
        event_name = event_name + "_" + sub_op.id;
    }
    return event_name, sub_op;
}

int BaseMind::call_triggers(RootOperation & op)
{
    return(0);
}
#endif

oplist BaseMind::operation(RootOperation & op)
{
    // This might end up being quite tricky to do

    // In the python the following happens here:
    //   Find out if the op refers to any ids we don't know about.
    //   If so create look operations to those ids
    //   Set the minds time and date 
    oplist res;
    res = call_operation(op);
    //res = call_triggers(op);
    return(res);
}

#if 0
bad_type BaseMind::operation(bad_type op)
{
    if (hasattr(this,"lock")) {
        return;
    }
    if (BaseMind::log_fp) {
        BaseMind::log_fp.write("receiving:\n");
        BaseMind::log_fp.write(str(op)+"\n");
    }
    reply=Message();
    res=op.atlas2internal(BaseMind::map.things);
    if (res) {
        log.debug(3,str(BaseMind::id)+" : new id: "+str(res));
        for (/*(err_op,attr,id) in res*/) {
            obj=BaseMind::map.add_id(id);
            if (type(err_op)==InstanceType) {
                setattr(err_op,attr,obj);
            }
            else {
                err_op[attr]=obj;
            }
            log.debug(3,str(obj));
        }
    }
    while (1) {
        look=BaseMind::map.look_id();
        if (not look) {
            break;
        }
        look.from_=this;
        reply.append(look);
    }
    if (hasattr(op.time,"dateTime")) {
        BaseMind::time=op.time.dateTime;
    }
    reply=reply+BaseMind::call_operation(op);
    reply=reply+BaseMind::call_triggers(op);
    op.internal2atlas();
    if (BaseMind::log_fp) {
        BaseMind::log_fp.write("sending:\n");
        BaseMind::log_fp.write(str(reply)+"\n");
    }
    return reply;
}
#endif

// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#include <Atlas/Message/Object.h>
#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/Entity/RootEntity.h>
#include <Atlas/Objects/Operation/Login.h>
#include <Atlas/Objects/Operation/Sight.h>
#include <Atlas/Objects/Operation/Sound.h>
#include <Atlas/Objects/Operation/Combine.h>
#include <Atlas/Objects/Operation/Create.h>
#include <Atlas/Objects/Operation/Delete.h>
#include <Atlas/Objects/Operation/Divide.h>
#include <Atlas/Objects/Operation/Move.h>
#include <Atlas/Objects/Operation/Set.h>
#include <Atlas/Objects/Operation/Talk.h>
#include <Atlas/Objects/Operation/Touch.h>
#include <Atlas/Objects/Operation/Appearance.h>
#include <Atlas/Objects/Operation/Disappearance.h>

#include <common/Chop.h>
#include <common/Cut.h>
#include <common/Eat.h>
#include <common/Fire.h>

#include <common/utility.h>
#include <common/debug.h>
#include <common/op_switch.h>

#include "BaseMind.h"
#include "MemMap_methods.h"

//static const bool debug_flag = false;

BaseMind::BaseMind(const string & id, const string & body_name)
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
        //BaseMind::log_fp=NULL;
    //}
}

BaseMind::~BaseMind()
{
}

int BaseMind::set_script(Script * scrpt) {
    map.set_script(scrpt);
    script = scrpt;
    return(scrpt == NULL ? -1 : 0);
}

MemMap * BaseMind::getMap() {
    return &map;
}

oplist BaseMind::Sight_Operation(const Sight & op, Login & sub_op)
{
    oplist res;
    if (script->Operation("sight_login", op, res, &sub_op) != 0) {
        return(res);
    }
    return(res);
}

oplist BaseMind::Sight_Operation(const Sight & op, Chop & sub_op)
{
    oplist res;
    script->Operation("sight_chop", op, res, &sub_op);
    return(res);
}

oplist BaseMind::Sight_Operation(const Sight & op, Create & sub_op)
{
    oplist res;
    if (script->Operation("sight_create", op, res, &sub_op) != 0) {
        return(res);
    }
    const Object::ListType & args = sub_op.GetArgs();
    if (args.size() == 0) {
        debug( cout << " no args!" << endl << flush;);
        return(res);
    }
    Object obj = args.front();
    Root * arg = utility::Object_asRoot(obj);
    if (arg->GetObjtype() != "op") {
        map.add(arg->AsObject());
    }
    delete arg;
    return(res);
}

oplist BaseMind::Sight_Operation(const Sight & op, Cut & sub_op)
{
    oplist res;
    script->Operation("sight_cut", op, res, &sub_op);
    return(res);
}

oplist BaseMind::Sight_Operation(const Sight & op, Delete & sub_op)
{
    debug( cout << "Sight Delete operation" << endl << flush;);
    oplist res;
    if (script->Operation("sight_delete", op, res, &sub_op) != 0) {
        return(res);
    }
    const Object::ListType & args = sub_op.GetArgs();
    if (args.size() == 0) {
        debug( cout << " no args!" << endl << flush;);
        return(res);
    }
    Object obj = args.front();
    if (obj.IsString()) {
        map._delete(obj.AsString());
    } else {
        Root * arg = utility::Object_asRoot(obj);
        if (arg->GetObjtype() != "op") {
            map._delete(arg->GetId());
        }
        delete arg;
    }
    return(res);
}

oplist BaseMind::Sight_Operation(const Sight & op, Eat & sub_op)
{
    oplist res;
    script->Operation("sight_eat", op, res, &sub_op);
    return(res);
}

oplist BaseMind::Sight_Operation(const Sight & op, Fire & sub_op)
{
    oplist res;
    script->Operation("sight_fire", op, res, &sub_op);
    return(res);
}

oplist BaseMind::Sight_Operation(const Sight & op, Move & sub_op)
{
    debug( cout << "BaseMind::Sight_Operation(Sight, Move)" << endl << flush;);
    oplist res;
    if (script->Operation("sight_move", op, res, &sub_op) != 0) {
        return(res);
    }
    const Object::ListType & args = sub_op.GetArgs();
    if (args.size() == 0) {
        debug( cout << " no args!" << endl << flush;);
        return(res);
    }
    const Object & obj = args.front();
    Root * arg = utility::Object_asRoot(obj);
    if (arg->GetObjtype() != "op") {
        map.update(obj);
    }
    delete arg;
    return(res);
}

oplist BaseMind::Sight_Operation(const Sight & op, Set & sub_op)
{
    oplist res;
    if (script->Operation("sight_set", op, res, &sub_op) != 0) {
        return(res);
    }
    const Object::ListType & args = sub_op.GetArgs();
    if (args.size() == 0) {
        debug( cout << " no args!" << endl << flush;);
        return(res);
    }
    const Object & obj = args.front();
    Root * arg = utility::Object_asRoot(obj);
    if (arg->GetObjtype() != "op") {
        map.update(obj);
    }
    delete arg;
    return(res);
}

oplist BaseMind::Sight_Operation(const Sight & op, Touch & sub_op)
{
    oplist res;
    script->Operation("sight_touch", op, res, &sub_op);
    return(res);
}

oplist BaseMind::Sight_Operation(const Sight & op, RootOperation & sub_op)
{
    debug( cout << "BaseMind::Sight_Operation(Sight, RootOperation)" << endl << flush;);
    oplist res;
    script->Operation("sight_undefined", op, res, &sub_op);
    return(res);
}

oplist BaseMind::Sound_Operation(const Sound & op, Talk & sub_op)
{
    debug( cout << "BaseMind::Sound_Operation(Sound, Talk)" << endl << flush;);
    oplist res;
    script->Operation("sound_talk", op, res, &sub_op);
    return(res);
}

oplist BaseMind::Sound_Operation(const Sound & op, RootOperation & sub_op)
{
    debug( cout << "BaseMind::Sound_Operation(Sound, RootOperation)" << endl << flush;);
    oplist res;
    script->Operation("sound_undefined", op, res, &sub_op);
    return(res);
}

#if 0
oplist BaseMind::call_sound_operation(const Sound & op, RootOperation & sub_op)
{
    map.get_add(sub_op.GetFrom());
    op_no_t op_no = op_enumerate(&sub_op);
    SUB_OP_SWITCH(op, op_no, Sound_, sub_op)
}
#endif

oplist BaseMind::Operation(const Sound & op)
{
    // Deliver argument to Sound_ things
    oplist res;
    if (script->Operation("sound", op, res) != 0) {
        return(res);
    }
    const Object::ListType & args = op.GetArgs();
    if (args.size() == 0) {
        debug( cout << " no args!" << endl << flush;);
        return(res);
    }
    Object obj = args.front();
    Root * op2 = utility::Object_asRoot(obj);
    if (op2->GetObjtype() == "op") {
        debug( cout << " args is an op!" << endl << flush;);
        res = call_sound_operation(op, *(RootOperation *)op2);
    }
    delete op2;
    return(res);
}

#if 0
oplist BaseMind::call_sight_operation(const Sight & op, RootOperation & sub_op)
{
    map.get_add(sub_op.GetFrom());
    op_no_t op_no = op_enumerate(&sub_op);
    SUB_OP_SWITCH(op, op_no, Sight_, sub_op)
}
#endif

oplist BaseMind::Operation(const Sight & op)
{
    debug( cout << "BaseMind::Operation(Sight)" << endl << flush;);
    // Deliver argument to Sight_ things
    oplist(res);
    if (script->Operation("sight", op, res) != 0) {
        debug( cout << " its in the script" << endl << flush;);
        return(res);
    }
    const Object::ListType & args = op.GetArgs();
    if (args.size() == 0) {
        debug( cout << " no args!" << endl << flush;);
        return(res);
    }
    Object obj = args.front();
    Root * op2 = utility::Object_asRoot(obj);
    if (op2->GetObjtype() == "op") {
        debug( cout << " args is an op!" << endl << flush;);
        res = call_sight_operation(op, *(RootOperation *)op2);
        //string & op2type = op2->GetParents().front().AsString();
        //string subop = "sight_" + op2type;
        //script->Operation(subop, op, res, (RootOperation *)op2);
    } else /* if (op2->GetObjtype() == "object") */ {
        debug( cout << " arg is an entity!" << endl << flush;);
        map.add(obj);
    }
    delete op2;
    return(res);
}

oplist BaseMind::Operation(const Appearance & op)
{
    oplist(res);
    script->Operation("appearance", op, res);
    Object::ListType args = op.GetArgs();
    Object::ListType::iterator I;
    for(I = args.begin(); I != args.end(); I++) {
        map.get_add(I->AsMap()["id"].AsString());
    }
    return res;
}

oplist BaseMind::Operation(const Disappearance & op)
{
    oplist(res);
    script->Operation("disappearance", op, res);
    // Not quite sure what to do to the map here, but should do something.
    return res;
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

//oplist BaseMind::message(const RootOperation & msg)
//{
    //return operation(msg);
//}

oplist BaseMind::operation(const RootOperation & op)
{
    // This might end up being quite tricky to do

    // In the python the following happens here:
    //   Find out if the op refers to any ids we don't know about.
    //   If so create look operations to those ids
    //   Set the minds time and date 
    oplist res;
    map.get_add(op.GetFrom());
    RootOperation * look;
    while ((look = map.look_id()) != NULL) {
        res.push_back(look);
    }
    oplist res2 = call_operation(op);
    for(oplist::const_iterator I = res2.begin(); I != res2.end(); I++) {
        res.push_back(*I);
    }
    //res = call_triggers(op);
    return(res);
}

oplist BaseMind::call_sight_operation(const Sight& op, RootOperation& sub_op) {
    map.get_add(sub_op.GetFrom());
    op_no_t op_no = op_enumerate(&sub_op);
    SUB_OP_SWITCH(op, op_no, Sight_, sub_op)
}

oplist BaseMind::call_sound_operation(const Sound& op, RootOperation& sub_op) {
    map.get_add(sub_op.GetFrom());
    op_no_t op_no = op_enumerate(&sub_op);
    SUB_OP_SWITCH(op, op_no, Sound_, sub_op)
}

#if 0
oplist BaseMind::operation(const RootOperation & op)
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

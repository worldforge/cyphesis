// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

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
#include <common/Load.h>
#include <common/Save.h>

#include <common/utility.h>
#include <common/debug.h>
#include <common/op_switch.h>

#include "BaseMind.h"
#include "MemMap_methods.h"

//static const bool debug_flag = false;

BaseMind::BaseMind(const string & id, const string & body_name) : map(script),
                                                                  isAwake(true)
{
    fullid = id;
    name = body_name;
    map.addObject(this);
    //BaseMind::time=WorldTime();
}

BaseMind::~BaseMind()
{
}

MemMap * BaseMind::getMap() {
    return &map;
}

oplist BaseMind::sightOperation(const Sight & op, Login & sub_op)
{
    oplist res;
    if (script->Operation("sight_login", op, res, &sub_op) != 0) {
        return res;
    }
    return res;
}

oplist BaseMind::sightOperation(const Sight & op, Action & sub_op)
{
    oplist res;
    script->Operation("sight_action", op, res, &sub_op);
    return res;
}

oplist BaseMind::sightOperation(const Sight & op, Chop & sub_op)
{
    oplist res;
    script->Operation("sight_chop", op, res, &sub_op);
    return res;
}

oplist BaseMind::sightOperation(const Sight & op, Create & sub_op)
{
    cout << "sighCreate " << fullid << endl << flush;
    oplist res;
    if (script->Operation("sight_create", op, res, &sub_op) != 0) {
        return res;
    }
    const Object::ListType & args = sub_op.GetArgs();
    if (args.size() == 0) {
        debug( cout << " no args!" << endl << flush;);
        return res;
    }
    Object obj = args.front();
    Root * arg = utility::Object_asRoot(obj);
    if (arg->GetObjtype() != "op") {
        map.add(arg->AsObject());
    }
    delete arg;
    return res;
}

oplist BaseMind::sightOperation(const Sight & op, Cut & sub_op)
{
    oplist res;
    script->Operation("sight_cut", op, res, &sub_op);
    return res;
}

oplist BaseMind::sightOperation(const Sight & op, Delete & sub_op)
{
    debug( cout << "Sight Delete operation" << endl << flush;);
    oplist res;
    if (script->Operation("sight_delete", op, res, &sub_op) != 0) {
        return res;
    }
    const Object::ListType & args = sub_op.GetArgs();
    if (args.size() == 0) {
        debug( cout << " no args!" << endl << flush;);
        return res;
    }
    Object obj = args.front();
    if (obj.IsString()) {
        map.del(obj.AsString());
    } else {
        Root * arg = utility::Object_asRoot(obj);
        if (arg->GetObjtype() != "op") {
            map.del(arg->GetId());
        }
        delete arg;
    }
    return res;
}

oplist BaseMind::sightOperation(const Sight & op, Eat & sub_op)
{
    oplist res;
    script->Operation("sight_eat", op, res, &sub_op);
    return res;
}

oplist BaseMind::sightOperation(const Sight & op, Fire & sub_op)
{
    oplist res;
    script->Operation("sight_fire", op, res, &sub_op);
    return res;
}

oplist BaseMind::sightOperation(const Sight & op, Move & sub_op)
{
    debug( cout << "BaseMind::sightOperation(Sight, Move)" << endl << flush;);
    oplist res;
    if (script->Operation("sight_move", op, res, &sub_op) != 0) {
        return res;
    }
    const Object::ListType & args = sub_op.GetArgs();
    if (args.size() == 0) {
        debug( cout << " no args!" << endl << flush;);
        return res;
    }
    const Object & obj = args.front();
    Root * arg = utility::Object_asRoot(obj);
    if (arg->GetObjtype() != "op") {
        map.update(obj);
    }
    delete arg;
    return res;
}

oplist BaseMind::sightOperation(const Sight & op, Set & sub_op)
{
    oplist res;
    if (script->Operation("sight_set", op, res, &sub_op) != 0) {
        return res;
    }
    const Object::ListType & args = sub_op.GetArgs();
    if (args.size() == 0) {
        debug( cout << " no args!" << endl << flush;);
        return res;
    }
    const Object & obj = args.front();
    Root * arg = utility::Object_asRoot(obj);
    if (arg->GetObjtype() != "op") {
        map.update(obj);
    }
    delete arg;
    return res;
}

oplist BaseMind::sightOperation(const Sight & op, Touch & sub_op)
{
    oplist res;
    script->Operation("sight_touch", op, res, &sub_op);
    return res;
}

oplist BaseMind::sightOperation(const Sight & op, RootOperation & sub_op)
{
    debug( cout << "BaseMind::sightOperation(Sight, RootOperation)" << endl << flush;);
    oplist res;
    script->Operation("sight_undefined", op, res, &sub_op);
    return res;
}

oplist BaseMind::soundOperation(const Sound & op, Talk & sub_op)
{
    debug( cout << "BaseMind::soundOperation(Sound, Talk)" << endl << flush;);
    oplist res;
    script->Operation("sound_talk", op, res, &sub_op);
    return res;
}

oplist BaseMind::soundOperation(const Sound & op, RootOperation & sub_op)
{
    debug( cout << "BaseMind::soundOperation(Sound, RootOperation)" << endl << flush;);
    oplist res;
    script->Operation("sound_undefined", op, res, &sub_op);
    return res;
}

oplist BaseMind::Operation(const Sound & op)
{
    // Louder sounds might eventually make character wake up
    if (!isAwake) { return oplist(); }
    // Deliver argument to sound things
    oplist res;
    if (script->Operation("sound", op, res) != 0) {
        return res;
    }
    const Object::ListType & args = op.GetArgs();
    if (args.size() == 0) {
        debug( cout << " no args!" << endl << flush;);
        return res;
    }
    Object obj = args.front();
    Root * op2 = utility::Object_asRoot(obj);
    if (op2->GetObjtype() == "op") {
        debug( cout << " args is an op!" << endl << flush;);
        res = callSoundOperation(op, *(RootOperation *)op2);
    }
    delete op2;
    return res;
}

oplist BaseMind::Operation(const Sight & op)
{
    if (!isAwake) { return oplist(); }
    debug( cout << "BaseMind::Operation(Sight)" << endl << flush;);
    // Deliver argument to sight things
    oplist res;
    if (script->Operation("sight", op, res) != 0) {
        debug( cout << " its in the script" << endl << flush;);
        return res;
    }
    const Object::ListType & args = op.GetArgs();
    if (args.size() == 0) {
        debug( cout << " no args!" << endl << flush;);
        return res;
    }
    Object obj = args.front();
    Root * op2 = utility::Object_asRoot(obj);
    if (op2->GetObjtype() == "op") {
        debug( cout << " args is an op!" << endl << flush;);
        res = callSightOperation(op, *(RootOperation *)op2);
        //string & op2type = op2->GetParents().front().AsString();
        //string subop = "sight_" + op2type;
        //script->Operation(subop, op, res, (RootOperation *)op2);
    } else /* if (op2->GetObjtype() == "object") */ {
        debug( cout << " arg is an entity!" << endl << flush;);
        map.add(obj);
    }
    delete op2;
    return res;
}

oplist BaseMind::Operation(const Appearance & op)
{
    if (!isAwake) { return oplist(); }
    oplist res;
    script->Operation("appearance", op, res);
    const Object::ListType & args = op.GetArgs();
    Object::ListType::const_iterator I;
    for(I = args.begin(); I != args.end(); I++) {
        map.getAdd(I->AsMap().find("id")->second.AsString());
    }
    return res;
}

oplist BaseMind::Operation(const Disappearance & op)
{
    if (!isAwake) { return oplist(); }
    oplist res;
    script->Operation("disappearance", op, res);
    // Not quite sure what to do to the map here, but should do something.
    return res;
}

oplist BaseMind::Operation(const Save & op)
{
    oplist res;
    script->Operation("save", op, res);
    Object::MapType emap;
    cout << res.size() << " Got stuff from mind" << endl << flush;
    if ((res.size() != 0) && (res.front()->GetArgs().size() != 0)) {
        emap = res.front()->GetArgs().front().AsMap();
        // FIXME Operations created in python are leaked
    }
    Info * i = new Info(Info::Instantiate());
    emap["map"] = map.asObject();
    emap["id"] = fullid;
    i->SetArgs(Object::ListType(1,emap));
    return oplist(1,i);
}

oplist BaseMind::Operation(const Load & op)
{
    oplist res;
    script->Operation("load", op, res);
    const Object::MapType & emap = op.GetArgs().front().AsMap();
    Object::MapType::const_iterator I = emap.find("map");
    if (I != emap.end()) {
        const Object::MapType & memmap = I->second.AsMap();
        Object::MapType::const_iterator J = memmap.begin();
        for(; J != memmap.end(); J++) {
            map.add(J->second);
        }
    }
    return oplist();
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
    return 0;
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
    map.getAdd(op.GetFrom());
    RootOperation * look;
    while ((look = map.lookId()) != NULL) {
        res.push_back(look);
    }
    oplist res2 = callOperation(op);
    for(oplist::const_iterator I = res2.begin(); I != res2.end(); I++) {
        res.push_back(*I);
    }
    //res = call_triggers(op);
    return res;
}

oplist BaseMind::callSightOperation(const Sight& op, RootOperation& sub_op) {
    map.getAdd(sub_op.GetFrom());
    op_no_t op_no = opEnumerate(sub_op);
    SUB_OP_SWITCH(op, op_no, sight, sub_op)
}

oplist BaseMind::callSoundOperation(const Sound& op, RootOperation& sub_op) {
    map.getAdd(sub_op.GetFrom());
    op_no_t op_no = opEnumerate(sub_op);
    SUB_OP_SWITCH(op, op_no, sound, sub_op)
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

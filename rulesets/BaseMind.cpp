// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include <Atlas/Objects/Operation/Login.h>
#include <Atlas/Objects/Operation/Sight.h>
#include <Atlas/Objects/Operation/Sound.h>
#include <Atlas/Objects/Operation/Combine.h>
#include <Atlas/Objects/Operation/Create.h>
#include <Atlas/Objects/Operation/Delete.h>
#include <Atlas/Objects/Operation/Divide.h>
#include <Atlas/Objects/Operation/Imaginary.h>
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

using Atlas::Message::Object;
using Atlas::Objects::Root;

BaseMind::BaseMind(const std::string & id, const std::string & body_name)
                               : map(script), isAwake(true)
{
    setId(id);
    name = body_name;
    map.addObject(this);
    //BaseMind::time=WorldTime();
}

BaseMind::~BaseMind()
{
    map.things.erase(getId());
    map.flushMap();
}

oplist BaseMind::sightLoginOperation(const Sight & op, Login & sub_op)
{
    oplist res;
    script->Operation("sight_login", op, res, &sub_op);
    return res;
}

oplist BaseMind::sightCombineOperation(const Sight & op, Combine & sub_op)
{
    oplist res;
    script->Operation("sight_combine", op, res, &sub_op);
    return res;
}

oplist BaseMind::sightDivideOperation(const Sight & op, Divide & sub_op)
{
    oplist res;
    script->Operation("sight_divide", op, res, &sub_op);
    return res;
}

oplist BaseMind::sightTalkOperation(const Sight & op, Talk & sub_op)
{
    oplist res;
    script->Operation("sight_talk", op, res, &sub_op);
    return res;
}

oplist BaseMind::sightActionOperation(const Sight & op, Action & sub_op)
{
    oplist res;
    script->Operation("sight_action", op, res, &sub_op);
    return res;
}

oplist BaseMind::sightChopOperation(const Sight & op, Chop & sub_op)
{
    oplist res;
    script->Operation("sight_chop", op, res, &sub_op);
    return res;
}

oplist BaseMind::sightCreateOperation(const Sight & op, Create & sub_op)
{
    oplist res;
    if (script->Operation("sight_create", op, res, &sub_op) != 0) {
        return res;
    }
    const Object::ListType & args = sub_op.GetArgs();
    if (args.size() == 0) {
        debug( std::cout << " no args!" << std::endl << std::flush;);
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

oplist BaseMind::sightCutOperation(const Sight & op, Cut & sub_op)
{
    oplist res;
    script->Operation("sight_cut", op, res, &sub_op);
    return res;
}

oplist BaseMind::sightDeleteOperation(const Sight & op, Delete & sub_op)
{
    debug( std::cout << "Sight Delete operation" << std::endl << std::flush;);
    oplist res;
    if (script->Operation("sight_delete", op, res, &sub_op) != 0) {
        return res;
    }
    const Object::ListType & args = sub_op.GetArgs();
    if (args.size() == 0) {
        debug( std::cout << " no args!" << std::endl << std::flush;);
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

oplist BaseMind::sightEatOperation(const Sight & op, Eat & sub_op)
{
    oplist res;
    script->Operation("sight_eat", op, res, &sub_op);
    return res;
}

oplist BaseMind::sightFireOperation(const Sight & op, Fire & sub_op)
{
    oplist res;
    script->Operation("sight_fire", op, res, &sub_op);
    return res;
}

oplist BaseMind::sightImaginaryOperation(const Sight & op, Imaginary & sub_op)
{
    oplist res;
    script->Operation("sight_imaginary", op, res, &sub_op);
    return res;
}

oplist BaseMind::sightMoveOperation(const Sight & op, Move & sub_op)
{
    debug( std::cout << "BaseMind::sightOperation(Sight, Move)" << std::endl << std::flush;);
    oplist res;
    if (script->Operation("sight_move", op, res, &sub_op) != 0) {
        return res;
    }
    const Object::ListType & args = sub_op.GetArgs();
    if (args.size() == 0) {
        debug( std::cout << " no args!" << std::endl << std::flush;);
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

oplist BaseMind::sightSetOperation(const Sight & op, Set & sub_op)
{
    oplist res;
    if (script->Operation("sight_set", op, res, &sub_op) != 0) {
        return res;
    }
    const Object::ListType & args = sub_op.GetArgs();
    if (args.size() == 0) {
        debug( std::cout << " no args!" << std::endl << std::flush;);
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

oplist BaseMind::sightTouchOperation(const Sight & op, Touch & sub_op)
{
    oplist res;
    script->Operation("sight_touch", op, res, &sub_op);
    return res;
}

oplist BaseMind::sightOtherOperation(const Sight & op, RootOperation & sub_op)
{
    debug( std::cout << "BaseMind::sightOperation(Sight, RootOperation)" << std::endl << std::flush;);
    oplist res;
    script->Operation("sight_undefined", op, res, &sub_op);
    return res;
}

oplist BaseMind::soundLoginOperation(const Sound & op, Login & sub_op)
{
    oplist res;
    script->Operation("sound_login", op, res, &sub_op);
    return res;
}

oplist BaseMind::soundActionOperation(const Sound & op, Action & sub_op)
{
    oplist res;
    script->Operation("sound_action", op, res, &sub_op);
    return res;
}

oplist BaseMind::soundCutOperation(const Sound & op, Cut & sub_op)
{
    oplist res;
    script->Operation("sound_cut", op, res, &sub_op);
    return res;
}

oplist BaseMind::soundChopOperation(const Sound & op, Chop & sub_op)
{
    oplist res;
    script->Operation("sound_chop", op, res, &sub_op);
    return res;
}

oplist BaseMind::soundCombineOperation(const Sound & op, Combine & sub_op)
{
    oplist res;
    script->Operation("sound_combine", op, res, &sub_op);
    return res;
}

oplist BaseMind::soundCreateOperation(const Sound & op, Create & sub_op)
{
    oplist res;
    script->Operation("sound_create", op, res, &sub_op);
    return res;
}

oplist BaseMind::soundDeleteOperation(const Sound & op, Delete & sub_op)
{
    oplist res;
    script->Operation("sound_delete", op, res, &sub_op);
    return res;
}

oplist BaseMind::soundDivideOperation(const Sound & op, Divide & sub_op)
{
    oplist res;
    script->Operation("sound_divide", op, res, &sub_op);
    return res;
}

oplist BaseMind::soundEatOperation(const Sound & op, Eat & sub_op)
{
    oplist res;
    script->Operation("sound_eat", op, res, &sub_op);
    return res;
}

oplist BaseMind::soundFireOperation(const Sound & op, Fire & sub_op)
{
    oplist res;
    script->Operation("sound_fire", op, res, &sub_op);
    return res;
}

oplist BaseMind::soundImaginaryOperation(const Sound & op, Imaginary & sub_op)
{
    oplist res;
    script->Operation("sound_imaginary", op, res, &sub_op);
    return res;
}

oplist BaseMind::soundMoveOperation(const Sound & op, Move & sub_op)
{
    oplist res;
    script->Operation("sound_move", op, res, &sub_op);
    return res;
}

oplist BaseMind::soundSetOperation(const Sound & op, Set & sub_op)
{
    oplist res;
    script->Operation("sound_set", op, res, &sub_op);
    return res;
}

oplist BaseMind::soundTouchOperation(const Sound & op, Touch & sub_op)
{
    oplist res;
    script->Operation("sound_touch", op, res, &sub_op);
    return res;
}

oplist BaseMind::soundTalkOperation(const Sound & op, Talk & sub_op)
{
    debug( std::cout << "BaseMind::soundOperation(Sound, Talk)" << std::endl << std::flush;);
    oplist res;
    script->Operation("sound_talk", op, res, &sub_op);
    return res;
}

oplist BaseMind::soundOtherOperation(const Sound & op, RootOperation & sub_op)
{
    debug( std::cout << "BaseMind::soundOperation(Sound, RootOperation)" << std::endl << std::flush;);
    oplist res;
    script->Operation("sound_undefined", op, res, &sub_op);
    return res;
}

oplist BaseMind::SoundOperation(const Sound & op)
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
        debug( std::cout << " no args!" << std::endl << std::flush;);
        return res;
    }
    Object obj = args.front();
    Root * op2 = utility::Object_asRoot(obj);
    if (op2->GetObjtype() == "op") {
        debug( std::cout << " args is an op!" << std::endl << std::flush;);
        res = callSoundOperation(op, *(RootOperation *)op2);
    }
    delete op2;
    return res;
}

oplist BaseMind::SightOperation(const Sight & op)
{
    if (!isAwake) { return oplist(); }
    debug( std::cout << "BaseMind::Operation(Sight)" << std::endl << std::flush;);
    // Deliver argument to sight things
    oplist res;
    if (script->Operation("sight", op, res) != 0) {
        debug( std::cout << " its in the script" << std::endl << std::flush;);
        return res;
    }
    const Object::ListType & args = op.GetArgs();
    if (args.size() == 0) {
        debug( std::cout << " no args!" << std::endl << std::flush;);
        return res;
    }
    Object obj = args.front();
    Root * op2 = utility::Object_asRoot(obj);
    if (op2->GetObjtype() == "op") {
        debug( std::cout << " args is an op!" << std::endl << std::flush;);
        res = callSightOperation(op, *(RootOperation *)op2);
        //std::string & op2type = op2->GetParents().front().AsString();
        //std::string subop = "sight_" + op2type;
        //script->Operation(subop, op, res, (RootOperation *)op2);
    } else /* if (op2->GetObjtype() == "object") */ {
        debug( std::cout << " arg is an entity!" << std::endl << std::flush;);
        map.add(obj);
    }
    delete op2;
    return res;
}

oplist BaseMind::AppearanceOperation(const Appearance & op)
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

oplist BaseMind::DisappearanceOperation(const Disappearance & op)
{
    if (!isAwake) { return oplist(); }
    oplist res;
    script->Operation("disappearance", op, res);
    // Not quite sure what to do to the map here, but should do something.
    return res;
}

oplist BaseMind::SaveOperation(const Save & op)
{
    oplist res;
    script->Operation("save", op, res);
    Object::MapType emap;
    std::cout << res.size() << " Got stuff from mind" << std::endl << std::flush;
    if ((res.size() != 0) && (res.front()->GetArgs().size() != 0)) {
        emap = res.front()->GetArgs().front().AsMap();
        // FIXME Operations created in python are leaked
    }
    Info * i = new Info(Info::Instantiate());
    emap["map"] = map.asObject();
    emap["id"] = getId();
    i->SetArgs(Object::ListType(1,emap));
    return oplist(1,i);
}

oplist BaseMind::LoadOperation(const Load & op)
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
RootOperation * BaseMind::get_op_name_and_sub(RootOperation & op, std::string & name)
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
    time.update(op.GetSeconds());
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

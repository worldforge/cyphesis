// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "BaseMind.h"

#include "MemMap_methods.h"

#include <common/utility.h>
#include <common/debug.h>
#include <common/op_switch.h>
#include <common/inheritance.h>

#include <common/Chop.h>
#include <common/Cut.h>
#include <common/Eat.h>
#include <common/Burn.h>
#include <common/Load.h>
#include <common/Save.h>

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

BaseMind::BaseMind(const std::string & id, const std::string & body_name)
                               : map(script), isAwake(true)
{
    setId(id);
    name = body_name;
    map.addObject(this);
    //BaseMind::time=WorldTime();

    subscribe("sight", OP_SIGHT);
    subscribe("sound", OP_SOUND);
    subscribe("save", OP_SAVE);
    subscribe("load", OP_LOAD);
    subscribe("tick", OP_TICK);
    subscribe("setup", OP_SETUP);
    subscribe("appearance", OP_APPEARANCE);
    subscribe("disappearance", OP_DISAPPEARANCE);

    sightSubscribe("create", OP_CREATE);
    sightSubscribe("delete", OP_DELETE);
    sightSubscribe("set", OP_SET);
    sightSubscribe("move", OP_MOVE);

    soundSubscribe("talk", OP_TALK);
}

BaseMind::~BaseMind()
{
    map.things.erase(getId());
    map.flushMap();
}

void BaseMind::scriptSubscribe(const std::string & op)
{
    std::string::size_type l = op.find("_");
    if (l == std::string::npos) {
        OpNo n = Inheritance::instance().opEnumerate(op);
        if (n != OP_INVALID) {
            debug(std::cout << "MINDSCRIPT requesting subscription to " << op
                            << std::endl << std::flush;);
            subscribe(op, n);
        } else {
            debug(std::cout << "MINDSCRIPT requesting subscription to " << op
                            << " but inheritance could not give me a reference"
                            << std::endl << std::flush;);
        }
        return;
    }
    // This only works cos sight and sound are the same length
    std::string subop = op.substr(6, std::string::npos);
    OpNo subno = Inheritance::instance().opEnumerate(subop);
    if (subno == OP_INVALID) {
        debug(std::cerr << "Mind script requested subscription to " << op
                        << " operations, of which the argument is " << subop
                        << " but I can't work out what it wants."
                        << std::endl << std::flush;);
        return;
    }
#if defined(__GNUC__) && __GNUC__ < 3
    if (!op.substr(0,6).compare("sight_")) {
#else
    if (!op.compare(0,6,"sight_")) {
#endif
        debug(std::cout << "MINDSCRIPT requesting subscription to sight of "
                        << subop << " ops" << std::endl << std::flush;);
        sightSubscribe(subop, subno);
#if defined(__GNUC__) && __GNUC__ < 3
    } else if (!op.substr(0,6).compare("sound_")) {
#else
    } else if (!op.compare(0,6,"sound_")) {
#endif
        debug(std::cout << "MINDSCRIPT requesting subscription to sound of "
                        << subop << " ops" << std::endl << std::flush;);
        soundSubscribe(subop, subno);
    } else {
        debug(std::cerr << "Mind script requested subscription to " << op
                        << " operations, of which the argument is " << subop
                        << " but op arguments for that kind of op are not yet"
                        << " supported by the mind scriptig interface"
                        << std::endl << std::flush;);
    }
}

OpVector BaseMind::sightLoginOperation(const Sight & op, Login & sub_op)
{
    OpVector res;
    script->Operation("sight_login", op, res, &sub_op);
    return res;
}

OpVector BaseMind::sightCombineOperation(const Sight & op, Combine & sub_op)
{
    OpVector res;
    script->Operation("sight_combine", op, res, &sub_op);
    return res;
}

OpVector BaseMind::sightDivideOperation(const Sight & op, Divide & sub_op)
{
    OpVector res;
    script->Operation("sight_divide", op, res, &sub_op);
    return res;
}

OpVector BaseMind::sightTalkOperation(const Sight & op, Talk & sub_op)
{
    OpVector res;
    script->Operation("sight_talk", op, res, &sub_op);
    return res;
}

OpVector BaseMind::sightActionOperation(const Sight & op, Action & sub_op)
{
    OpVector res;
    script->Operation("sight_action", op, res, &sub_op);
    return res;
}

OpVector BaseMind::sightChopOperation(const Sight & op, Chop & sub_op)
{
    OpVector res;
    script->Operation("sight_chop", op, res, &sub_op);
    return res;
}

OpVector BaseMind::sightCreateOperation(const Sight & op, Create & sub_op)
{
    OpVector res;
    if (script->Operation("sight_create", op, res, &sub_op) != 0) {
        return res;
    }
    const Fragment::ListType & args = sub_op.GetArgs();
    if (args.empty() || !args.front().IsMap()) {
        debug( std::cout << " no args!" << std::endl << std::flush;);
        return res;
    }
    const Fragment::MapType & obj = args.front().AsMap();
    map.add(obj);
    return res;
}

OpVector BaseMind::sightCutOperation(const Sight & op, Cut & sub_op)
{
    OpVector res;
    script->Operation("sight_cut", op, res, &sub_op);
    return res;
}

OpVector BaseMind::sightDeleteOperation(const Sight & op, Delete & sub_op)
{
    debug( std::cout << "Sight Delete operation" << std::endl << std::flush;);
    OpVector res;
    if (script->Operation("sight_delete", op, res, &sub_op) != 0) {
        return res;
    }
    const Fragment::ListType & args = sub_op.GetArgs();
    if (args.empty() || !args.front().IsMap()) {
        debug( std::cout << " no args!" << std::endl << std::flush;);
        return res;
    }
    const Fragment::MapType & obj = args.front().AsMap();
    Fragment::MapType::const_iterator I = obj.find("id");
    if ((I != obj.end()) && (I->second.IsString())) {
        map.del(I->second.AsString());
    } else {
        debug(std::cout << "args has no string id" << std::endl << std::flush;);
    }
    return res;
}

OpVector BaseMind::sightEatOperation(const Sight & op, Eat & sub_op)
{
    OpVector res;
    script->Operation("sight_eat", op, res, &sub_op);
    return res;
}

OpVector BaseMind::sightBurnOperation(const Sight & op, Burn & sub_op)
{
    OpVector res;
    script->Operation("sight_burn", op, res, &sub_op);
    return res;
}

OpVector BaseMind::sightImaginaryOperation(const Sight & op, Imaginary & sub_op)
{
    OpVector res;
    script->Operation("sight_imaginary", op, res, &sub_op);
    return res;
}

OpVector BaseMind::sightMoveOperation(const Sight & op, Move & sub_op)
{
    debug( std::cout << "BaseMind::sightOperation(Sight, Move)" << std::endl << std::flush;);
    OpVector res;
    if (script->Operation("sight_move", op, res, &sub_op) != 0) {
        return res;
    }
    const Fragment::ListType & args = sub_op.GetArgs();
    if (args.empty() || !args.front().IsMap()) {
        debug( std::cout << " no args!" << std::endl << std::flush;);
        return res;
    }
    const Fragment::MapType & obj = args.front().AsMap();
    map.update(obj);
    return res;
}

OpVector BaseMind::sightSetOperation(const Sight & op, Set & sub_op)
{
    OpVector res;
    if (script->Operation("sight_set", op, res, &sub_op) != 0) {
        return res;
    }
    const Fragment::ListType & args = sub_op.GetArgs();
    if (args.empty() || !args.front().IsMap()) {
        debug( std::cout << " no args!" << std::endl << std::flush;);
        return res;
    }
    const Fragment::MapType & obj = args.front().AsMap();
    map.update(obj);
    return res;
}

OpVector BaseMind::sightTouchOperation(const Sight & op, Touch & sub_op)
{
    OpVector res;
    script->Operation("sight_touch", op, res, &sub_op);
    return res;
}

OpVector BaseMind::sightOtherOperation(const Sight & op, RootOperation & sub_op)
{
    debug( std::cout << "BaseMind::sightOperation(Sight, RootOperation)" << std::endl << std::flush;);
    OpVector res;
    script->Operation("sight_undefined", op, res, &sub_op);
    return res;
}

OpVector BaseMind::soundLoginOperation(const Sound & op, Login & sub_op)
{
    OpVector res;
    script->Operation("sound_login", op, res, &sub_op);
    return res;
}

OpVector BaseMind::soundActionOperation(const Sound & op, Action & sub_op)
{
    OpVector res;
    script->Operation("sound_action", op, res, &sub_op);
    return res;
}

OpVector BaseMind::soundCutOperation(const Sound & op, Cut & sub_op)
{
    OpVector res;
    script->Operation("sound_cut", op, res, &sub_op);
    return res;
}

OpVector BaseMind::soundChopOperation(const Sound & op, Chop & sub_op)
{
    OpVector res;
    script->Operation("sound_chop", op, res, &sub_op);
    return res;
}

OpVector BaseMind::soundCombineOperation(const Sound & op, Combine & sub_op)
{
    OpVector res;
    script->Operation("sound_combine", op, res, &sub_op);
    return res;
}

OpVector BaseMind::soundCreateOperation(const Sound & op, Create & sub_op)
{
    OpVector res;
    script->Operation("sound_create", op, res, &sub_op);
    return res;
}

OpVector BaseMind::soundDeleteOperation(const Sound & op, Delete & sub_op)
{
    OpVector res;
    script->Operation("sound_delete", op, res, &sub_op);
    return res;
}

OpVector BaseMind::soundDivideOperation(const Sound & op, Divide & sub_op)
{
    OpVector res;
    script->Operation("sound_divide", op, res, &sub_op);
    return res;
}

OpVector BaseMind::soundEatOperation(const Sound & op, Eat & sub_op)
{
    OpVector res;
    script->Operation("sound_eat", op, res, &sub_op);
    return res;
}

OpVector BaseMind::soundBurnOperation(const Sound & op, Burn & sub_op)
{
    OpVector res;
    script->Operation("sound_burn", op, res, &sub_op);
    return res;
}

OpVector BaseMind::soundImaginaryOperation(const Sound & op, Imaginary & sub_op)
{
    OpVector res;
    script->Operation("sound_imaginary", op, res, &sub_op);
    return res;
}

OpVector BaseMind::soundMoveOperation(const Sound & op, Move & sub_op)
{
    OpVector res;
    script->Operation("sound_move", op, res, &sub_op);
    return res;
}

OpVector BaseMind::soundSetOperation(const Sound & op, Set & sub_op)
{
    OpVector res;
    script->Operation("sound_set", op, res, &sub_op);
    return res;
}

OpVector BaseMind::soundTouchOperation(const Sound & op, Touch & sub_op)
{
    OpVector res;
    script->Operation("sound_touch", op, res, &sub_op);
    return res;
}

OpVector BaseMind::soundTalkOperation(const Sound & op, Talk & sub_op)
{
    debug( std::cout << "BaseMind::soundOperation(Sound, Talk)" << std::endl << std::flush;);
    OpVector res;
    script->Operation("sound_talk", op, res, &sub_op);
    return res;
}

OpVector BaseMind::soundOtherOperation(const Sound & op, RootOperation & sub_op)
{
    debug( std::cout << "BaseMind::soundOperation(Sound, RootOperation)" << std::endl << std::flush;);
    OpVector res;
    script->Operation("sound_undefined", op, res, &sub_op);
    return res;
}

OpVector BaseMind::SoundOperation(const Sound & op)
{
    // Louder sounds might eventually make character wake up
    if (!isAwake) { return OpVector(); }
    // Deliver argument to sound things
    OpVector res;
    if (script->Operation("sound", op, res) != 0) {
        return res;
    }
    const Fragment::ListType & args = op.GetArgs();
    if (args.empty() || !args.front().IsMap()) {
        debug( std::cout << " no args!" << std::endl << std::flush;);
        return res;
    }
    const Fragment::MapType & obj = args.front().AsMap();
    RootOperation op2;
    bool isOp = utility::Object_asOperation(obj, op2);
    if (isOp) {
        debug( std::cout << " args is an op!" << std::endl << std::flush;);
        res = callSoundOperation(op, op2);
    }
    return res;
}

OpVector BaseMind::SightOperation(const Sight & op)
{
    if (!isAwake) { return OpVector(); }
    debug( std::cout << "BaseMind::Operation(Sight)" << std::endl << std::flush;);
    // Deliver argument to sight things
    OpVector res;
    if (script->Operation("sight", op, res) != 0) {
        debug( std::cout << " its in the script" << std::endl << std::flush;);
        return res;
    }
    const Fragment::ListType & args = op.GetArgs();
    if (args.empty() || !args.front().IsMap()) {
        debug( std::cout << " no args!" << std::endl << std::flush;);
        return res;
    }
    const Fragment::MapType & obj = args.front().AsMap();
    RootOperation op2;
    bool isOp = utility::Object_asOperation(obj, op2);
    if (isOp) {
        debug( std::cout << " args is an op!" << std::endl << std::flush;);
        res = callSightOperation(op, op2);
    } else /* if (op2->GetObjtype() == "object") */ {
        debug( std::cout << " arg is an entity!" << std::endl << std::flush;);
        map.add(obj);
    }
    return res;
}

OpVector BaseMind::AppearanceOperation(const Appearance & op)
{
    if (!isAwake) { return OpVector(); }
    OpVector res;
    script->Operation("appearance", op, res);
    const Fragment::ListType & args = op.GetArgs();
    Fragment::ListType::const_iterator I;
    for(I = args.begin(); I != args.end(); I++) {
        map.getAdd(I->AsMap().find("id")->second.AsString());
    }
    return res;
}

OpVector BaseMind::DisappearanceOperation(const Disappearance & op)
{
    if (!isAwake) { return OpVector(); }
    OpVector res;
    script->Operation("disappearance", op, res);
    // Not quite sure what to do to the map here, but should do something.
    return res;
}

OpVector BaseMind::SaveOperation(const Save & op)
{
    OpVector res;
    script->Operation("save", op, res);
    Fragment::MapType emap;
    debug(std::cout << "Got " << res.size() << " stuff to save from mind"
                    << std::endl << std::flush;);
    OpVector::const_iterator I = res.begin();
    if ((I != res.end()) && !((*I)->GetArgs().empty())) {
        emap = (*I)->GetArgs().front().AsMap();
    }
    for (; I != res.end(); ++I) {
        delete *I;
    }
    Info * i = new Info(Info::Instantiate());
    emap["map"] = map.asObject();
    emap["id"] = getId();
    i->SetArgs(Fragment::ListType(1,emap));
    return OpVector(1,i);
}

OpVector BaseMind::LoadOperation(const Load & op)
{
    OpVector res;
    script->Operation("load", op, res);
    if (!op.GetArgs().empty()) {
        const Fragment::MapType & emap = op.GetArgs().front().AsMap();
        Fragment::MapType::const_iterator I = emap.find("map");
        if ((I != emap.end()) && I->second.IsMap()) {
            const Fragment::MapType & memmap = I->second.AsMap();
            Fragment::MapType::const_iterator J = memmap.begin();
            for(; J != memmap.end(); J++) {
                map.add(J->second.AsMap());
            }
        }
    }
    return OpVector();
}

OpVector BaseMind::operation(const RootOperation & op)
{
    // This might end up being quite tricky to do

    // In the python the following happens here:
    //   Find out if the op refers to any ids we don't know about.
    //   If so create look operations to those ids
    //   Set the minds time and date 
    OpVector res;
    time.update(op.GetSeconds());
    map.getAdd(op.GetFrom());
    RootOperation * look;
    while ((look = map.lookId()) != NULL) {
        res.push_back(look);
    }
    OpVector res2 = callOperation(op);
    for(OpVector::const_iterator I = res2.begin(); I != res2.end(); I++) {
        res.push_back(*I);
    }
    res2.clear();
    script->Operation("call_triggers", op, res2);
    for(OpVector::const_iterator I = res2.begin(); I != res2.end(); I++) {
        res.push_back(*I);
    }
    return res;
}

OpVector BaseMind::callSightOperation(const Sight& op, RootOperation& sub_op) {
    map.getAdd(sub_op.GetFrom());
    OpNo op_no = opEnumerate(sub_op, opSightLookup);
    if (debug_flag && (op_no == OP_INVALID)) {
        std::cout << getId() << " could not deliver sight of "
                  << sub_op.GetParents().front().AsString()
                  << std::endl << std::flush;
    }
    SUB_OP_SWITCH(op, op_no, sight, sub_op)
    return OpVector();
}

OpVector BaseMind::callSoundOperation(const Sound& op, RootOperation& sub_op) {
    map.getAdd(sub_op.GetFrom());
    OpNo op_no = opEnumerate(sub_op, opSoundLookup);
    if (debug_flag && (op_no == OP_INVALID)) {
        std::cout << getId() << " could not deliver sound of "
                  << sub_op.GetParents().front().AsString()
                  << std::endl << std::flush;
    }
    SUB_OP_SWITCH(op, op_no, sound, sub_op)
    return OpVector();
}

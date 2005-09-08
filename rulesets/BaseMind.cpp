// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "BaseMind.h"

#include "Script.h"

#include "common/log.h"
#include "common/utility.h"
#include "common/debug.h"
#include "common/op_switch.h"
#include "common/inheritance.h"

#include "common/Chop.h"
#include "common/Cut.h"
#include "common/Eat.h"
#include "common/Burn.h"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/RootEntity.h>

using Atlas::Message::MapType;
using Atlas::Message::ListType;
using Atlas::Objects::Root;
using Atlas::Objects::Entity::RootEntity;

using Atlas::Objects::smart_dynamic_cast;

static const bool debug_flag = false;

BaseMind::BaseMind(const std::string & id, const std::string & body_name)
                               : MemEntity(id), m_map(m_script), m_isAwake(true)
{
    // setId(id);
    m_name = body_name;
    setVisible(true);
    m_map.addEntity(this);
    //BaseMind::time=WorldTime();

    subscribe("sight", OP_SIGHT);
    subscribe("sound", OP_SOUND);
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
    m_map.m_entities.erase(getId());
    // FIXME Remove this once MemMap uses parent refcounting
    m_location.m_loc = 0;
    debug(std::cout << getId() << ":" << getType() << " flushing mind with "
                    << m_map.getEntities().size() << " entities in it"
                    << std::endl << std::flush;);
    m_map.flush();
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

void BaseMind::sightLoginOperation(const Operation & op, Operation & sub_op, OpVector & res)
{
    m_script->Operation("sight_login", op, res, &sub_op);
}

void BaseMind::sightCombineOperation(const Operation & op, Operation & sub_op, OpVector & res)
{
    m_script->Operation("sight_combine", op, res, &sub_op);
}

void BaseMind::sightDivideOperation(const Operation & op, Operation & sub_op, OpVector & res)
{
    m_script->Operation("sight_divide", op, res, &sub_op);
}

void BaseMind::sightTalkOperation(const Operation & op, Operation & sub_op, OpVector & res)
{
    m_script->Operation("sight_talk", op, res, &sub_op);
}

void BaseMind::sightActionOperation(const Operation & op, Operation & sub_op, OpVector & res)
{
    m_script->Operation("sight_action", op, res, &sub_op);
}

void BaseMind::sightChopOperation(const Operation & op, Operation & sub_op, OpVector & res)
{
    m_script->Operation("sight_chop", op, res, &sub_op);
}

void BaseMind::sightCreateOperation(const Operation & op, Operation & sub_op, OpVector & res)
{
    if (m_script->Operation("sight_create", op, res, &sub_op) != 0) {
        return;
    }
    const std::vector<Root> & args = sub_op->getArgs();
    if (args.empty()) {
        debug( std::cout << " no args!" << std::endl << std::flush;);
        return;
    }
    RootEntity ent(smart_dynamic_cast<RootEntity>(args.front()));
    if (!ent.isValid()) {
        log(ERROR, "Got sight(create) of non-entity");
        return;
    }
    // This does not send a look, so anything added this way will not
    // get flagged as visible until we get an appearance. This is important.
    m_map.updateAdd(ent, op->getSeconds());
}

void BaseMind::sightCutOperation(const Operation & op, Operation & sub_op, OpVector & res)
{
    m_script->Operation("sight_cut", op, res, &sub_op);
}

void BaseMind::sightDeleteOperation(const Operation & op, Operation & sub_op, OpVector & res)
{
    debug( std::cout << "Sight Delete operation" << std::endl << std::flush;);
    if (m_script->Operation("sight_delete", op, res, &sub_op) != 0) {
        return;
    }
    const std::vector<Root> & args = sub_op->getArgs();
    if (args.empty()) {
        debug( std::cout << " no args!" << std::endl << std::flush;);
        return;
    }
    const Root & obj = args.front();
    const std::string & id = obj->getId();
    if (!id.empty()) {
        m_map.del(obj->getId());
    } else {
        log(WARNING, "Sight Delete with no ID");
    }
}

void BaseMind::sightEatOperation(const Operation & op, Operation & sub_op, OpVector & res)
{
    m_script->Operation("sight_eat", op, res, &sub_op);
}

void BaseMind::sightBurnOperation(const Operation & op, Operation & sub_op, OpVector & res)
{
    m_script->Operation("sight_burn", op, res, &sub_op);
}

void BaseMind::sightImaginaryOperation(const Operation & op, Operation & sub_op, OpVector & res)
{
    m_script->Operation("sight_imaginary", op, res, &sub_op);
}

void BaseMind::sightMoveOperation(const Operation & op, Operation & sub_op, OpVector & res)
{
    debug( std::cout << "BaseMind::sightOperation(Sight, Move)" << std::endl << std::flush;);
    if (m_script->Operation("sight_move", op, res, &sub_op) != 0) {
        return;
    }
    const std::vector<Root> & args = sub_op->getArgs();
    if (args.empty()) {
        debug( std::cout << " no args!" << std::endl << std::flush;);
        return;
    }
    RootEntity ent(smart_dynamic_cast<RootEntity>(args.front()));
    if (!ent.isValid()) {
        log(ERROR, "Got sight(move) of non-entity");
        return;
    }
    m_map.updateAdd(ent, op->getSeconds());
}

void BaseMind::sightSetOperation(const Operation & op, Operation & sub_op, OpVector & res)
{
    if (m_script->Operation("sight_set", op, res, &sub_op) != 0) {
        return;
    }
    const std::vector<Root> & args = sub_op->getArgs();
    if (args.empty()) {
        debug( std::cout << " no args!" << std::endl << std::flush;);
        return;
    }
    RootEntity ent(smart_dynamic_cast<RootEntity>(args.front()));
    if (!ent.isValid()) {
        log(ERROR, "Got sight(set) of non-entity");
        return;
    }
    m_map.updateAdd(ent, op->getSeconds());
}

void BaseMind::sightTouchOperation(const Operation & op, Operation & sub_op, OpVector & res)
{
    m_script->Operation("sight_touch", op, res, &sub_op);
}

void BaseMind::sightOtherOperation(const Operation & op, Operation & sub_op, OpVector & res)
{
    debug( std::cout << "BaseMind::sightOperation(Sight, Operation)" << std::endl << std::flush;);

    std::string event_name("sight_");
    event_name += sub_op->getParents().front();

    m_script->Operation(event_name, op, res, &sub_op);
}

void BaseMind::soundLoginOperation(const Operation & op, Operation & sub_op, OpVector & res)
{
    m_script->Operation("sound_login", op, res, &sub_op);
}

void BaseMind::soundActionOperation(const Operation & op, Operation & sub_op, OpVector & res)
{
    m_script->Operation("sound_action", op, res, &sub_op);
}

void BaseMind::soundCutOperation(const Operation & op, Operation & sub_op, OpVector & res)
{
    m_script->Operation("sound_cut", op, res, &sub_op);
}

void BaseMind::soundChopOperation(const Operation & op, Operation & sub_op, OpVector & res)
{
    m_script->Operation("sound_chop", op, res, &sub_op);
}

void BaseMind::soundCombineOperation(const Operation & op, Operation & sub_op, OpVector & res)
{
    m_script->Operation("sound_combine", op, res, &sub_op);
}

void BaseMind::soundCreateOperation(const Operation & op, Operation & sub_op, OpVector & res)
{
    m_script->Operation("sound_create", op, res, &sub_op);
}

void BaseMind::soundDeleteOperation(const Operation & op, Operation & sub_op, OpVector & res)
{
    m_script->Operation("sound_delete", op, res, &sub_op);
}

void BaseMind::soundDivideOperation(const Operation & op, Operation & sub_op, OpVector & res)
{
    m_script->Operation("sound_divide", op, res, &sub_op);
}

void BaseMind::soundEatOperation(const Operation & op, Operation & sub_op, OpVector & res)
{
    m_script->Operation("sound_eat", op, res, &sub_op);
}

void BaseMind::soundBurnOperation(const Operation & op, Operation & sub_op, OpVector & res)
{
    m_script->Operation("sound_burn", op, res, &sub_op);
}

void BaseMind::soundImaginaryOperation(const Operation & op, Operation & sub_op, OpVector & res)
{
    m_script->Operation("sound_imaginary", op, res, &sub_op);
}

void BaseMind::soundMoveOperation(const Operation & op, Operation & sub_op, OpVector & res)
{
    m_script->Operation("sound_move", op, res, &sub_op);
}

void BaseMind::soundSetOperation(const Operation & op, Operation & sub_op, OpVector & res)
{
    m_script->Operation("sound_set", op, res, &sub_op);
}

void BaseMind::soundTouchOperation(const Operation & op, Operation & sub_op, OpVector & res)
{
    m_script->Operation("sound_touch", op, res, &sub_op);
}

void BaseMind::soundTalkOperation(const Operation & op, Operation & sub_op, OpVector & res)
{
    debug( std::cout << "BaseMind::soundOperation(Sound, Talk)" << std::endl << std::flush;);
    m_script->Operation("sound_talk", op, res, &sub_op);
}

void BaseMind::soundOtherOperation(const Operation & op, Operation & sub_op, OpVector & res)
{
    debug( std::cout << "BaseMind::soundOperation(Sound, Operation)" << std::endl << std::flush;);

    std::string event_name("sound_");
    event_name += sub_op->getParents().front();

    m_script->Operation(event_name, op, res, &sub_op);
}

void BaseMind::SoundOperation(const Operation & op, OpVector & res)
{
    // Louder sounds might eventually make character wake up
    if (!m_isAwake) { return; }
    // Deliver argument to sound things
    if (m_script->Operation("sound", op, res) != 0) {
        return;
    }
    const std::vector<Root> & args = op->getArgs();
    if (args.empty()) {
        debug( std::cout << " no args!" << std::endl << std::flush;);
        return;
    }
    const Root & arg = args.front();
    Operation op2(Atlas::Objects::smart_dynamic_cast<Operation>(arg));
    if (op2.isValid()) {
        debug( std::cout << " args is an op!" << std::endl << std::flush;);
        callSoundOperation(op, op2, res);
    }
}

void BaseMind::SightOperation(const Operation & op, OpVector & res)
{
    if (!m_isAwake) { return; }
    debug( std::cout << "BaseMind::Operation(Sight)" << std::endl << std::flush;);
    // Deliver argument to sight things
    if (m_script->Operation("sight", op, res) != 0) {
        debug( std::cout << " its in the script" << std::endl << std::flush;);
        return;
    }
    const std::vector<Root> & args = op->getArgs();
    if (args.empty()) {
        debug( std::cout << " no args!" << std::endl << std::flush;);
        return;
    }
    const Root & arg = args.front();
    Operation op2(Atlas::Objects::smart_dynamic_cast<Operation>(arg));
    if (op2.isValid()) {
        debug( std::cout << " args is an op!" << std::endl << std::flush;);
        callSightOperation(op, op2, res);
    } else /* if (op2->getObjtype() == "object") */ {
        RootEntity ent(Atlas::Objects::smart_dynamic_cast<RootEntity>(arg));
        if (!ent.isValid()) {
            log(ERROR, "Arg of sigh operation is not an op or an entity");
            return;
        }
        debug( std::cout << " arg is an entity!" << std::endl << std::flush;);
        MemEntity * me = m_map.updateAdd(ent, op->getSeconds());
        if (me != 0) {
            me->setVisible();
        }
    }
}

void BaseMind::AppearanceOperation(const Operation & op, OpVector & res)
{
    if (!m_isAwake) { return; }
    m_script->Operation("appearance", op, res);
    const std::vector<Root> & args = op->getArgs();
    std::vector<Root>::const_iterator Iend = args.end();
    for (std::vector<Root>::const_iterator I = args.begin(); I != Iend; ++I) {
        const std::string & id = (*I)->getId();
        if (id.empty()) { continue; }
        MemEntity * me = m_map.getAdd(id);
        if (me != 0) {
            me->update(op->getSeconds());
            me->setVisible();
        }
    }
}

void BaseMind::DisappearanceOperation(const Operation & op, OpVector & res)
{
    if (!m_isAwake) { return; }
    m_script->Operation("disappearance", op, res);
    const std::vector<Root> & args = op->getArgs();
    std::vector<Root>::const_iterator Iend = args.end();
    for (std::vector<Root>::const_iterator I = args.begin(); I != Iend; ++I) {
        const std::string & id = (*I)->getId();
        if (id.empty()) { continue; }
        MemEntity * me = m_map.get(id);
        if (me != 0) {
            me->update(op->getSeconds());
            me->setVisible(false);
        }
    }
}

void BaseMind::operation(const Operation & op, OpVector & res)
{
    // This might end up being quite tricky to do

    // In the python the following happens here:
    //   Find out if the op refers to any ids we don't know about.
    //   If so create look operations to those ids
    //   Set the minds time and date 
    debug(std::cout << "BaseMind::operation("
                    << op->getParents().front() << ")"
                    << std::endl << std::flush;);
    m_time.update((int)op->getSeconds());
    m_map.check(op->getSeconds());
    m_map.getAdd(op->getFrom());
    Operation look(0);
    while ((look = m_map.lookId()).isValid()) {
        res.push_back(look);
    }
    m_script->Operation("call_triggers", op, res);
    callOperation(op, res);
}

void BaseMind::callSightOperation(const Operation & op,
                                  Operation & sub_op,
                                  OpVector & res)
{
    m_map.getAdd(sub_op->getFrom());
    OpNo op_no = opEnumerate(sub_op, opSightLookup);
    if (debug_flag && (op_no == OP_INVALID)) {
        std::cout << getId() << " could not deliver sight of "
                  << sub_op->getParents().front()
                  << std::endl << std::flush;
    }
    SUB_OP_SWITCH(op, op_no, res, sight, sub_op)
}

void BaseMind::callSoundOperation(const Operation & op,
                                  Operation & sub_op,
                                  OpVector & res)
{
    m_map.getAdd(sub_op->getFrom());
    OpNo op_no = opEnumerate(sub_op, opSoundLookup);
    if (debug_flag && (op_no == OP_INVALID)) {
        std::cout << getId() << " could not deliver sound of "
                  << sub_op->getParents().front()
                  << std::endl << std::flush;
    }

#if 0
    const MemEntityDict & ents = m_map.getEntities();
    MemEntityDict::const_iterator Iend = ents.end();
    for (MemEntityDict::const_iterator I = ents.begin(); I != Iend; ++I) {
        std::cout << I->second->getId() << ":" << I->second->getType() << " is "
                  << ( I->second->isVisible() ? "visible " : "hid " )
                  << I->second->lastSeen()
                  << std::endl << std::flush;
    }
#endif

    SUB_OP_SWITCH(op, op_no, res, sound, sub_op)
}

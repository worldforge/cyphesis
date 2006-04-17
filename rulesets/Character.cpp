// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000,2001 Alistair Riddoch
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

#include "Character.h"

#include "Pedestrian.h"
#include "Combat.h"
#include "MindFactory.h"
#include "BaseMind.h"
#include "Script.h"
#include "World.h"
#include "StatisticsProperty.h"

#include "common/op_switch.h"
#include "common/const.h"
#include "common/debug.h"
#include "common/globals.h"
#include "common/log.h"
#include "common/inheritance.h"
#include "common/serialno.h"
#include "common/compose.hpp"

#include "common/Add.h"
#include "common/Attack.h"
#include "common/Burn.h"
#include "common/Chop.h"
#include "common/Cut.h"
#include "common/Eat.h"
#include "common/Nourish.h"
#include "common/Setup.h"
#include "common/Tick.h"
#include "common/Unseen.h"
#include "common/Update.h"

#include <wfmath/atlasconv.h>

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>

#include <cassert>

using Atlas::Message::Element;
using Atlas::Message::ListType;
using Atlas::Objects::Root;
using Atlas::Objects::Operation::Set;
using Atlas::Objects::Operation::Sight;
using Atlas::Objects::Operation::Sound;
using Atlas::Objects::Operation::Tick;
using Atlas::Objects::Operation::Look;
using Atlas::Objects::Operation::Move;
using Atlas::Objects::Operation::Setup;
using Atlas::Objects::Operation::Action;
using Atlas::Objects::Operation::Unseen;
using Atlas::Objects::Operation::Nourish;
using Atlas::Objects::Operation::Appearance;
using Atlas::Objects::Operation::Wield;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Entity::RootEntity;

using Atlas::Objects::smart_dynamic_cast;

static const bool debug_flag = false;

const double Character::energyConsumption = 0.001;
const double Character::foodConsumption = 0.1;
const double Character::weightConsumption = 0.1;
const double Character::energyGain = 0.5;
const double Character::energyLoss = 0.1;
const double Character::weightGain = 0.5;


void Character::metabolise(OpVector & res, double ammount)
{
    // Currently handles energy
    // We should probably call this whenever the entity performs a movement.
    Anonymous set_arg;
    set_arg->setId(getId());
    if ((m_status > (1.5 + energyLoss)) && (m_mass < m_maxMass)) {
        m_status = m_status - energyLoss;
        set_arg->setAttr("mass", m_mass + weightGain);
    }
    double energyUsed = energyConsumption * ammount;
    if ((m_status <= energyUsed) && (m_mass > weightConsumption)) {
        set_arg->setAttr("status", m_status - energyUsed + energyGain);
        set_arg->setAttr("mass", m_mass - weightConsumption);
    } else {
        set_arg->setAttr("status", m_status - energyUsed);
    }
    if (m_drunkness > 0) {
        set_arg->setAttr("drunkness", m_drunkness - 0.1);
    }
    if (m_stamina < 1. && m_task == 0 && !m_movement.updateNeeded(m_location)) {
        set_arg->setAttr("stamina", 1.);
    }

    Set s;
    s->setTo(getId());
    s->setArgs1(set_arg);

    res.push_back(s);
}

void Character::wieldDropped()
{
    Wield wield;
    wield->setTo(getId());
    sendWorld(wield);
}

Character::Character(const std::string & id, long intId) :
                                            Character_parent(id, intId),
                                            m_statistics(*this),
                                            m_movement(*new Pedestrian(*this)),
                                            m_task(0), m_isAlive(true),
                                            m_stamina(1.), m_drunkness(0.),
                                            m_sex("female"),
                                            m_food(0), m_maxMass(100),
                                            m_mind(0), m_externalMind(0)
{
    m_mass = 60;
    m_location.setBBox(BBox(WFMath::Point<3>(-0.25, -0.25, 0),
                            WFMath::Point<3>(0.25, 0.25, 2)));

    m_properties["stamina"] = new Property<double>(m_stamina, 0);
    m_properties["drunkness"] = new Property<double>(m_drunkness, a_drunk);
    m_properties["sex"] = new Property<std::string>(m_sex, a_sex);
    m_properties["statistics"] = new StatisticsProperty(m_statistics, 0);
    m_properties["right_hand_wield"] = new Property<std::string>(m_rightHandWield, a_rwield);
}

Character::~Character()
{
    delete &m_movement;
    if (m_mind != 0) {
        delete m_mind;
    }
    if (m_externalMind != 0) {
        delete m_externalMind;
    }
}

void Character::setTask(Task * task)
{
    if (m_task != 0) {
        clearTask();
    }
    m_task = task;
    task->incRef();
}

void Character::clearTask()
{
    if (m_task == 0) {
        log(ERROR, "Character.clearTask: No task currently set");
        return;
    }
    m_task->decRef();
    m_task = 0;
}

void Character::ImaginaryOperation(const Operation & op, OpVector & res)
{
    Sight s;
    s->setArgs1(op);
    res.push_back(s);
}

void Character::SetupOperation(const Operation & op, OpVector & res)
{
    debug( std::cout << "CHaracter::SetupOperation()" << std::endl
                     << std::flush;);

    if (op->hasAttr("sub_to")) {
        debug( std::cout << "Setup has sub_to" << std::endl << std::flush;);
        return;
    }

    Appearance app;
    Anonymous app_arg;
    app_arg->setId(getId());
    app->setArgs1(app_arg);

    res.push_back(app);

    if (m_script->operation("setup", op, res) != 0) {
        return;
    }

    if (0 == m_externalMind) {
        // This ensures that newly created player characters don't get
        // bogged down with an NPC mind. In the short term this
        // takes away PC programmability.
        // FIXME Characters restored from the database will still get
        // AI minds, so  we need to handle them somehow differently.
        // Perhaps the Restore op (different from Setup op) is needed?

        m_mind = MindFactory::instance()->newMind(getId(), getIntId(),
                                                  m_name, m_type);

        Operation s(op.copy());
        // THis is so not the right thing to do
        s->setAttr("sub_to", "mind");
        res.push_back(s);

        Look l;
        l->setTo(getId());
        res.push_back(l);
    }

    Tick tick;
    tick->setTo(getId());
    res.push_back(tick);
}

void Character::TickOperation(const Operation & op, OpVector & res)
{
    Element sub_to;
    if (op->copyAttr("sub_to", sub_to) == 0) {
        debug( std::cout << "Has sub_to" << std::endl << std::flush;);
        if (!sub_to.isString()) {
            error(op, "Tick op sub_to is not string", res, getId());
            return;
        }
        if (sub_to.String() == "task") {
            if (m_task == 0) {
                log(ERROR, "Got Tick op for task, but task is null");
                return;
            }
            m_task->TickOperation(op, res);
            if (m_task == 0) {
                log(ERROR, "task is NULL after it processed tick op");
                return;
            }
            if (m_task->obsolete()) {
                clearTask();
            }
        }
        return;
    }
    debug(std::cout << "================================" << std::endl
                    << std::flush;);
    const std::vector<Root> & args = op->getArgs();
    if (!args.empty()) {
        // Deal with movement.
        const Root & arg = args.front();
        Element serialno;
        if (arg->copyAttr("serialno", serialno) == 0 && (serialno.isInt())) {
            if (serialno.asInt() < m_movement.serialno()) {
                debug(std::cout << "Old tick" << std::endl << std::flush;);
                return;
            }
        } else {
            log(ERROR, "Character::TickOperation: No serialno in tick arg");
        }
        Location return_location;
        if (m_movement.getUpdatedLocation(return_location)) {
            return;
        }
        res.push_back(m_movement.generateMove(return_location));
        Anonymous tick_arg;
        tick_arg->setName("move");
        tick_arg->setAttr("serialno", m_movement.serialno());
        Tick tickOp;
        tickOp->setTo(getId());
        tickOp->setFutureSeconds(m_movement.getTickAddition(return_location.pos(), return_location.velocity()));
        tickOp->setArgs1(tick_arg);
        res.push_back(tickOp);
    } else {
        m_script->operation("tick", op, res);

        // DIGEST
        if ((m_food >= foodConsumption) && (m_status < 2)) {
            // It is important that the metabolise bit is done next, as this
            // handles the status change
            m_status = m_status + foodConsumption;
            m_food = m_food - foodConsumption;

            Set s;
            Anonymous food_ent;
            food_ent->setId(getId());
            food_ent->setAttr("food", m_food);
            s->setTo(getId());
            s->setArgs1(food_ent);

            Sight si;
            si->setTo(getId());
            si->setArgs1(s);
            res.push_back(si);
        }

        // METABOLISE
        metabolise(res);
        
        // TICK
        Tick tickOp;
        tickOp->setTo(getId());
        tickOp->setFutureSeconds(consts::basic_tick * 30);
        res.push_back(tickOp);
    }
}

void Character::TalkOperation(const Operation & op, OpVector & res)
{
    debug( std::cout << "Character::OPeration(Talk)" << std::endl<<std::flush;);
    Sound s;
    s->setArgs1(op);
    res.push_back(s);
}

void Character::EatOperation(const Operation & op, OpVector & res)
{
    // This is identical to Food::Operation(Eat &)
    // Perhaps animal should inherit from Food?
    if (m_script->operation("eat", op, res) != 0) {
        return;
    }

    Set s;
    Anonymous self_ent;
    self_ent->setId(getId());
    self_ent->setAttr("status", -1);
    s->setTo(getId());
    s->setArgs1(self_ent);

    const std::string & to = op->getFrom();

    Nourish n;
    Anonymous nour_ent;
    nour_ent->setId(to);
    nour_ent->setAttr("mass", m_mass);
    n->setTo(to);
    n->setArgs1(nour_ent);

    res.push_back(s);
    res.push_back(n);
}

void Character::NourishOperation(const Operation & op, OpVector & res)
{
    if (op->getArgs().empty()) {
        error(op, "Nourish has no argument", res, getId());
        return;
    }
    const Root & arg = op->getArgs().front();
    Element mass_attr;
    if (arg->copyAttr("mass", mass_attr) != 0 || !mass_attr.isNum()) {
        return;
    }
    m_food = m_food + mass_attr.asNum();

    Anonymous food_ent;
    food_ent->setId(getId());
    food_ent->setAttr("food", m_food);
    Element alcohol_attr;
    if (arg->copyAttr("alcohol", alcohol_attr) == 0 && alcohol_attr.isNum()) {
        m_drunkness += alcohol_attr.asNum() / m_mass;
        food_ent->setAttr("drunkness", m_drunkness);
    }

    Set s;
    s->setArgs1(food_ent);

    Sight si;
    si->setTo(getId());
    si->setArgs1(s);
    res.push_back(si);
}

void Character::UseOperation(const Operation & op, OpVector & res)
{
}

void Character::WieldOperation(const Operation & op, OpVector & res)
{
    if (op->getArgs().empty()) {
        // Wield nothing
        if (m_rightHandWieldConnection.connected()) {
            m_rightHandWieldConnection.disconnect();
        }

        Set set;
        set->setTo(getId());
        Anonymous set_arg;
        set_arg->setId(getId());
        set_arg->setAttr("right_hand_wield", "");
        set->setArgs1(set_arg);
        res.push_back(set);

        return;
    }
    const Root & arg = op->getArgs().front();
    if (!arg->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
        error(op, "Wield arg has no ID", res, getId());
        return;
    }
    const std::string & id = arg->getId();
    Entity * item = m_world->getEntity(id);
    if (item == 0) {
        error(op, "Wield arg does not exist", res, getId());
        return;
    }

    EntitySet::const_iterator K = m_contains.find(item);
    if (K == m_contains.end()) {
        error(op, "Wield arg is not in inventory", res, getId());
        return;
    }

    if (m_rightHandWieldConnection.connected()) {
        m_rightHandWieldConnection.disconnect();
    }

    Set set;
    set->setTo(getId());
    Anonymous set_arg;
    set_arg->setId(getId());
    set_arg->setAttr("right_hand_wield", item->getId());
    set->setArgs1(set_arg);
    res.push_back(set);

    m_rightHandWieldConnection = item->containered.connect(sigc::mem_fun(this, &Character::wieldDropped));

    // m_rightHandWield = item->getId();
    debug(std::cout << "Wielding " << item->getId() << std::endl << std::flush;);
}

void Character::AttackOperation(const Operation & op, OpVector & res)
{
    const std::string & from = op->getFrom();
    if (from == getId()) {
        return;
    }

    Entity * attack_ent = m_world->getEntity(op->getFrom());
    if (attack_ent == 0) {
        log(ERROR, "AttackOperation: Attack op from non-existant ID");
        return;
    }

    Character * attacker = dynamic_cast<Character *>(attack_ent);

    if (attacker == 0) {
        log(ERROR, "AttackOperation: Attack op from non-character entity");
        return;
    }

    if (attacker->m_task != 0) {
        log(ERROR, String::compose("AttackOperation: Attack op aborted because attacker %1(%2) busy", attacker->getId(), attacker->getType()).c_str());
        return;
    }

    if (m_task != 0) {
        log(ERROR, String::compose("AttackOperation: Attack op aborted because defender %1(%2) busy", getId(), getType()).c_str());
        return;
    }

#if 0
    Combat * combat = new Combat(*attacker, *this);

    m_task = combat;
    combat->incRef();

    attacker->m_task = combat;
    combat->incRef();

    m_task->initTask(op, res);

    if (combat->obsolete()) {
        std::cout << "Attack aborted because one of more character is exhausted" << std::endl << std::flush;

        assert(m_task == 0);
        assert(attacker->m_task != 0);

        attacker->m_task = 0;
        combat->decRef();

        return;
    }
#else
    Task * combat = m_world->newTask("combat", *this);

    if (combat != 0) {
        setTask(combat);
        m_task->initTask(op, res);
        if (m_task->obsolete()) {
            clearTask();
        }
    } else {
        log(ERROR, "Character::AttackOperation: Unable to create combat task");
    }
#endif
}

void Character::ChopOperation(const Operation & op, OpVector & res)
{
    Set s;
    Anonymous sarg;
    sarg->setId(getId());
    sarg->setAttr("status", getStatus() - 0.2);
    s->setArgs1(sarg);
    s->setTo(getId());
    res.push_back(s);
}

void Character::mindLoginOperation(const Operation & op, OpVector & res)
{
}

void Character::mindLogoutOperation(const Operation & op, OpVector & res)
{
}

void Character::mindActionOperation(const Operation & op, OpVector & res)
{
    // FIXME Put this in, and make sure it doesn't happen again.
    log(WARNING, "Explicit Action operation from client");
    
    const std::vector<Root> & args = op->getArgs();
    if (args.empty()) {
        log(ERROR, "mindActionOperation: action op has no argument");
        return;
    }
    const Root & arg = args.front();
    Element action;
    if (arg->copyAttr("action", action) == -1) {
        log(ERROR, "mindActionOperation: action op arg has no action");
        return;
    } else {
        std::cout << "Action: " << action.asString() << std::endl << std::flush;
    }

    op->setTo(getId());
    res.push_back(op);
}

void Character::mindAddOperation(const Operation & op, OpVector & res)
{
}

void Character::mindAttackOperation(const Operation & op, OpVector & res)
{
    const std::vector<Root> & args = op->getArgs();
    if (args.empty()) {
        log(ERROR, "mindAttackOperation: attack op has no argument");
        return;
    }
    const Root & arg = args.front();
    if (!arg->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
        log(ERROR, "mindAttackOperation: attack op arg has no ID");
        return;
    }
    const std::string & id = arg->getId();

    // FIXME Check current TASK?
    // If already in combat, probably this should not work.
    // If doing something else, it gets aborted.
    // endCurrentTask();

    op->setTo(id);
    res.push_back(op);
}

void Character::mindSetupOperation(const Operation & op, OpVector & res)
{
    op->setTo(getId());
    op->setAttr("sub_to", "mind");
    res.push_back(op);
}

void Character::mindUseOperation(const Operation & op, OpVector & res)
{
    debug(std::cout << "Got Use op from mind" << std::endl << std::flush;);

    const std::string & toolId = m_rightHandWield;

    if (toolId.empty()) {
        error(op, "Character::mindUseOp No tool wielded.", res, getId());
        return;
    }
    // FIXME Get a tool id from the op attributes?

    Entity * tool = m_world->getEntity(toolId);
    if (tool == 0) {
        error(op, "Character::mindUseOp Tool does not exist.", res, getId());
        return;
    }

    Element toolOpAttr;
    std::set<std::string> toolOps;
    std::string op_type;

    // Determine the operations this tool supports
    if (!tool->get("operations", toolOpAttr)) {
        log(NOTICE, "Character::mindUseOp Attempt to use something not a tool");
        return;
    }

    if (!toolOpAttr.isList()) {
        log(ERROR, "Character::mindUseOp Tool has non list operations list");
        return;
    }
    const ListType & toolOpList = toolOpAttr.asList();
    if (toolOpList.empty()) {
        log(ERROR, "Character::mindUseOp Tool operation list is empty");
        return;
    }
    ListType::const_iterator J = toolOpList.begin();
    ListType::const_iterator Jend = toolOpList.end();
    assert(J != Jend);
    if (!(*J).isString()) {
        log(ERROR, "Character::mindUseOp Tool operation list is malformed");
        return;
    }
    op_type = (*J).String();
    debug(std::cout << "default tool op is " << op_type << std::endl
                                                        << std::flush;);
    for (; J != Jend; ++J) {
        if (!(*J).isString()) {
            log(ERROR, "Character::mindUseOp Tool has non string in operations list");
        } else {
            toolOps.insert((*J).String());
        }
    }


    RootEntity entity_arg(0);

    assert(!entity_arg.isValid());

    // Look at Use args. If arg is an entity, this is the target.
    // If arg is an operation, this is the operation to be used, and the
    // sub op arg may be an entity specifying target. If op to be used is
    // specified, this is checked against the ops permitted by this tool.
    const std::vector<Root> & args = op->getArgs();
    if (!args.empty()) {
        const Root & arg = args.front();
        const std::string & argtype = arg->getObjtype();
        if (argtype == "op") {
            if (!arg->hasAttrFlag(Atlas::Objects::PARENTS_FLAG) ||
                (arg->getParents().empty())) {
                error(op, "Use arg op has malformed parents", res, getId());
                return;
            }
            op_type = arg->getParents().front();
            debug(std::cout << "Got op type " << op_type << " from arg"
                            << std::endl << std::flush;);
            if (toolOps.find(op_type) == toolOps.end()) {
                error(op, "Use op is not permitted by tool", res, getId());
                return;
            }
            // Check against valid ops
            Operation arg_op = smart_dynamic_cast<Operation>(arg);
            if (!arg_op.isValid()) {
                error(op, "Use op arg is a malformed op", res, getId());
                return;
            }

            const std::vector<Root> & arg_op_args = arg_op->getArgs();
            if (!arg_op_args.empty()) {
                entity_arg = smart_dynamic_cast<RootEntity>(arg_op_args.front());
                if (!entity_arg.isValid()) {
                    error(op, "Use op target is malformed", res, getId());
                    return;
                }
            }
        } else if (argtype == "obj") {
            entity_arg = smart_dynamic_cast<RootEntity>(arg);
            if (!entity_arg.isValid()) {
                error(op, "Use target is malformed", res, getId());
                return;
            }
        } else {
            error(op, "Use arg has unknown objtype", res, getId());
            return;
        }
    }

    Anonymous target;
    if (!entity_arg.isValid()) {
        error(op, "Character::mindUseOperation No target specified", res, getId());
        return;
    }

    if (!entity_arg->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
        error(op, "Character::mindUseOperation Target entity has no ID", res, getId());
        return;
    }
    target->setId(entity_arg->getId());

    if (entity_arg->hasAttrFlag(Atlas::Objects::Entity::POS_FLAG)) {
        target->setPos(entity_arg->getPos());
    }

    if (op_type.empty()) {
        error(op, "Character::mindUseOperation Unable to determine op type for tool", res, getId());
        return;
    }

    debug(std::cout << "Using tool " << tool->getType() << " on "
                    << target->getId()
                    << " with " << op_type << " action."
                    << std::endl << std::flush;);

    Root obj = Atlas::Objects::Factories::instance()->createObject(op_type);
    if (!obj.isValid()) {
        log(ERROR, String::compose("Character::mindUseOperation Unknown op type %1 requested by %2 tool", op_type, tool->getType()).c_str());
        return;
    }
    Operation rop = smart_dynamic_cast<Operation>(obj);
    if (!rop.isValid()) {
        log(ERROR, String::compose("Character::mindUseOperation Op type %1 requested by %2 tool, but it is not an operation type", op_type, tool->getType()).c_str());
        // FIXME Think hard about how this error is reported. Would the error
        // make it back to the client if we made an error response?
        return;
    } else if (!target->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
        debug(std::cout << "No target" << std::endl << std::flush;);
    } else {
        rop->setArgs1(target);
    }

    rop->setTo(toolId);

    Task * task = m_world->activateTask(tool->getType(), op_type, *this);
    if (task != NULL) {
        setTask(task);
        m_task->initTask(rop, res);
        if (m_task->obsolete()) {
            clearTask();
        }
        return;
    }

    res.push_back(rop);

    Sight sight;
    sight->setArgs1(rop);
    res.push_back(sight);
}

void Character::mindUpdateOperation(const Operation & op, OpVector & res)
{
}

void Character::mindWieldOperation(const Operation & op, OpVector & res)
{
    debug(std::cout << "Got Wield op from mind" << std::endl << std::flush;);
    op->setTo(getId());
    res.push_back(op);
}

void Character::mindTickOperation(const Operation & op, OpVector & res)
{
    op->setTo(getId());
    op->setAttr("sub_to", "mind");
    res.push_back(op);
}

void Character::mindMoveOperation(const Operation & op, OpVector & res)
{
    debug( std::cout << "Character::mind_move_op" << std::endl << std::flush;);
    const std::vector<Root> & args = op->getArgs();
    if (args.empty()) {
        log(ERROR, "mindMoveOperation: move op has no argument");
        return;
    }
    const RootEntity arg = smart_dynamic_cast<RootEntity>(args.front());
    if (!arg.isValid()) {
        log(ERROR, "mindMoveOperation: Arg is not an entity");
        return;
    }
    if (!arg->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
        log(ERROR, "mindMoveOperation: Arg has no ID");
        return;
    }
    if (getStamina() <= 0.f) {
        // Character is immobilised.
        return;
    }
    const std::string & other_id = arg->getId();
    // FIXME We are looking up the object, but the vast majority of the
    // time we are moving ourselves. Bypass this lookup if possible.
    if (other_id != getId()) {
        debug( std::cout << "Moving something else. " << other_id << std::endl << std::flush;);
        Entity * other = m_world->getEntity(other_id);
        if (other == 0) {
            Unseen u;

            Anonymous unseen_arg;
            unseen_arg->setId(other_id);
            u->setArgs1(unseen_arg);

            u->setTo(getId());
            res.push_back(u);
            return;
        }
        if ((other->getMass() < 0) ||
            (other->getMass() > m_statistics.get("strength"))) {
            debug( std::cout << "We can't move this. Just too heavy" << std::endl << std::flush;);
            return;
        }
        op->setTo(other_id);
        res.push_back(op);
        return;
    }
    std::string new_loc;
    if (arg->hasAttrFlag(Atlas::Objects::Entity::LOC_FLAG)) {
        new_loc = arg->getLoc();
    } else {
        debug( std::cout << "Parent not set" << std::endl << std::flush;);
    }
    Point3D new_pos;
    Vector3D new_velocity;
    Quaternion new_orientation;
    try {
        if (arg->hasAttrFlag(Atlas::Objects::Entity::POS_FLAG)) {
            fromStdVector(new_pos, arg->getPos());
            debug( std::cout << "pos set to " << new_pos << std::endl << std::flush;);
        }

        if (arg->hasAttrFlag(Atlas::Objects::Entity::VELOCITY_FLAG)) {
            fromStdVector(new_velocity, arg->getVelocity());
            debug( std::cout << "vel set to " << new_velocity
                             << std::endl << std::flush;);
        }

        Element orientation_attr;
        if (arg->copyAttr("orientation", orientation_attr) == 0) {
            new_orientation.fromAtlas(orientation_attr);
            debug( std::cout << "ori set to " << new_orientation << std::endl << std::flush;);
            if (!new_orientation.isValid()) {
                log(ERROR, "Invalid orientation from client. Ignoring");
            }
        }
    }
    catch (Atlas::Message::WrongTypeException&) {
        log(ERROR, "EXCEPTION: mindMoveOperation: Malformed move operation");
        return;
    }
    catch (...) {
        log(ERROR, "EXCEPTION: mindMoveOperation: Unknown exception thrown");
        return;
    }

    if (new_pos.isValid()) {
        new_pos +=
            (Vector3D(((double)rand())/RAND_MAX, ((double)rand())/RAND_MAX, 0)
                                * (m_drunkness * 10));
    }
    debug( std::cout << ":" << new_loc << ":" << m_location.m_loc->getId()
                     << ":" << std::endl << std::flush;);
    if (!new_loc.empty() && (new_loc != m_location.m_loc->getId())) {
        debug(std::cout << "Changing loc" << std::endl << std::flush;);
        Entity * target_loc = m_world->getEntity(new_loc);
        if (target_loc == 0) {
            Unseen u;

            Anonymous unseen_arg;
            unseen_arg->setId(new_loc);
            u->setArgs1(unseen_arg);

            u->setTo(getId());
            res.push_back(u);
            return;
        }

        if (new_pos.isValid()) {
            Location target(target_loc, new_pos);
            Vector3D distance = distanceTo(m_location, target);
            assert(distance.isValid());
            // Convert target into our current frame of reference.
            new_pos = m_location.pos() + distance;
        } else {
            log(WARNING, "mindMoveOperation: Argument changes LOC, but no POS specified. Not sure this makes any sense");
        }
    }
    // Movement within current loc. Work out the speed and stuff and
    // use movement object to track movement.

    Location ret_location;
    int ret = m_movement.getUpdatedLocation(ret_location);
    if (ret) {
        ret_location = m_location;
    }

    // FIXME THis here?
    m_movement.reset();

    Vector3D direction;
    if (new_pos.isValid()) {
        direction = new_pos - ret_location.pos();
    } else if (new_velocity.isValid()) {
        direction = new_velocity;
    }
    if (direction.isValid() && (direction.mag() > 0)) {
        direction.normalize();
        debug( std::cout << "Direction: " << direction << std::endl
                         << std::flush;);
        if (!new_orientation.isValid()) {
            // This is a character walking, so it should stap upright
            Vector3D uprightDirection = direction;
            uprightDirection[cZ] = 0;
            if (uprightDirection.mag() > 0) {
                uprightDirection.normalize();
                new_orientation = quaternionFromTo(Vector3D(1,0,0),
                                                   uprightDirection);
                debug( std::cout << "Orientation: " << new_orientation
                                 << std::endl << std::flush;);
            }
        }
    }

    double vel_mag;
    if (new_velocity.isValid()) {
        vel_mag = std::min(new_velocity.mag(), consts::base_velocity);
    } else {
        vel_mag = consts::base_velocity;
    }

    // Need to add the arguments to this op before we return it
    // direction is already a unit vector
    debug( if (new_pos.isValid()) { std::cout<<"\tUsing target"
                                           << std::endl
                                           << std::flush; } );
    if (new_pos.isValid()) {
        m_movement.setTarget(new_pos);
        debug(std::cout << "Target" << new_pos
                        << std::endl << std::flush;);
    }
    if (direction.isValid()) {
        ret_location.m_velocity = direction;
        ret_location.m_velocity *= vel_mag;
        debug(std::cout << "Velocity" << ret_location.velocity()
                        << std::endl << std::flush;);
    }
    ret_location.m_orientation = new_orientation;
    debug(std::cout << "Orientation" << ret_location.orientation()
                    << std::endl << std::flush;);

    Operation moveOp = m_movement.generateMove(ret_location);
    assert(moveOp.isValid());
    res.push_back(moveOp);

    if (ret_location.velocity().isValid() &&
        ret_location.velocity() != Vector3D(0,0,0)) {

        Tick tickOp;
        Anonymous tick_arg;
        tick_arg->setAttr("serialno", m_movement.serialno());
        tick_arg->setName("move");
        tickOp->setArgs1(tick_arg);
        tickOp->setTo(getId());
        tickOp->setFutureSeconds(m_movement.getTickAddition(ret_location.pos(),
                                                     ret_location.velocity()));

        res.push_back(tickOp);
    }

}

void Character::mindSetOperation(const Operation & op, OpVector & res)
{
    const std::vector<Root> & args = op->getArgs();
    if (args.empty()) {
        log(ERROR, "mindSetOperation: set op has no argument");
        return;
    }
    const Root & arg = args.front();
    if (arg->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
        op->setTo(arg->getId());
    } else {
        op->setTo(getId());
    }
    res.push_back(op);
}

void Character::mindSightOperation(const Operation & op, OpVector & res)
{
}

void Character::mindSoundOperation(const Operation & op, OpVector & res)
{
}

void Character::mindChopOperation(const Operation & op, OpVector & res)
{
}

void Character::mindCombineOperation(const Operation & op, OpVector & res)
{
}

void Character::mindCreateOperation(const Operation & op, OpVector & res)
{
    op->setTo(getId());
    res.push_back(op);
}

void Character::mindDeleteOperation(const Operation & op, OpVector & res)
{
    op->setTo(getId());
    res.push_back(op);
}

void Character::mindDivideOperation(const Operation & op, OpVector & res)
{
}

void Character::mindBurnOperation(const Operation & op, OpVector & res)
{
}

void Character::mindGetOperation(const Operation & op, OpVector & res)
{
}

void Character::mindImaginaryOperation(const Operation & op, OpVector & res)
{
    op->setTo(getId());
    res.push_back(op);
}

void Character::mindInfoOperation(const Operation & op, OpVector & res)
{
}

void Character::mindNourishOperation(const Operation & op, OpVector & res)
{
}

void Character::mindTalkOperation(const Operation & op, OpVector & res)
{
    debug( std::cout << "Character::mindTalkOperation"
                     << std::endl << std::flush;);
    op->setTo(getId());
    res.push_back(op);
}

void Character::mindLookOperation(const Operation & op, OpVector & res)
{
    debug(std::cout << "Got look up from mind from [" << op->getFrom()
               << "] to [" << op->getTo() << "]" << std::endl << std::flush;);
    m_perceptive = true;
    const std::vector<Root> & args = op->getArgs();
    if (args.empty()) {
        op->setTo(m_world->m_gameWorld.getId());
    } else {
        const Root & arg = args.front();
        if (arg->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
            op->setTo(arg->getId());
        } else {
            op->setTo(getId());
        }
    }
    debug( std::cout <<"  now to ["<<op->getTo()<<"]"<<std::endl<<std::flush;);
    res.push_back(op);
}

void Character::mindCutOperation(const Operation & op, OpVector & res)
{
    log(WARNING, "mindCutOperation: Unexpected Cut op from mind");
    op->setTo(getId());
    res.push_back(op);
}

void Character::mindEatOperation(const Operation & op, OpVector & res)
{
    // FIXME Need to get what food to eat from the arg, and sort out goals
    // so they don't set TO
    const std::vector<Root> & args = op->getArgs();
    if (args.empty()) {
        log(ERROR, "mindEatOperation: Op has no ARGS");
        return;
    }
    const Root & arg = args.front();
    if (!arg->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
        log(ERROR, "mindEatOperation: Arg has no ID");
        return;
    }
    op->setTo(arg->getId());
    res.push_back(op);
}

void Character::mindTouchOperation(const Operation & op, OpVector & res)
{
    // Work out what is being touched.
    const std::vector<Root> & args = op->getArgs();
    if (args.empty()) {
        log(ERROR, "mindTouchOperation: Op has no ARGS");
        return;
    }
    const Root & arg = args.front();
    if (!arg->hasAttrFlag(Atlas::Objects::ID_FLAG)) {
        log(ERROR, "mindTouchOperation: Op has no ARGS");
        return;
    }
    // Pass the modified touch operation on to target.
    op->setTo(arg->getId());
    res.push_back(op);
    // Send sight of touch
    Sight s;
    s->setArgs1(op);
    res.push_back(s);
}

void Character::mindAppearanceOperation(const Operation & op, OpVector & res)
{
}

void Character::mindDisappearanceOperation(const Operation & op, OpVector & res)
{
}


void Character::mindErrorOperation(const Operation & op, OpVector & res)
{
}

void Character::mindOtherOperation(const Operation & op, OpVector & res)
{
    op->setTo(getId());
    res.push_back(op);
}

bool Character::w2mActionOperation(const Operation & op)
{
    return false;
}

bool Character::w2mAttackOperation(const Operation & op)
{
    return false;
}

bool Character::w2mLoginOperation(const Operation & op)
{
    return false;
}

bool Character::w2mLogoutOperation(const Operation & op)
{
    return false;
}

bool Character::w2mChopOperation(const Operation & op)
{
    return false;
}

bool Character::w2mCreateOperation(const Operation & op)
{
    return false;
}

bool Character::w2mCutOperation(const Operation & op)
{
    return false;
}

bool Character::w2mDeleteOperation(const Operation & op)
{
    return false;
}

bool Character::w2mEatOperation(const Operation & op)
{
    return false;
}

bool Character::w2mBurnOperation(const Operation & op)
{
    return false;
}

bool Character::w2mMoveOperation(const Operation & op)
{
    return false;
}

bool Character::w2mSetOperation(const Operation & op)
{
    return false;
}

bool Character::w2mLookOperation(const Operation & op)
{
    return false;
}

bool Character::w2mDivideOperation(const Operation & op)
{
    return false;
}

bool Character::w2mCombineOperation(const Operation & op)
{
    return false;
}

bool Character::w2mGetOperation(const Operation & op)
{
    return false;
}

bool Character::w2mImaginaryOperation(const Operation & op)
{
    return false;
}

bool Character::w2mInfoOperation(const Operation & op)
{
    return false;
}

bool Character::w2mTalkOperation(const Operation & op)
{
    return false;
}

bool Character::w2mNourishOperation(const Operation & op)
{
    return false;
}

bool Character::w2mAppearanceOperation(const Operation & op)
{
    if (m_drunkness > 1.0) {
        return false;
    }
    return true;
}

bool Character::w2mDisappearanceOperation(const Operation & op)
{
    if (m_drunkness > 1.0) {
        return false;
    }
    return true;
}

bool Character::w2mErrorOperation(const Operation & op)
{
    return true;
}

bool Character::w2mOtherOperation(const Operation & op)
{
    return true;
}

bool Character::w2mSetupOperation(const Operation & op)
{
    if (op->hasAttr("sub_to")) {
        return true;
    }
    return false;
}

bool Character::w2mUseOperation(const Operation & op)
{
    return false;
}

bool Character::w2mWieldOperation(const Operation & op)
{
    return false;
}

bool Character::w2mTickOperation(const Operation & op)
{
    if (op->hasAttr("sub_to")) {
        return true;
    }
    return false;
}

bool Character::w2mSightOperation(const Operation & op)
{
    if (m_drunkness > 1.0) {
        return false;
    }
    return true;
}

bool Character::w2mSoundOperation(const Operation & op)
{
    if (m_drunkness > 1.0) {
        return false;
    }
    return true;
}

bool Character::w2mTouchOperation(const Operation & op)
{
    if (m_drunkness > 1.0) {
        return false;
    }
    return true;
}

void Character::sendMind(const Operation & op, OpVector & res)
{
    debug( std::cout << "Character::sendMind(" << op->getParents().front() << ")" << std::endl << std::flush;);

    if (0 != m_externalMind) {
        if (0 != m_mind) {
            OpVector mindRes;
            m_mind->operation(op, mindRes);
            // Discard all the local results
        }
        debug(std::cout << "Sending to external mind" << std::endl
                         << std::flush;);
        m_externalMind->operation(op, res);
    } else {
        debug(std::cout << "Using ops from local mind"
                        << std::endl << std::flush;);
        if (0 != m_mind) {
            m_mind->operation(op, res);
        }
    }
}

/// \brief Filter operations from the mind destined for the body.
///
/// Operations from the character's mind which is either an NPC mind,
/// or a remote client are passed in here for pre-processing and filtering
/// before they are valid to be processed as internal ops. The operation
/// may be modified and re-used so operations passed to this function have
/// their ownership passed in, and caller should not modify the operation,
/// make assumptions that it has not been modified after calling mind2body.
/// @param op The operation to be processed.
/// @param res The result of the operation is returned here.
void Character::mind2body(const Operation & op, OpVector & res)
{
    debug( std::cout << "Character::mind2body(" << std::endl << std::flush;);

    if (m_drunkness > 1.0) {
        return;
    }
    if (op->hasAttrFlag(Atlas::Objects::Operation::TO_FLAG)) {
        log(ERROR, String::compose("Operation \"%1\" from mind with TO set", op->getParents().front()).c_str());
    }
    if (op->hasAttrFlag(Atlas::Objects::Operation::FUTURE_SECONDS_FLAG) &&
        op->getClassNo() != OP_TICK) {
        log(ERROR, String::compose("Operation \"%1\" from mind with FUTURE_SECONDS set", op->getParents().front()).c_str());
    }
    OpNo otype = opEnumerate(op);
    OP_SWITCH(op, otype, res, mind)
}

bool Character::world2mind(const Operation & op)
{
    debug( std::cout << "Character::world2mind(" << op->getParents().front() << ")" << std::endl << std::flush;);
    OpNo otype = opEnumerate(op);
    POLL_OP_SWITCH(op, otype, w2m)
    return false;
}

void Character::operation(const Operation & op, OpVector & res)
{
    debug( std::cout << "Character::operation(" << op->getParents().front() << ")" << std::endl << std::flush;);
    callOperation(op, res);
    // set refno on result?
    if (!m_isAlive) {
        return;
    }
    if (world2mind(op)) {
        debug( std::cout << "Character::operation(" << op->getParents().front() << ") passed to mind" << std::endl << std::flush;);
        OpVector mres;
        sendMind(op, mres);
        OpVector::const_iterator Iend = mres.end();
        for (OpVector::const_iterator I = mres.begin(); I != Iend; ++I) {
            //Operation * mr2 = mind2_res.front();
            // Need to be very careful about what this actually does
            (*I)->setSerialno(newSerialNo());
            externalOperation(*I);
        }
    }
}

void Character::externalOperation(const Operation & op)
{
    debug( std::cout << "Character::externalOperation(" << op->getParents().front() << ")" << std::endl << std::flush;);
    OpVector mres;
    mind2body(op, mres);
    
    // We require that the first op is the direct consequence of the minds
    // op, so it gets the same serialno
    // FIXME in Atlas-C++ 0.6 we can do this by relying on being able
    // to query if an object has a certain attribute. A copied op will have
    // it, a new op won't.
    OpVector::const_iterator Ibegin = mres.begin();
    OpVector::const_iterator Iend = mres.end();
    for (OpVector::const_iterator I = Ibegin; I != Iend; ++I) {
        if (I == Ibegin) {
            (*I)->setSerialno(op->getSerialno());
        } else {
            (*I)->setSerialno(newSerialNo());
        }
        sendWorld(*I);
        // Don't delete br as it has gone into World's queue
        // World will deal with it.
    }
}

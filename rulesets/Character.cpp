// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "Character.h"

#include "Pedestrian.h"
#include "Task.h"
#include "MindFactory.h"
#include "BaseMind.h"
#include "Script.h"
#include "World.h"

#include "common/op_switch.h"
#include "common/const.h"
#include "common/debug.h"
#include "common/globals.h"
#include "common/log.h"
#include "common/inheritance.h"
#include "common/Property.h"
#include "common/serialno.h"

#include "common/Tick.h"
#include "common/Nourish.h"
#include "common/Unseen.h"

#include <wfmath/atlasconv.h>

#include <Atlas/Objects/Operation/Action.h>
#include <Atlas/Objects/Operation/Sound.h>
#include <Atlas/Objects/Operation/Set.h>
#include <Atlas/Objects/Operation/Look.h>
#include <Atlas/Objects/Operation/Sight.h>
#include <Atlas/Objects/Operation/Move.h>
#include <Atlas/Objects/Operation/Appearance.h>

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Message::ListType;
using Atlas::Objects::Operation::Set;
using Atlas::Objects::Operation::Sight;
using Atlas::Objects::Operation::Sound;
using Atlas::Objects::Operation::Tick;
using Atlas::Objects::Operation::Look;
using Atlas::Objects::Operation::Move;
using Atlas::Objects::Operation::Action;
using Atlas::Objects::Operation::Unseen;
using Atlas::Objects::Operation::Nourish;
using Atlas::Objects::Operation::Appearance;

static const bool debug_flag = false;

const double Character::energyConsumption = 0.001;
const double Character::foodConsumption = 0.1;
const double Character::weightConsumption = 1.0;
const double Character::energyGain = 0.5;
const double Character::energyLoss = 0.1;
const double Character::weightGain = 0.5;


void Character::metabolise(OpVector & res, double ammount)
{
    // Currently handles energy
    // We should probably call this whenever the entity performs a movement.
    MapType ent;
    ent["id"] = getId();
    if ((m_status > (1.5 + energyLoss)) && (m_mass < m_maxMass)) {
        m_status = m_status - energyLoss;
        ent["mass"] = m_mass + weightGain;
    }
    double energyUsed = energyConsumption * ammount;
    if ((m_status <= energyUsed) && (m_mass > weightConsumption)) {
        ent["status"] = m_status - energyUsed + energyGain;
        ent["mass"] = m_mass - weightConsumption;
    } else {
        ent["status"] = m_status - energyUsed;
    }
    if (m_drunkness > 0) {
        ent["drunkness"] = m_drunkness - 0.1;
    }

    Set * s = new Set();
    s->setTo(getId());
    s->setArgs(ListType(1,ent));

    res.push_back(s);
}

Character::Character(const std::string & id) : Character_parent(id),
                                            m_movement(*new Pedestrian(*this)),
                                            m_task(0), m_isAlive(true),
                                            m_drunkness(0.0), m_sex("female"),
                                            m_food(0), m_maxMass(100),
                                            m_mind(NULL), m_externalMind(NULL)
{
    m_mass = 60;
    m_location.m_bBox = BBox(WFMath::Point<3>(-0.25, -0.25, 0),
                             WFMath::Point<3>(0.25, 0.25, 2));

    subscribe("imaginary", OP_IMAGINARY);
    subscribe("tick", OP_TICK);
    subscribe("talk", OP_TALK);
    subscribe("eat", OP_EAT);
    subscribe("nourish", OP_NOURISH);
    subscribe("wield", OP_WIELD);

    // subscribe to ops from the mind
    mindSubscribe("action", OP_ACTION);
    mindSubscribe("setup", OP_SETUP);
    mindSubscribe("tick", OP_TICK);
    mindSubscribe("move", OP_MOVE);
    mindSubscribe("set", OP_SET);
    mindSubscribe("create", OP_CREATE);
    mindSubscribe("delete", OP_DELETE);
    mindSubscribe("imaginary", OP_IMAGINARY);
    mindSubscribe("talk", OP_TALK);
    mindSubscribe("look", OP_LOOK);
    mindSubscribe("cut", OP_CUT);
    mindSubscribe("eat", OP_EAT);
    mindSubscribe("touch", OP_TOUCH);
    mindSubscribe("use", OP_USE);
    mindSubscribe("wield", OP_WIELD);
    mindSubscribe("shoot", OP_OTHER);

    // subscribe to ops for the mind
    w2mSubscribe("appearance", OP_APPEARANCE);
    w2mSubscribe("disappearance", OP_DISAPPEARANCE);
    w2mSubscribe("error", OP_ERROR);
    w2mSubscribe("setup", OP_SETUP);
    w2mSubscribe("tick", OP_TICK);
    w2mSubscribe("sight", OP_SIGHT);
    w2mSubscribe("sound", OP_SOUND);
    w2mSubscribe("touch", OP_TOUCH);

    m_properties["drunkness"] = new Property<double>(m_drunkness, a_drunk);
    m_properties["sex"] = new Property<std::string>(m_sex, a_sex);
    m_properties["right_hand_wield"] = new Property<std::string>(m_rightHandWield, a_rwield);
}

Character::~Character()
{
    delete &m_movement;
    if (m_mind != NULL) {
        delete m_mind;
    }
    if (m_externalMind != NULL) {
        delete m_externalMind;
    }
}

void Character::ImaginaryOperation(const Operation & op, OpVector & res)
{
    Sight * s = new Sight();
    s->setArgs(ListType(1,op.asObject()));
    res.push_back(s);
}

void Character::SetupOperation(const Operation & op, OpVector & res)
{
    debug( std::cout << "CHaracter::Operation(setup)" << std::endl
                     << std::flush;);

    if (op.hasAttr("sub_to")) {
        debug( std::cout << "Has sub_to" << std::endl << std::flush;);
        return;
    }

    Appearance * app = new Appearance();
    ListType & args = app->getArgs();
    args.push_back(MapType());
    MapType & arg = args.back().asMap();
    arg["id"] = getId();

    res.push_back(app);

    if (m_script->Operation("setup", op, res) != 0) {
        return;
    }

    if (0 == m_externalMind) {
        // This ensures that newly created player characters don't get
        // bogged down with an NPC mind. In the short term this
        // takes away PC programmability.
        // FIXME Characters restored from the database will still get
        // AI minds, so  we need to handle them somehow differently.
        // Perhaps the Restore op (different from Setup op) is needed?

        m_mind = MindFactory::instance()->newMind(getId(), m_name, m_type);

        Operation * s = new Operation(op);
        // THis is so not the right thing to do
        s->setAttr("sub_to", "mind");
        res.push_back(s);

        Look * l = new Look();
        l->setTo(getId());
        res.push_back(l);
    }

    Tick * tick = new Tick();
    tick->setTo(getId());
    res.push_back(tick);
}

void Character::TickOperation(const Operation & op, OpVector & res)
{
    if (op.hasAttr("sub_to")) {
        debug( std::cout << "Has sub_to" << std::endl << std::flush;);
        Element sub_to = op.getAttr("sub_to");
        if (!sub_to.isString()) {
            error(op, "Tick op sub_to is not string", res, getId());
            return;
        }
        if (sub_to.asString() == "task") {
            std::cout << "Task tick" << std::endl << std::flush;
            if (m_task == 0) {
                log(ERROR, "Got Tick op for task, but task is null");
                return;
            }
            m_task->TickOperation(op, res);
        }
        return;
    }
    debug(std::cout << "================================" << std::endl
                    << std::flush;);
    const ListType & args = op.getArgs();
    if (!args.empty() && args.front().isMap()) {
        // Deal with movement.
        const MapType & arg1 = args.front().asMap();
        MapType::const_iterator I = arg1.find("serialno");
        if ((I != arg1.end()) && (I->second.isInt())) {
            if (I->second.asInt() < m_movement.m_serialno) {
                debug(std::cout << "Old tick" << std::endl << std::flush;);
                return;
            }
        }
        Location ret_loc;
        Move * moveOp = m_movement.genMoveUpdate(&ret_loc);
        if (moveOp) {
            res.push_back(moveOp);
            if (!m_movement.moving()) {
                return;
            }
            MapType entmap;
            entmap["name"] = "move";
            entmap["serialno"] = m_movement.m_serialno;
            Tick * tickOp = new Tick();
            tickOp->setTo(getId());
            tickOp->setFutureSeconds(m_movement.getTickAddition(ret_loc.m_pos));
            tickOp->setArgs(ListType(1,entmap));
            res.push_back(tickOp);
            return;
        }
    } else {
        m_script->Operation("tick", op, res);

        // DIGEST
        if ((m_food >= foodConsumption) && (m_status < 2)) {
            // It is important that the metabolise bit is done next, as this
            // handles the status change
            m_status = m_status + foodConsumption;
            m_food = m_food - foodConsumption;

            MapType food_ent;
            food_ent["id"] = getId();
            food_ent["food"] = m_food;
            Set s;
            s.setTo(getId());
            s.setArgs(ListType(1,food_ent));

            Sight * si = new Sight();
            si->setTo(getId());
            si->setArgs(ListType(1,s.asObject()));
            res.push_back(si);
        }

        // METABOLISE
        metabolise(res);
        
        // TICK
        Tick * tickOp = new Tick();
        tickOp->setTo(getId());
        tickOp->setFutureSeconds(consts::basic_tick * 30);
        res.push_back(tickOp);
    }
}

void Character::TalkOperation(const Operation & op, OpVector & res)
{
    debug( std::cout << "Character::OPeration(Talk)" << std::endl<<std::flush;);
    Sound * s = new Sound();
    s->setArgs(ListType(1,op.asObject()));
    res.push_back(s);
}

void Character::EatOperation(const Operation & op, OpVector & res)
{
    // This is identical to Food::Operation(Eat &)
    // Perhaps animal should inherit from Food?
    if (m_script->Operation("eat", op, res) != 0) {
        return;
    }

    MapType self_ent;
    self_ent["id"] = getId();
    self_ent["status"] = -1;

    Set * s = new Set();
    s->setTo(getId());
    s->setArgs(ListType(1,self_ent));

    const std::string & to = op.getFrom();
    MapType nour_ent;
    nour_ent["id"] = to;
    nour_ent["mass"] = m_mass;
    Nourish * n = new Nourish();
    n->setTo(to);
    n->setArgs(ListType(1,nour_ent));

    res.push_back(s);
    res.push_back(n);
}

void Character::NourishOperation(const Operation & op, OpVector & res)
{
    if (op.getArgs().empty()) {
        error(op, "Nourish has no argument", res, getId());
        return;
    }
    if (!op.getArgs().front().isMap()) {
        error(op, "Nourish arg is malformed", res, getId());
        return;
    }
    const MapType & nent = op.getArgs().front().asMap();
    MapType::const_iterator I = nent.find("mass");
    if ((I == nent.end()) || !I->second.isNum()) {
        return;
    }
    m_food = m_food + I->second.asNum();

    MapType food_ent;
    food_ent["id"] = getId();
    food_ent["food"] = m_food;
    if (((I = nent.find("alcohol")) != nent.end()) && I->second.isNum()) {
        m_drunkness += I->second.asNum() / m_mass;
        food_ent["drunkness"] = m_drunkness;
    }
    Set s;
    s.setArgs(ListType(1,food_ent));

    Sight * si = new Sight();
    si->setTo(getId());
    si->setArgs(ListType(1,s.asObject()));
    res.push_back(si);
}

void Character::WieldOperation(const Operation & op, OpVector & res)
{
    if (op.getArgs().empty()) {
        // FIXME Wield nothing perhaps?
        error(op, "Wield has no argument", res, getId());
        return;
    }
    if (!op.getArgs().front().isMap()) {
        error(op, "Wield arg is malformed", res, getId());
        return;
    }
    const MapType & went = op.getArgs().front().asMap();
    MapType::const_iterator I = went.find("id");
    if ((I == went.end()) || !I->second.isString()) {
        error(op, "Wield arg has no ID", res, getId());
        return;
    }
    const std::string & id = I->second.asString();
    EntityDict::const_iterator J = m_world->getEntities().find(id);
    if (J == m_world->getEntities().end()) {
        error(op, "Wield arg does not exist", res, getId());
        return;
    }
    Entity * item = J->second;
    assert(item != 0);
    EntitySet::const_iterator K = m_contains.find(item);
    if (K == m_contains.end()) {
        error(op, "Wield arg is not in inventory", res, getId());
        return;
    }

    Operation * set = new Set;
    set->setTo(getId());
    ListType & set_args = set->getArgs();
    MapType set_arg;
    set_arg["id"] = getId();
    set_arg["right_hand_wield"] = item->getId();
    set_args.push_back(set_arg);
    res.push_back(set);

    // m_rightHandWield = item->getId();
    debug(std::cout << "Wielding " << item->getId() << std::endl << std::flush;);
}

void Character::mindLoginOperation(const Operation & op, OpVector & res)
{
}

void Character::mindLogoutOperation(const Operation & op, OpVector & res)
{
}

void Character::mindActionOperation(const Operation & op, OpVector & res)
{
    Operation *a = new Operation(op);
    a->setTo(getId());
    res.push_back(a);
}

void Character::mindAddOperation(const Operation & op, OpVector & res)
{
}

void Character::mindSetupOperation(const Operation & op, OpVector & res)
{
    Operation *s = new Operation(op);
    s->setTo(getId());
    s->setAttr("sub_to", "mind");
    res.push_back(s);
}

void Character::mindUseOperation(const Operation & op, OpVector & res)
{
    debug(std::cout << "Got Use op from mind" << std::endl << std::flush;);

    std::string toolId = m_rightHandWield;

    // FIXME Get a tool id from the op attributes?

    EntityDict::const_iterator I = m_world->getEntities().find(toolId);
    if (I == m_world->getEntities().end()) {
        error(op, "Use tool does not exist.", res, getId());
        return;
    }

    Entity * tool = I->second;
    assert(tool != 0);

    Element toolOpAttr;
    std::set<std::string> toolOps;
    std::string op_type;

    // Determine the operations this tool supports
    if (tool->get("operations", toolOpAttr)) {
        if (!toolOpAttr.isList()) {
            log(ERROR, "Use tool has non list operations list");
            return;
        }
        const ListType & toolOpList = toolOpAttr.asList();
        ListType::const_iterator J = toolOpList.begin();
        ListType::const_iterator Jend = toolOpList.end();
        if ((J != Jend) && ((*J).isString())) {
            op_type = (*J).asString();
            debug(std::cout << "default tool op is " << op_type << std::endl
                                                                << std::flush;);
        }
        for (; J != Jend; ++J) {
            if (!(*J).isString()) {
                log(ERROR, "Use tool has non string in operations list");
            }
            toolOps.insert((*J).asString());
        }
    } // FIXME else return? Not much can happen if toolOps is empty

    // Look at Use args. If arg is an entity, this is the target.
    // If arg is an operation, this is the operation to be used, and the
    // sub op arg may be an entity specifying target. If op to be used is
    // specified, this is checked against the ops permitted by this tool.
    MapType target;
    const ListType & args = op.getArgs();
    if (!args.empty()) {
        const Element & arg = args.front();
        if (!arg.isMap()) {
            error(op, "Use arg is not a map", res, getId());
            return;
        }
        const MapType & amap = arg.asMap();
        MapType::const_iterator K = amap.find("objtype");
        if (K == amap.end() || !K->second.isString()) {
            error(op, "Use arg has no objtype", res, getId());
            return;
        }
        const std::string & argtype = K->second.asString();
        if (argtype == "op") {
            K = amap.find("parents");
            if (K == amap.end() || (!K->second.isList()) ||
                (K->second.asList().empty())) {
                error(op, "Use arg op has malformed parents", res, getId());
                return;
            }
            const Element & arg_op_parent = K->second.asList().front();
            if (!arg_op_parent.isString()) {
                error(op, "Use arg op has non string parent", res, getId());
                return;
            }
            op_type = arg_op_parent.asString();
            debug(std::cout << "Got op type " << op_type << " from arg"
                            << std::endl << std::flush;);
            if (toolOps.find(op_type) == toolOps.end()) {
                error(op, "Use op is not permitted by tool", res, getId());
                return;
            }
            // Check against valid ops

            K = amap.find("args");
            if (K == amap.end() || (!K->second.isList())) {
                error(op, "Use arg op has malformed args", res, getId());
                return;
            }
            if (!K->second.asList().empty()) {
                const Element & arg_op_arg = K->second.asList().front();
                if (!arg_op_arg.isMap()) {
                    error(op, "Use arg op has map arg", res, getId());
                    return;
                }
                const MapType & arg_op_amap = arg_op_arg.asMap();
                
                K = arg_op_amap.find("id");
                if (K == arg_op_amap.end() || (!K->second.isString())) {
                    error(op, "Use arg entity has no ID", res, getId());
                    return;
                }
                target["id"] = K->second.asString();
                debug(std::cout << "Got target " << target["id"].asString()
                                << " from op arg"
                                << std::endl << std::flush;);
                // FIXME Duplicated code beloabove (see FIXME)
                K = amap.find("pos");
                if (K != amap.end()) {
                    debug(std::cout << "Got a use op with POS"
                                    << std::endl << std::flush;);
                    if (!K->second.isList()) {
                        error(op, "Use arg entity has non-list POS", res, getId());
                        return;
                    }
                    target["pos"] = K->second.asList();
                }
            }
        } else if (argtype == "obj") {
            K = amap.find("id");
            if (K == amap.end() || (!K->second.isString())) {
                error(op, "Use arg entity has no ID", res, getId());
                return;
            }
            target["id"] = K->second.asString();
            debug(std::cout << "Got target " << target["id"].asString()
                            << " from arg"
                            << std::endl << std::flush;);
            // FIXME Duplicated code above (see FIXME)
            K = amap.find("pos");
            if (K != amap.end()) {
                debug(std::cout << "Got a use op with POS"
                                << std::endl << std::flush;);
                if (!K->second.isList()) {
                    error(op, "Use arg entity has non-list POS", res, getId());
                    return;
                }
                target["pos"] = K->second.asList();
            }
        } else {
            error(op, "Use arg has unknown objtype", res, getId());
            return;
        }
    }

    if (op_type.empty()) {
        error(op, "Use unable to determine op type to for tool", res, getId());
        return;
    }

    debug(std::cout << "Using tool " << tool->getType() << " on "
                    << target["id"].asString()
                    << " with " << op_type << " action."
                    << std::endl << std::flush;);

    Operation * rop = Inheritance::instance().newOperation(op_type);
    if (rop == 0) {
        std::string err("Character::mindUseMethod: Unknown op type \"");
        err += op_type;
        err += "\" requested by \"";
        err += tool->getType();
        err += "\" tool";
        log(ERROR, err.c_str());
        // FIXME Thing hard about how this error is reported. Would the error
        // make it back to the client if we made an error response?
        return;
    } else if (target.empty()) {
        debug(std::cout << "No target" << std::endl << std::flush;);
    } else {
        ListType & rop_args = rop->getArgs();
        rop_args.push_back(target);
    }

    rop->setTo(toolId);
    res.push_back(rop);

    Operation * sight = new Sight;
    ListType & sight_args = sight->getArgs();
    sight_args.push_back(rop->asMap());
    res.push_back(sight);
}

void Character::mindUpdateOperation(const Operation & op, OpVector & res)
{
}

void Character::mindWieldOperation(const Operation & op, OpVector & res)
{
    debug(std::cout << "Got Wield op from mind" << std::endl << std::flush;);
    Operation *w = new Operation(op);
    w->setTo(getId());
    res.push_back(w);
}

void Character::mindTickOperation(const Operation & op, OpVector & res)
{
    Operation *t = new Operation(op);
    t->setTo(getId());
    t->setAttr("sub_to", "mind");
    res.push_back(t);
}

void Character::mindMoveOperation(const Operation & op, OpVector & res)
{
    debug( std::cout << "Character::mind_move_op" << std::endl << std::flush;);
    const ListType & args = op.getArgs();
    if (args.empty() || !args.front().isMap()) {
        log(ERROR, "mindMoveOperation: move op has no argument");
        return;
    }
    const MapType & arg1 = args.front().asMap();
    MapType::const_iterator I = arg1.find("id");
    if ((I == arg1.end()) || !I->second.isString()) {
        log(ERROR, "mindMoveOperation: Args has got no ID");
    }
    const std::string & other_id = I->second.asString();
    EntityDict::const_iterator J = m_world->getEntities().find(other_id);
    if (J == m_world->getEntities().end()) {
        log(ERROR, "mindMoveOperation: This move op is for a phoney id");
        log(NOTICE, "Sending Unseen op back to mind. We should not see this again.");
        Operation * u = new Unseen;

        ListType & unseen_args = u->getArgs();
        MapType unseen_arg;
        unseen_arg["id"] = other_id;
        unseen_args.push_back(unseen_arg);

        u->setTo(getId());
        res.push_back(u);
        return;
    }
    Entity * obj = J->second;
    if (obj != this) {
        debug( std::cout << "Moving something else. " << other_id << std::endl << std::flush;);
        if ((obj->getMass() < 0) || (obj->getMass() > m_mass)) {
            debug( std::cout << "We can't move this. Just too heavy" << std::endl << std::flush;);
            return;
        }
        Operation * newop = new Operation(op);
        newop->setTo(other_id);
        res.push_back(newop);
        return;
    }
    std::string new_loc;
    I = arg1.find("loc");
    if ((I != arg1.end()) && (I->second.isString())) {
        new_loc = I->second.asString();
    } else {
        debug( std::cout << "Parent not set" << std::endl << std::flush;);
    }
    Point3D new_pos;
    Vector3D new_velocity;
    Quaternion new_orientation;
    try {
        I = arg1.find("pos");
        if (I != arg1.end()) {
            new_pos.fromAtlas(I->second.asList());
            debug( std::cout << "pos set to " << new_pos << std::endl << std::flush;);
        }

        I = arg1.find("velocity");
        if (I != arg1.end()) {
            new_velocity.fromAtlas(I->second.asList());
            debug( std::cout << "vel set to " << new_velocity
                             << std::endl << std::flush;);
        }

        I = arg1.find("orientation");
        if (I != arg1.end()) {
            new_orientation.fromAtlas(I->second.asList());
            debug( std::cout << "ori set to " << new_orientation << std::endl << std::flush;);
        }
    }
    catch (Atlas::Message::WrongTypeException&) {
        log(ERROR, "EXCEPTION: mindMoveOperation: Malformed move operation");
    }

    double futureSeconds = op.getFutureSeconds();
    if (!new_pos.isValid()) {
        if (futureSeconds < 0.) {
            futureSeconds = 0.;
        }
    } else {
        new_pos +=
            (Vector3D(((double)rand())/RAND_MAX, ((double)rand())/RAND_MAX, 0)
                                * (m_drunkness * 10));
    }
    debug( std::cout << ":" << new_loc << ":" << m_location.m_loc->getId()
                     << ":" << std::endl << std::flush;);
    // At the moment we can't deal if the movement changes ref, or occurs
    // in the past, so we just let it by unchanged.
    if ((futureSeconds < 0.) ||
        ((!new_loc.empty()) && (new_loc != m_location.m_loc->getId()))) {
        Operation * newop = new Operation(op);
        newop->setTo(getId());
        newop->setFutureSeconds(futureSeconds);
        res.push_back(newop);
        return;
    }
    // Movement within current ref. Work out the speed and stuff and
    // use movement object to track movement.
    //
    double vel_mag;
    if (!new_velocity.isValid()) {
        debug( std::cout << "\tVelocity default" << std::endl<<std::flush;);
        vel_mag = consts::base_velocity;
    } else {
        debug( std::cout << "\tVelocity: " << new_velocity
                         << std::endl << std::flush;);
        vel_mag = new_velocity.mag();
        if (vel_mag > consts::base_velocity) {
            vel_mag = consts::base_velocity;
        }
    }

    // If the position is given, and it is about right, don't bother to 
    // use it. FIXME This breaks the idea that if position is given
    // it is destination. Removed for now, but may be of future interest
    // if (new_pos.isValid() &&
        // (squareDistance(new_pos, m_location.m_pos) < 0.01)) {
        // new_pos = Vector3D();
    // }

    Location ret_location;
    Move * moveOp = m_movement.genMoveUpdate(&ret_location);
    const Location & current_location = (NULL != moveOp) ? ret_location
                                                         : m_location;
    m_movement.reset();

    Vector3D direction;
    if (!new_pos.isValid()) {
        if (!new_velocity.isValid() || isZero(new_velocity)) {
            debug( std::cout << "\tUsing orientation for direction"
                             << std::endl << std::flush;);
            // If velocity is not given, and target is not given,
            // then we are not moving at all, so direction must
            // remain invalid.
        } else {
            debug( std::cout << "\tUsing velocity for direction"
                             << std::endl << std::flush;);
            direction = new_velocity;
        }
    } else {
        debug( std::cout << "\tUsing destination for direction"
                         << std::endl << std::flush;);
        direction = new_pos - current_location.m_pos;
    }
    if (direction.isValid() && !(direction.mag() > 0)) {
        direction.setValid(false);
    }
    if (direction.isValid()) {
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

    if ((vel_mag == 0) || !direction.isValid()) {
        debug( std::cout << "\tMovement stopped" << std::endl
                         << std::flush;);
        if (NULL != moveOp) {
            debug( std::cout << "Stop!" << std::endl << std::flush;);
            ListType & args = moveOp->getArgs();
            MapType & ent = args.front().asMap();
            ent["velocity"] = Vector3D(0,0,0).toAtlas();
            ent["mode"] = "standing";
            if (new_orientation.isValid()) {
                ent["orientation"] = new_orientation.toAtlas();
            }
        } else {
            debug( std::cout << "Turn!" << std::endl << std::flush;);
            m_movement.m_orient = new_orientation;
            moveOp = m_movement.genFaceOperation();
        }
        if (NULL != moveOp) {
            res.push_back(moveOp);
        }
        return;
    }
    if (NULL != moveOp) {
        delete moveOp;
    }

    Tick * tickOp = new Tick();
    MapType ent;
    ent["serialno"] = m_movement.m_serialno;
    ent["name"] = "move";
    ListType tick_args(1,ent);
    tickOp->setArgs(tick_args);
    tickOp->setTo(getId());
    // Need to add the arguments to this op before we return it
    // direction is already a unit vector
    debug( if (new_pos.isValid()) { std::cout<<"\tUsing target"
                                           << std::endl
                                           << std::flush; } );
    m_movement.m_targetPos = new_pos;
    m_movement.m_velocity = direction;
    m_movement.m_velocity *= vel_mag;
    m_movement.m_orient = new_orientation;
    debug( std::cout << "Velocity " << vel_mag << std::endl << std::flush;);
    moveOp = m_movement.genMoveOperation(NULL, current_location);
    tickOp->setFutureSeconds(m_movement.getTickAddition(m_location.m_pos));
    debug( std::cout << "Next tick " << tickOp->getFutureSeconds()
                     << std::endl << std::flush;);
    assert(moveOp != NULL);

    // return moveOp and tickOp;
    res.push_back(moveOp);
    res.push_back(tickOp);
}

void Character::mindSetOperation(const Operation & op, OpVector & res)
{
    const ListType & args = op.getArgs();
    if (args.empty() || !args.front().isMap()) {
        return;
    }
    Operation * s = new Operation(op);
    const MapType & amap = args.front().asMap();
    MapType::const_iterator I = amap.find("id");
    if (I != amap.end() && I->second.isString()) {
        const std::string & opid = I->second.asString();
        s->setTo(opid);
    } else {
        if (op.getTo().empty()) {
            s->setTo(getId());
        }
    }
    res.push_back(s);
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
    Operation * c = new Operation(op);
    c->setTo(getId());
    res.push_back(c);
}

void Character::mindDeleteOperation(const Operation & op, OpVector & res)
{
    Operation * d = new Operation(op);
    d->setTo(getId());
    res.push_back(d);
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
    Operation * i = new Operation(op);
    i->setTo(getId());
    res.push_back(i);
}

void Character::mindInfoOperation(const Operation & op, OpVector & res)
{
}

void Character::mindNourishOperation(const Operation & op, OpVector & res)
{
}

void Character::mindTalkOperation(const Operation & op, OpVector & res)
{
    debug( std::cout << "Character::mindOPeration(Talk)"
                     << std::endl << std::flush;);
    Operation * t = new Operation(op);
    t->setTo(getId());
    res.push_back(t);
}

void Character::mindLookOperation(const Operation & op, OpVector & res)
{
    debug(std::cout << "Got look up from mind from [" << op.getFrom()
               << "] to [" << op.getTo() << "]" << std::endl << std::flush;);
    m_perceptive = true;
    Operation * l = new Operation(op);
    if (op.getTo().empty()) {
        const ListType & args = op.getArgs();
        if (args.empty() || !args.front().isMap()) {
            l->setTo(m_world->m_gameWorld.getId());
        } else {
            const MapType & amap = args.front().asMap();
            MapType::const_iterator I = amap.find("id");
            if (I != amap.end() && I->second.isString()) {
                l->setTo(I->second.asString());
            }
        }
    }
    debug( std::cout <<"    now to ["<<l->getTo()<<"]"<<std::endl<<std::flush;);
    res.push_back(l);
}

void Character::mindCutOperation(const Operation & op, OpVector & res)
{
    Operation * c = new Operation(op);
    if (op.getTo().empty()) {
        c->setTo(getId());
    }
    res.push_back(c);
}

void Character::mindEatOperation(const Operation & op, OpVector & res)
{
    Operation * e = new Operation(op);
    // FIXME Need to get what food to eat from the arg, and sort out goals
    // so they don't set TO
    if (op.getTo().empty()) {
        e->setTo(getId());
    }
    res.push_back(e);
}

void Character::mindTouchOperation(const Operation & op, OpVector & res)
{
    Operation * t = new Operation(op);
    // Work out what is being touched.
    const ListType & args = op.getArgs();
    if (op.getTo().empty()) {
        if (args.empty() || !args.front().isMap()) {
            t->setTo(m_world->m_gameWorld.getId());
        } else {
            const MapType & amap = args.front().asMap();
            MapType::const_iterator I = amap.find("id");
            if (I != amap.end() && I->second.isString()) {
                t->setTo(I->second.asString());
            }
        }
    }
    // Pass the modified touch operation on to target.
    res.push_back(t);
    // Send action "touch"
    Action * a = new Action();
    a->setTo(getId());
    MapType amap;
    amap["id"] = getId();
    amap["action"] = "touch";
    ListType setArgs(1,amap);
    a->setArgs(setArgs);
    res.push_back(a);
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
    Operation * e = new Operation(op);
    if (op.getTo().empty()) {
        e->setTo(getId());
    }
    res.push_back(e);
}

bool Character::w2mActionOperation(const Operation & op)
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
    if (op.hasAttr("sub_to")) {
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
    if (op.hasAttr("sub_to")) {
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
    debug( std::cout << "Character::sendMind" << std::endl << std::flush;);

    if (0 != m_externalMind) {
        if (0 != m_mind) {
            OpVector mindRes;
            m_mind->operation(op, mindRes);
            // Discard all the local results
            OpVector::const_iterator Jend = mindRes.end(); 
            for (OpVector::const_iterator J = mindRes.begin(); J != Jend; ++J) {
                delete *J;
            }
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

void Character::mind2body(const Operation & op, OpVector & res)
{
    debug( std::cout << "Character::mind2body" << std::endl << std::flush;);

    if (m_drunkness > 1.0) {
        return;
    }
    if (!op.getTo().empty()) {
        std::cerr << "Operation \"" << op.getParents().front().asString() << "\"from mind with TO set" << std::endl << std::flush;
        log(WARNING, "Operation from mind with TO set");
    }
    OpNo otype = opEnumerate(op, opMindLookup);
    OP_SWITCH(op, otype, res, mind)
}

bool Character::world2mind(const Operation & op)
{
    debug( std::cout << "Character::world2mind" << std::endl << std::flush;);
    OpNo otype = opEnumerate(op, opW2mLookup);
    POLL_OP_SWITCH(op, otype, w2m)
    return false;
}

void Character::operation(const Operation & op, OpVector & res)
{
    debug( std::cout << "Character::operation" << std::endl << std::flush;);
    callOperation(op, res);
    // set refno on result?
    if (!m_isAlive) {
        return;
    }
    if (world2mind(op)) {
        OpVector mres;
        sendMind(op, mres);
        OpVector::const_iterator Iend = mres.end();
        for (OpVector::const_iterator I = mres.begin(); I != Iend; ++I) {
            //Operation * mr2 = mind2_res.front();
            // Need to be very careful about what this actually does
            (*I)->setSerialno(newSerialNo());
            externalOperation(**I);
            delete *I;
        }
    }
}

void Character::externalOperation(const Operation & op)
{
    debug( std::cout << "Character::externalOperation" << std::endl << std::flush;);
    OpVector mres;
    mind2body(op, mres);
    
    // We require that the first op is the direct consequence of the minds
    // op, so it gets the same serialno
    // FIXME in Atlas-C++ 0.6 we can do this by relying on being able
    // to query if an object has a certain attribute. A copied op will have
    // it, a new op won't.
    OpVector::const_iterator Iend = mres.end();
    for (OpVector::const_iterator I = mres.begin(); I != Iend; ++I) {
        if (I == mres.begin()) {
            (*I)->setSerialno(op.getSerialno());
        } else {
            (*I)->setSerialno(newSerialNo());
        }
        sendWorld(*I);
        // Don't delete br as it has gone into World's queue
        // World will deal with it.
    }
}

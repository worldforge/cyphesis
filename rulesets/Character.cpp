// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "Character.h"

#include "Pedestrian.h"
#include "MindFactory.h"
#include "BaseMind.h"
#include "Script.h"
#include "World.h"

#include "common/BaseWorld.h"
#include "common/op_switch.h"
#include "common/const.h"
#include "common/debug.h"
#include "common/globals.h"
#include "common/log.h"

#include "common/Setup.h"
#include "common/Tick.h"
#include "common/Cut.h"
#include "common/Eat.h"
#include "common/Nourish.h"
#include "common/Use.h"
#include "common/Wield.h"

#include <wfmath/atlasconv.h>

#include <Atlas/Objects/Operation/Action.h>
#include <Atlas/Objects/Operation/Sound.h>
#include <Atlas/Objects/Operation/Set.h>
#include <Atlas/Objects/Operation/Delete.h>
#include <Atlas/Objects/Operation/Imaginary.h>
#include <Atlas/Objects/Operation/Look.h>
#include <Atlas/Objects/Operation/Talk.h>
#include <Atlas/Objects/Operation/Touch.h>
#include <Atlas/Objects/Operation/Sight.h>
#include <Atlas/Objects/Operation/Move.h>
#include <Atlas/Objects/Operation/Error.h>
#include <Atlas/Objects/Operation/Appearance.h>
#include <Atlas/Objects/Operation/Disappearance.h>

static const bool debug_flag = false;

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
                                            m_isAlive(true),
                                            m_drunkness(0.0), m_sex("female"),
                                            m_food(0), m_maxMass(100),
                                            m_mind(NULL), m_externalMind(NULL)
{
    m_mass = 60;
    m_location.m_bBox = BBox(WFMath::Point<3>(-0.25, -0.25, 0),
                             WFMath::Point<3>(0.25, 0.25, 2));
    m_attributes["mode"] = "birth";

    subscribe("imaginary", OP_IMAGINARY);
    subscribe("tick", OP_TICK);
    subscribe("talk", OP_TALK);
    subscribe("eat", OP_EAT);
    subscribe("nourish", OP_NOURISH);

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

bool Character::get(const std::string & aname, Element & attr) const
{
    if (aname == "drunkness") {
        attr = m_drunkness;
        return true;
    } else if (aname == "sex") {
        attr = m_sex;
        return true;
    }
    return Character_parent::get(aname, attr);
}

void Character::set(const std::string & aname, const Element & attr)
{
    if ((aname == "drunkness") && attr.isFloat()) {
        m_drunkness = attr.asFloat();
        m_update_flags |= a_drunk;
    } else if ((aname == "sex") && attr.isString()) {
        m_sex = attr.asString();
        m_update_flags |= a_sex;
    } else {
        Character_parent::set(aname, attr);
    }
}

void Character::addToMessage(MapType & omap) const
{
    omap["sex"] = m_sex;
    Character_parent::addToMessage(omap);
}

void Character::ImaginaryOperation(const Imaginary & op, OpVector & res)
{
    Sight * s = new Sight();
    s->setArgs(ListType(1,op.asObject()));
    res.push_back(s);
}

void Character::SetupOperation(const Setup & op, OpVector & res)
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

        Setup * s = new Setup(op);
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

void Character::TickOperation(const Tick & op, OpVector & res)
{
    if (op.hasAttr("sub_to")) {
        debug( std::cout << "Has sub_to" << std::endl << std::flush;);
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

void Character::TalkOperation(const Talk & op, OpVector & res)
{
    debug( std::cout << "Character::OPeration(Talk)" << std::endl<<std::flush;);
    Sound * s = new Sound();
    s->setArgs(ListType(1,op.asObject()));
    res.push_back(s);
}

void Character::EatOperation(const Eat & op, OpVector & res)
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

void Character::NourishOperation(const Nourish & op, OpVector & res)
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

void Character::mindLoginOperation(const Login & op, OpVector & res)
{
}

void Character::mindLogoutOperation(const Logout & op, OpVector & res)
{
}

void Character::mindActionOperation(const Action & op, OpVector & res)
{
    Action *a = new Action(op);
    a->setTo(getId());
    res.push_back(a);
}

void Character::mindSetupOperation(const Setup & op, OpVector & res)
{
    Setup *s = new Setup(op);
    s->setTo(getId());
    s->setAttr("sub_to", "mind");
    res.push_back(s);
}

void Character::mindUseOperation(const Use & op, OpVector & res)
{
    std::cout << "Got Use op from mind" << std::endl << std::flush;
    // Use *s = new Use(op);
    // s->setTo(getId());
    // res.push_back(s);
    
}

void Character::mindWieldOperation(const Wield & op, OpVector & res)
{
    std::cout << "Got Wield op from mind" << std::endl << std::flush;
    Wield *s = new Wield(op);
    s->setTo(getId());
    res.push_back(s);
}

void Character::mindTickOperation(const Tick & op, OpVector & res)
{
    Tick *t = new Tick(op);
    t->setTo(getId());
    t->setAttr("sub_to", "mind");
    res.push_back(t);
}

void Character::mindMoveOperation(const Move & op, OpVector & res)
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
        log(ERROR, "mindMoveOperation: Args has got no id");
    }
    const std::string & oname = I->second.asString();
    EntityDict::const_iterator J = m_world->getObjects().find(oname);
    if (J == m_world->getObjects().end()) {
        log(ERROR, "mindMoveOperation: This move op is for a phoney id");
        return;
    }
    Entity * obj = J->second;
    if (obj != this) {
        debug( std::cout << "Moving something else. " << oname << std::endl << std::flush;);
        if ((obj->getMass() < 0) || (obj->getMass() > m_mass)) {
            debug( std::cout << "We can't move this. Just too heavy" << std::endl << std::flush;);
            return;
        }
        Move * newop = new Move(op);
        newop->setTo(oname);
        res.push_back(newop);
        return;
    }
    std::string new_ref;
    I = arg1.find("loc");
    if ((I != arg1.end()) && (I->second.isString())) {
        new_ref = I->second.asString();
    } else {
        debug( std::cout << "Parent not set" << std::endl << std::flush;);
    }
    Point3D new_coords;
    Vector3D new_vel;
    Quaternion new_orientation;
    try {
        I = arg1.find("pos");
        if (I != arg1.end()) {
            new_coords.fromAtlas(I->second.asList());
            debug( std::cout << "pos set to " << new_coords << std::endl << std::flush;);
        }

        I = arg1.find("velocity");
        if (I != arg1.end()) {
            new_vel.fromAtlas(I->second.asList());
            debug( std::cout << "vel set to " << new_vel << std::endl << std::flush;);
        }

        I = arg1.find("orientation");
        if (I != arg1.end()) {
            new_orientation.fromAtlas(I->second.asList());
            debug( std::cout << "ori set to " << new_orientation << std::endl << std::flush;);
        }
    }
    catch (Atlas::Message::WrongTypeException) {
        log(ERROR, "EXCEPTION: mindMoveOperation: Malformed move operation");
    }

    double futureSeconds = op.getFutureSeconds();
    if (!new_coords.isValid()) {
        if (futureSeconds < 0.) {
            futureSeconds = 0.;
        }
    } else {
        new_coords +=
            (Vector3D(((double)rand())/RAND_MAX, ((double)rand())/RAND_MAX, 0)
                                * (m_drunkness * 10));
    }
    debug( std::cout << ":" << new_ref << ":" << m_location.m_loc->getId()
                     << ":" << std::endl << std::flush;);
    // At the moment we can't deal if the movement changes ref, or occurs
    // in the past, so we just let it by unchanged.
    if ((futureSeconds < 0.) ||
        ((new_ref != m_location.m_loc->getId()) &&
         (!new_ref.empty())) ) {
        Move * newop = new Move(op);
        newop->setTo(getId());
        newop->setFutureSeconds(futureSeconds);
        res.push_back(newop);
        return;
    }
    // Movement within current ref. Work out the speed and stuff and
    // use movement object to track movement.
    //
    double vel_mag;
    if (!new_vel.isValid()) {
        debug( std::cout << "\tVelocity default" << std::endl<<std::flush;);
        vel_mag = consts::base_velocity;
    } else {
        debug( std::cout << "\tVelocity: " << new_vel
                         << std::endl << std::flush;);
        vel_mag = new_vel.mag();
        if (vel_mag > consts::base_velocity) {
            vel_mag = consts::base_velocity;
        }
    }

    // If the position is given, and it is about right, don't bother to 
    // use it. FIXME This breaks the idea that if position is given
    // it is destination. Removed for now, but may be of future interest
    // if (new_coords.isValid() &&
        // (squareDistance(new_coords, m_location.m_pos) < 0.01)) {
        // new_coords = Vector3D();
    // }

    Location ret_location;
    Move * moveOp = m_movement.genMoveUpdate(&ret_location);
    const Location & current_location = (NULL != moveOp) ? ret_location
                                                         : m_location;
    m_movement.reset();

    Vector3D direction;
    if (!new_coords.isValid()) {
        if (!new_vel.isValid() || isZero(new_vel)) {
            debug( std::cout << "\tUsing orientation for direction"
                             << std::endl << std::flush;);
            // If velocity is not given, and target is not given,
            // then we are not moving at all, so direction must
            // remain invalid.
        } else {
            debug( std::cout << "\tUsing velocity for direction"
                             << std::endl << std::flush;);
            direction = new_vel;
        }
    } else {
        debug( std::cout << "\tUsing destination for direction"
                         << std::endl << std::flush;);
        direction = new_coords - current_location.m_pos;
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
    debug( if (new_coords.isValid()) { std::cout<<"\tUsing target"
                                           << std::endl
                                           << std::flush; } );
    m_movement.m_targetPos = new_coords;
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

void Character::mindSetOperation(const Set & op, OpVector & res)
{
    const ListType & args = op.getArgs();
    if (args.empty() || !args.front().isMap()) {
        return;
    }
    Set * s = new Set(op);
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

void Character::mindSightOperation(const Sight & op, OpVector & res)
{
}

void Character::mindSoundOperation(const Sound & op, OpVector & res)
{
}

void Character::mindChopOperation(const Chop & op, OpVector & res)
{
}

void Character::mindCombineOperation(const Combine & op, OpVector & res)
{
}

void Character::mindCreateOperation(const Create & op, OpVector & res)
{
    Create * c = new Create(op);
    c->setTo(getId());
    res.push_back(c);
}

void Character::mindDeleteOperation(const Delete & op, OpVector & res)
{
    Delete * d = new Delete(op);
    d->setTo(getId());
    res.push_back(d);
}

void Character::mindDivideOperation(const Divide & op, OpVector & res)
{
}

void Character::mindBurnOperation(const Burn & op, OpVector & res)
{
}

void Character::mindGetOperation(const Get & op, OpVector & res)
{
}

void Character::mindImaginaryOperation(const Imaginary & op, OpVector & res)
{
    Imaginary * i = new Imaginary(op);
    i->setTo(getId());
    res.push_back(i);
}

void Character::mindInfoOperation(const Info & op, OpVector & res)
{
}

void Character::mindNourishOperation(const Nourish & op, OpVector & res)
{
}

void Character::mindTalkOperation(const Talk & op, OpVector & res)
{
    debug( std::cout << "Character::mindOPeration(Talk)"
                     << std::endl << std::flush;);
    Talk * t = new Talk(op);
    t->setTo(getId());
    res.push_back(t);
}

void Character::mindLookOperation(const Look & op, OpVector & res)
{
    debug(std::cout << "Got look up from mind from [" << op.getFrom()
               << "] to [" << op.getTo() << "]" << std::endl << std::flush;);
    m_perceptive = true;
    Look * l = new Look(op);
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

void Character::mindCutOperation(const Cut & op, OpVector & res)
{
    Cut * c = new Cut(op);
    if (op.getTo().empty()) {
        c->setTo(getId());
    }
    res.push_back(c);
}

void Character::mindEatOperation(const Eat & op, OpVector & res)
{
    Eat * e = new Eat(op);
    if (op.getTo().empty()) {
        e->setTo(getId());
    }
    res.push_back(e);
}

void Character::mindTouchOperation(const Touch & op, OpVector & res)
{
    Touch * t = new Touch(op);
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

void Character::mindAppearanceOperation(const Appearance & op, OpVector & res)
{
}

void Character::mindDisappearanceOperation(const Disappearance & op, OpVector & res)
{
}


void Character::mindErrorOperation(const Error & op, OpVector & res)
{
}

void Character::mindOtherOperation(const RootOperation & op, OpVector & res)
{
    RootOperation * e = new RootOperation(op);
    if (op.getTo().empty()) {
        e->setTo(getId());
    }
    res.push_back(e);
}

bool Character::w2mActionOperation(const Action & op)
{
    return false;
}

bool Character::w2mLoginOperation(const Login & op)
{
    return false;
}

bool Character::w2mLogoutOperation(const Logout & op)
{
    return false;
}

bool Character::w2mChopOperation(const Chop & op)
{
    return false;
}

bool Character::w2mCreateOperation(const Create & op)
{
    return false;
}

bool Character::w2mCutOperation(const Cut & op)
{
    return false;
}

bool Character::w2mDeleteOperation(const Delete & op)
{
    return false;
}

bool Character::w2mEatOperation(const Eat & op)
{
    return false;
}

bool Character::w2mBurnOperation(const Burn & op)
{
    return false;
}

bool Character::w2mMoveOperation(const Move & op)
{
    return false;
}

bool Character::w2mSetOperation(const Set & op)
{
    return false;
}

bool Character::w2mLookOperation(const Look & op)
{
    return false;
}

bool Character::w2mDivideOperation(const Divide & op)
{
    return false;
}

bool Character::w2mCombineOperation(const Combine & op)
{
    return false;
}

bool Character::w2mGetOperation(const Get & op)
{
    return false;
}

bool Character::w2mImaginaryOperation(const Imaginary & op)
{
    return false;
}

bool Character::w2mInfoOperation(const Info & op)
{
    return false;
}

bool Character::w2mTalkOperation(const Talk & op)
{
    return false;
}

bool Character::w2mNourishOperation(const Nourish & op)
{
    return false;
}

bool Character::w2mAppearanceOperation(const Appearance & op)
{
    if (m_drunkness > 1.0) {
        return false;
    }
    return true;
}

bool Character::w2mDisappearanceOperation(const Disappearance & op)
{
    if (m_drunkness > 1.0) {
        return false;
    }
    return true;
}

bool Character::w2mErrorOperation(const Error & op)
{
    return true;
}

bool Character::w2mOtherOperation(const RootOperation & op)
{
    return true;
}

bool Character::w2mSetupOperation(const Setup & op)
{
    if (op.hasAttr("sub_to")) {
        return true;
    }
    return false;
}

bool Character::w2mUseOperation(const Use & op)
{
    return false;
}

bool Character::w2mWieldOperation(const Wield & op)
{
    return false;
}

bool Character::w2mTickOperation(const Tick & op)
{
    if (op.hasAttr("sub_to")) {
        return true;
    }
    return false;
}

bool Character::w2mSightOperation(const Sight & op)
{
    if (m_drunkness > 1.0) {
        return false;
    }
    return true;
}

bool Character::w2mSoundOperation(const Sound & op)
{
    if (m_drunkness > 1.0) {
        return false;
    }
    return true;
}

bool Character::w2mTouchOperation(const Touch & op)
{
    if (m_drunkness > 1.0) {
        return false;
    }
    return true;
}

void Character::sendMind(const RootOperation & op, OpVector & res)
{
    debug( std::cout << "Character::sendMind" << std::endl << std::flush;);

    if (0 != m_externalMind) {
        if (0 != m_mind) {
            OpVector mindRes;
            m_mind->operation(op, mindRes);
            // Discard all the local results
            OpVector::const_iterator J = mindRes.begin(); 
            for(; J != mindRes.end(); J++) {
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

void Character::mind2body(const RootOperation & op, OpVector & res)
{
    debug( std::cout << "Character::mind2body" << std::endl << std::flush;);

    if (m_drunkness > 1.0) {
        return;
    }
    OpNo otype = opEnumerate(op, opMindLookup);
    OP_SWITCH(op, otype, res, mind)
}

bool Character::world2mind(const RootOperation & op)
{
    debug( std::cout << "Character::world2mind" << std::endl << std::flush;);
    OpNo otype = opEnumerate(op, opW2mLookup);
    POLL_OP_SWITCH(op, otype, w2m)
    return false;
}

void Character::operation(const RootOperation & op, OpVector & res)
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
        for(OpVector::const_iterator I = mres.begin(); I != mres.end(); I++) {
            //RootOperation * mr2 = mind2_res.front();
            // Need to be very careful about what this actually does
            m_world->setSerialnoOp(**I);
            externalOperation(**I);
            delete *I;
        }
    }
}

void Character::externalOperation(const RootOperation & op)
{
    debug( std::cout << "Character::externalOperation" << std::endl << std::flush;);
    OpVector mres;
    mind2body(op, mres);
    
    // We require that the first op is the direct consequence of the minds
    // op, so it gets the same serialno
    // FIXME in Atlas-C++ 0.6 we can do this by relying on being able
    // to query if an object has a certain attribute. A copied op will have
    // it, a new op won't.
    for (OpVector::const_iterator I = mres.begin(); I != mres.end(); ++I) {
        if (I == mres.begin()) {
            (*I)->setSerialno(op.getSerialno());
        } else {
            m_world->setSerialnoOp(**I);
        }
        sendWorld(*I);
        // Don't delete br as it has gone into World's queue
        // World will deal with it.
    }
}

// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#include "Character.h"

#include "Pedestrian.h"
#include "MindFactory.h"
#include "BaseMind.h"
#include "Script.h"
#include "World.h"

#include <common/BaseWorld.h>
#include <common/op_switch.h>
#include <common/const.h>
#include <common/debug.h>
#include <common/globals.h>
#include <common/log.h>

#include <common/Setup.h>
#include <common/Tick.h>
#include <common/Cut.h>
#include <common/Eat.h>
#include <common/Nourish.h>

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

OpVector Character::metabolise(double ammount)
{
    // Currently handles energy
    // We should probably call this whenever the entity performs a movement.
    Fragment::MapType ent;
    ent["id"] = getId();
    if ((status > (1.5 + energyLoss)) && (mass < maxMass)) {
        status = status - energyLoss;
        ent["mass"] = mass + weightGain;
    }
    double energyUsed = energyConsumption * ammount;
    if ((status <= energyUsed) && (mass > weightConsumption)) {
        ent["status"] = status - energyUsed + energyGain;
        ent["mass"] = mass - weightConsumption;
    } else {
        ent["status"] = status - energyUsed;
    }
    if (drunkness > 0) {
        ent["drunkness"] = drunkness - 0.1;
    }

    Set * s = new Set(Set::Instantiate());
    s->SetTo(getId());
    s->SetArgs(Fragment::ListType(1,ent));

    return OpVector(1,s);
}

Character::Character(const std::string & id) : Thing(id), movement(*new Pedestrian(*this)),
                                               drunkness(0.0), sex("female"), food(0),
                                               maxMass(100), isAlive(true),
                                               mind(NULL), externalMind(NULL)
{
    mass = 60;
    location.bBox = BBox(Vector3D(-0.25, -0.25, 0), Vector3D(0.25, 0.25, 2));
    attributes["mode"] = "birth";

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
    delete &movement;
    if (mind != NULL) {
        delete mind;
    }
    if (externalMind != NULL) {
        delete externalMind;
    }
}

const Fragment Character::get(const std::string & aname) const
{
    if (aname == "drunkness") {
        return drunkness;
    } else if (aname == "sex") {
        return sex;
    }
    return Thing::get(aname);
}

void Character::set(const std::string & aname, const Fragment & attr)
{
    if ((aname == "drunkness") && attr.IsFloat()) {
        drunkness = attr.AsFloat();
        update_flags |= a_drunk;
    } else if ((aname == "sex") && attr.IsString()) {
        sex = attr.AsString();
        update_flags |= a_sex;
    } else {
        Thing::set(aname, attr);
    }
}

void Character::addToObject(Fragment::MapType & omap) const
{
    omap["sex"] = sex;
    Entity::addToObject(omap);
}

OpVector Character::ImaginaryOperation(const Imaginary & op)
{
    Sight * s = new Sight(Sight::Instantiate());
    s->SetArgs(Fragment::ListType(1,op.AsObject()));
    return OpVector(1,s);
}

OpVector Character::SetupOperation(const Setup & op)
{
    debug( std::cout << "Character::tick" << std::endl << std::flush;);
    OpVector res;
    debug( std::cout << "CHaracter::Operation(setup)" << std::endl
                     << std::flush;);
    if (script->Operation("setup", op, res) != 0) {
        return res;
    }
    if (op.HasAttr("sub_to")) {
        debug( std::cout << "Has sub_to" << std::endl << std::flush;);
        return res;
    }

    //mind = new BaseMind(getId(), name);
    //std::string mind_class("NPCMind"), mind_package("mind.NPCMind");
    //if (global_conf->findItem("mind", type)) {
        //mind_package = global_conf->getItem("mind", type);
        //mind_class = type + "Mind";
    //}
    //Create_PyMind(mind, mind_package, mind_class);
    mind = MindFactory::instance()->newMind(getId(), name, type);

    OpVector res2(2);
    Setup * s = new Setup(op);
    // THis is so not the right thing to do
    s->SetAttr("sub_to", "mind");
    res2[0] = s;
    Look * l = new Look(Look::Instantiate());
    l->SetTo(world->getId());
    res2[1] = l;
    if (location.ref != &world->gameWorld) {
        l = new Look(Look::Instantiate());
        l->SetTo(location.ref->getId());
        res2.push_back(l);
    }
    l = new Look(Look::Instantiate());
    l->SetTo(getId());
    res2.push_back(l);
    Tick * tick = new Tick(Tick::Instantiate());
    tick->SetTo(getId());
    res2.push_back(tick);
    return res2;
}

OpVector Character::TickOperation(const Tick & op)
{
    if (op.HasAttr("sub_to")) {
        debug( std::cout << "Has sub_to" << std::endl << std::flush;);
        return OpVector();
    }
    debug(std::cout << "================================" << std::endl
                    << std::flush;);
    const Fragment::ListType & args = op.GetArgs();
    if ((!args.empty()) && (args.front().IsMap())) {
        // Deal with movement.
        const Fragment::MapType & arg1 = args.front().AsMap();
        Fragment::MapType::const_iterator I = arg1.find("serialno");
        if ((I != arg1.end()) && (I->second.IsInt())) {
            if (I->second.AsInt() < movement.m_serialno) {
                debug(std::cout << "Old tick" << std::endl << std::flush;);
                return OpVector();
            }
        }
        Location ret_loc;
        Move * moveOp = movement.genMoveOperation(&ret_loc);
        if (moveOp) {
            if (!movement.moving()) {
                return OpVector (1,moveOp);
            }
            OpVector res(2);
            Fragment::MapType entmap;
            entmap["name"] = "move";
            entmap["serialno"] = movement.m_serialno;
            Tick * tickOp = new Tick(Tick::Instantiate());
            tickOp->SetTo(getId());
            tickOp->SetFutureSeconds(movement.getTickAddition(ret_loc.coords));
            tickOp->SetArgs(Fragment::ListType(1,entmap));
            res[0] = tickOp;
            res[1] = moveOp;
            return res;
        }
    } else {
        OpVector res;
        script->Operation("tick", op, res);

        // DIGEST
        if ((food >= foodConsumption) && (status < 2)) {
            // It is important that the metabolise bit is done next, as this
            // handles the status change
            status = status + foodConsumption;
            food = food - foodConsumption;

            Fragment::MapType food_ent;
            food_ent["id"] = getId();
            food_ent["food"] = food;
            Set s = Set::Instantiate();
            s.SetTo(getId());
            s.SetArgs(Fragment::ListType(1,food_ent));

            Sight * si = new Sight(Sight::Instantiate());
            si->SetTo(getId());
            si->SetArgs(Fragment::ListType(1,s.AsObject()));
            res.push_back(si);
        }

        // METABOLISE
        OpVector mres = metabolise();
        for(OpVector::const_iterator I = mres.begin(); I != mres.end(); I++) {
            res.push_back(*I);
        }
        
        // TICK
        Tick * tickOp = new Tick(Tick::Instantiate());
        tickOp->SetTo(getId());
        tickOp->SetFutureSeconds(consts::basic_tick * 30);
        res.push_back(tickOp);
        return res;
    }
    return OpVector();
}

OpVector Character::TalkOperation(const Talk & op)
{
    debug( std::cout << "Character::OPeration(Talk)" << std::endl<<std::flush;);
    Sound * s = new Sound(Sound::Instantiate());
    s->SetArgs(Fragment::ListType(1,op.AsObject()));
    return OpVector(1,s);
}

OpVector Character::EatOperation(const Eat & op)
{
    // This is identical to Foof::Operation(Eat &)
    // Perhaps animal should inherit from Food?
    OpVector res;
    if (script->Operation("eat", op, res) != 0) {
        return res;
    }
    Fragment::MapType self_ent;
    self_ent["id"] = getId();
    self_ent["status"] = -1;

    Set * s = new Set(Set::Instantiate());
    s->SetTo(getId());
    s->SetArgs(Fragment::ListType(1,self_ent));

    const std::string & to = op.GetFrom();
    Fragment::MapType nour_ent;
    nour_ent["id"] = to;
    nour_ent["mass"] = mass;
    Nourish * n = new Nourish(Nourish::Instantiate());
    n->SetTo(to);
    n->SetArgs(Fragment::ListType(1,nour_ent));

    OpVector res2(2);
    res2[0] = s;
    res2[1] = n;
    return res2;
}

OpVector Character::NourishOperation(const Nourish & op)
{
    if (op.GetArgs().empty()) {
        return error(op, "Nourish has no argument", getId());
    }
    if (!op.GetArgs().front().IsMap()) {
        return error(op, "Nourish arg is malformed", getId());
    }
    const Fragment::MapType & nent = op.GetArgs().front().AsMap();
    Fragment::MapType::const_iterator I = nent.find("mass");
    if ((I == nent.end()) || !I->second.IsNum()) { return OpVector(); }
    food = food + I->second.AsNum();

    Fragment::MapType food_ent;
    food_ent["id"] = getId();
    food_ent["food"] = food;
    if (((I = nent.find("alcohol")) != nent.end()) && I->second.IsNum()) {
        drunkness += I->second.AsNum() / mass;
        food_ent["drunkness"] = drunkness;
    }
    Set s = Set::Instantiate();
    s.SetArgs(Fragment::ListType(1,food_ent));

    Sight * si = new Sight(Sight::Instantiate());
    si->SetTo(getId());
    si->SetArgs(Fragment::ListType(1,s.AsObject()));
    return OpVector(1,si);
}

OpVector Character::mindLoginOperation(const Login & op)
{
    return OpVector();
}

OpVector Character::mindLogoutOperation(const Logout & op)
{
    return OpVector();
}

OpVector Character::mindActionOperation(const Action & op)
{
    Action *a = new Action(op);
    a->SetTo(getId());
    return OpVector(1,a);
}

OpVector Character::mindSetupOperation(const Setup & op)
{
    Setup *s = new Setup(op);
    s->SetTo(getId());
    s->SetAttr("sub_to", "mind");
    return OpVector(1,s);
}

OpVector Character::mindTickOperation(const Tick & op)
{
    Tick *t = new Tick(op);
    t->SetTo(getId());
    t->SetAttr("sub_to", "mind");
    return OpVector(1,t);
}

OpVector Character::mindMoveOperation(const Move & op)
{
    debug( std::cout << "Character::mind_move_op" << std::endl << std::flush;);
    const Fragment::ListType & args = op.GetArgs();
    if ((args.empty()) || (!args.front().IsMap())) {
        log(ERROR, "mindMoveOperation: move op has no argument");
        return OpVector();
    }
    const Fragment::MapType & arg1 = args.front().AsMap();
    Fragment::MapType::const_iterator I = arg1.find("id");
    if ((I == arg1.end()) || !I->second.IsString()) {
        log(ERROR, "mindMoveOperation: Args has got no id");
    }
    const std::string & oname = I->second.AsString();
    EntityDict::const_iterator J = world->getObjects().find(oname);
    if (J == world->getObjects().end()) {
        log(ERROR, "mindMoveOperation: This move op is for a phoney id");
        return OpVector();
    }
    Move * newop = new Move(op);
    Entity * obj = J->second;
    if (obj != this) {
        debug( std::cout << "Moving something else. " << oname << std::endl << std::flush;);
        if ((obj->getMass() < 0) || (obj->getMass() > mass)) {
            debug( std::cout << "We can't move this. Just too heavy" << std::endl << std::flush;);
            delete newop;
            return OpVector();
        }
        newop->SetTo(oname);
        return OpVector(1,newop);
    } else {
        newop->SetTo(getId());
    }
    std::string location_ref;
    I = arg1.find("loc");
    if ((I != arg1.end()) && (I->second.IsString())) {
        location_ref = I->second.AsString();
    } else {
        debug( std::cout << "Parent not set" << std::endl << std::flush;);
    }
    Vector3D location_coords, location_vel;
    Quaternion location_orientation;
    try {
        I = arg1.find("pos");
        if (I != arg1.end()) {
            location_coords = Vector3D(I->second.AsList());
            debug( std::cout << "pos set to " << location_coords << std::endl << std::flush;);
        }

        I = arg1.find("velocity");
        if (I != arg1.end()) {
            location_vel = Vector3D(I->second.AsList());
            debug( std::cout << "vel set to " << location_vel << std::endl << std::flush;);
        }

        I = arg1.find("orientation");
        if (I != arg1.end()) {
            location_orientation = Quaternion(I->second.AsList());
            debug( std::cout << "ori set to " << location_orientation << std::endl << std::flush;);
        }
    }
    catch (Atlas::Message::WrongTypeException) {
        log(ERROR, "EXCEPTION: mindMoveOperation: Malformed move operation");
    }

    if (!location_coords.isValid()) {
        if (op.GetFutureSeconds() < 0) {
            newop->SetFutureSeconds(0);
        }
    } else {
        location_coords +=
            (Vector3D(((double)rand())/RAND_MAX, ((double)rand())/RAND_MAX, 0)
				*= (drunkness * 10));
    }
    // Print out a bunch of debug info
    debug( std::cout << ":" << location_ref << ":" << location.ref->getId()
                     << ":" << std::endl << std::flush;);
    if (((location_ref == location.ref->getId()) || (location_ref.empty())) &&
        (newop->GetFutureSeconds() >= 0)) {
        // Movement within current ref. Work out the speed and stuff and
        // use movement object to track movement.
        //
        double vel_mag;
        if (!location_vel.isValid()) {
            debug( std::cout << "\tVelocity default" << std::endl<<std::flush;);
            vel_mag = consts::base_velocity;
        } else {
            debug( std::cout << "\tVelocity: " << location_vel
                             << std::endl << std::flush;);
            vel_mag = location_vel.mag();
            if (vel_mag > consts::base_velocity) {
                vel_mag = consts::base_velocity;
            }
        }

        Vector3D direction;
        // If the position is given, and it is about right, don't bother to 
        // use it.
        if (location_coords.isValid() &&
            (location_coords.relativeDistance(location.coords) < 0.01)) {
            location_coords = Vector3D();
        }
        if (!location_coords.isValid()) {
            if (!location_vel.isValid() || location_vel.isZero()) {
                debug( std::cout << "\tUsing orientation for direction"
                                 << std::endl << std::flush;);
                // If velocity is not given, and target is not given,
                // then we are not moving at all, so direction must
                // remain invalid.
            } else {
                debug( std::cout << "\tUsing velocity for direction"
                                 << std::endl << std::flush;);
                direction = location_vel;
            }
        } else {
            debug( std::cout << "\tUsing destination for direction"
                             << std::endl << std::flush;);
            direction = Vector3D(location_coords) -= location.coords;
        }
        if (direction.isValid()) {
            direction.unit();
            debug( std::cout << "Direction: " << direction << std::endl
                             << std::flush;);
            if (!location_orientation.isValid()) {
                // This is a character walking, so it should stap upright
                Vector3D uprightDirection = direction;
                uprightDirection[Vector3D::cZ] = 0;
                uprightDirection.unit();
                location_orientation = Quaternion(Vector3D(1,0,0),
                                                  uprightDirection);
            }
        }
        Location ret_location;
        Move * moveOp = movement.genMoveOperation(&ret_location);
        const Location & current_location = (NULL!=moveOp) ? ret_location : location;
        movement.reset();
        if ((vel_mag==0) || !direction.isValid()) {
            debug( std::cout << "\tMovement stopped" << std::endl
                             << std::flush;);
            if (NULL != moveOp) {
                debug( std::cout << "Stop!" << std::endl << std::flush;);
                Fragment::ListType & args = moveOp->GetArgs();
                Fragment::MapType & ent = args.front().AsMap();
                ent["velocity"] = Vector3D(0,0,0).asObject();
                ent["mode"] = "standing";
                if (location_orientation.isValid()) {
                    ent["orientation"] = location_orientation.asObject();
                }
                moveOp->SetArgs(args);
            } else {
                debug( std::cout << "Turn!" << std::endl << std::flush;);
                movement.m_orientation = location_orientation;
                moveOp = movement.genFaceOperation();
            }
            delete newop;
            if (NULL != moveOp) {
                return OpVector(1,moveOp);
            }
            return OpVector();
        }
        Tick * tickOp = new Tick(Tick::Instantiate());
        Fragment::MapType ent;
        ent["serialno"] = movement.m_serialno;
        ent["name"] = "move";
        Fragment::ListType args(1,ent);
        tickOp->SetArgs(args);
        tickOp->SetTo(getId());
        // Need to add the arguments to this op before we return it
        // direction is already a unit vector
        debug( if (location_coords.isValid()) { std::cout<<"\tUsing target"
                                               << std::endl
                                               << std::flush; } );
        movement.m_targetPos = location_coords;
        movement.m_velocity = direction;
        movement.m_velocity *= vel_mag;
        movement.m_orientation = location_orientation;
        debug( std::cout << "Velocity " << vel_mag << std::endl << std::flush;);
        Move * moveOp2 = movement.genMoveOperation(NULL,current_location);
        tickOp->SetFutureSeconds(movement.getTickAddition(location.coords));
        debug( std::cout << "Next tick " << tickOp->GetFutureSeconds()
                         << std::endl << std::flush;);
        debug( std::cout << "moveOp = " << moveOp << ", moveOp2 = "
                         << moveOp2 << std::endl << std::flush; );
        if (NULL != moveOp2) {
            if (NULL != moveOp) {
                delete moveOp;
            }
            moveOp=moveOp2;
        }
        if (moveOp == NULL) {
            log(ERROR, "No move operation generated in mindMoveOp");
            return OpVector();
        }
        // return moveOp and tickOp;
        OpVector res(2);
        res[0] = moveOp;
        res[1] = tickOp;
        delete newop;
        return res;
    }
    return OpVector(1,newop);
}

OpVector Character::mindSetOperation(const Set & op)
{
    const Fragment::ListType & args = op.GetArgs();
    if (args.front().IsMap()) {
        Set * s = new Set(op);
        const Fragment::MapType & amap = args.front().AsMap();
        Fragment::MapType::const_iterator I = amap.find("id");
        if (I != amap.end() && I->second.IsString()) {
            const std::string & opid = I->second.AsString();
            s->SetTo(opid);
        } else {
            if (op.GetTo().empty()) {
                s->SetTo(getId());
            }
        }
        return OpVector(1,s);
    }
    return OpVector();
}

OpVector Character::mindSightOperation(const Sight & op)
{
    return OpVector();
}

OpVector Character::mindSoundOperation(const Sound & op)
{
    return OpVector();
}

OpVector Character::mindChopOperation(const Chop & op)
{
    return OpVector();
}

OpVector Character::mindCombineOperation(const Combine & op)
{
    return OpVector();
}

OpVector Character::mindCreateOperation(const Create & op)
{
    Create * c = new Create(op);
    c->SetTo(getId());
    return OpVector(1,c);
}

OpVector Character::mindDeleteOperation(const Delete & op)
{
    Delete * d = new Delete(op);
    d->SetTo(getId());
    return OpVector(1,d);
}

OpVector Character::mindDivideOperation(const Divide & op)
{
    return OpVector();
}

OpVector Character::mindBurnOperation(const Burn & op)
{
    return OpVector();
}

OpVector Character::mindGetOperation(const Get & op)
{
    return OpVector();
}

OpVector Character::mindImaginaryOperation(const Imaginary & op)
{
    Imaginary * i = new Imaginary(op);
    i->SetTo(getId());
    return OpVector(1,i);
}

OpVector Character::mindInfoOperation(const Info & op)
{
    return OpVector();
}

OpVector Character::mindNourishOperation(const Nourish & op)
{
    return OpVector();
}

OpVector Character::mindTalkOperation(const Talk & op)
{
    debug( std::cout << "Character::mindOPeration(Talk)"
                     << std::endl << std::flush;);
    Talk * t = new Talk(op);
    t->SetTo(getId());
    return OpVector(1,t);
}

OpVector Character::mindLookOperation(const Look & op)
{
    debug(std::cout << "Got look up from mind from [" << op.GetFrom()
               << "] to [" << op.GetTo() << "]" << std::endl << std::flush;);
    perceptive = true;
    Look * l = new Look(op);
    if (op.GetTo().empty()) {
        const Fragment::ListType & args = op.GetArgs();
        if (args.empty()) {
            l->SetTo(world->getId());
        } else {
            if (args.front().IsMap()) {
                const Fragment::MapType & amap = args.front().AsMap();
                Fragment::MapType::const_iterator I = amap.find("id");
                if (I != amap.end() && I->second.IsString()) {
                    l->SetTo(I->second.AsString());
                }
            }
        }
    }
    debug( std::cout <<"    now to ["<<l->GetTo()<<"]"<<std::endl<<std::flush;);
    return OpVector(1,l);
}

OpVector Character::mindCutOperation(const Cut & op)
{
    Cut * c = new Cut(op);
    if (op.GetTo().empty()) {
        c->SetTo(getId());
    }
    return OpVector(1,c);
}

OpVector Character::mindEatOperation(const Eat & op)
{
    Eat * e = new Eat(op);
    if (op.GetTo().empty()) {
        e->SetTo(getId());
    }
    return OpVector(1,e);
}

OpVector Character::mindTouchOperation(const Touch & op)
{
    Touch * t = new Touch(op);
    // Work out what is being touched.
    const Fragment::ListType & args = op.GetArgs();
    if ((op.GetTo().empty()) || (!args.empty())) {
        if (args.empty()) {
            t->SetTo(world->getId());
        } else {
            if (args.front().IsMap()) {
                const Fragment::MapType & amap = args.front().AsMap();
                Fragment::MapType::const_iterator I = amap.find("id");
                if (I != amap.end() && I->second.IsString()) {
                    t->SetTo(I->second.AsString());
                }
            } else if (args.front().IsString()) {
                t->SetTo(args.front().AsString());
            }
        }
    }
    // Pass the modified touch operation on to target.
    OpVector res(2);
    res[0] = t;
    // Send action "touch"
    Action * a = new Action(Action::Instantiate());
    a->SetTo(getId());
    Fragment::MapType amap;
    amap["id"] = getId();
    amap["action"] = "touch";
    Fragment::ListType setArgs(1,amap);
    a->SetArgs(setArgs);
    res[1] = a;
    return res;
}

OpVector Character::mindAppearanceOperation(const Appearance & op)
{
    return OpVector();
}

OpVector Character::mindDisappearanceOperation(const Disappearance & op)
{
    return OpVector();
}


OpVector Character::mindErrorOperation(const Error & op)
{
    return OpVector();
}

OpVector Character::mindOtherOperation(const RootOperation & op)
{
    RootOperation * e = new RootOperation(op);
    if (op.GetTo().empty()) {
        e->SetTo(getId());
    }
    return OpVector(1,e);
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
    if (drunkness > 1.0) {
        return false;
    }
    return true;
}

bool Character::w2mDisappearanceOperation(const Disappearance & op)
{
    if (drunkness > 1.0) {
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
    if (op.HasAttr("sub_to")) {
        return true;
    }
    return false;
}

bool Character::w2mTickOperation(const Tick & op)
{
    if (op.HasAttr("sub_to")) {
        return true;
    }
    return false;
}

bool Character::w2mSightOperation(const Sight & op)
{
    if (drunkness > 1.0) {
        return false;
    }
    return true;
}

bool Character::w2mSoundOperation(const Sound & op)
{
    if (drunkness > 1.0) {
        return false;
    }
    return true;
}

bool Character::w2mTouchOperation(const Touch & op)
{
    if (drunkness > 1.0) {
        return false;
    }
    return true;
}

OpVector Character::sendMind(const RootOperation & op)
{
    debug( std::cout << "Character::sendMind" << std::endl << std::flush;);
    if (mind == NULL) {
        return OpVector();
    }
    OpVector res = mind->message(op);

    if (NULL != externalMind) {
        debug( std::cout << "Sending to external mind" << std::endl
                         << std::flush;);
        // Discard all the local results
        OpVector::const_iterator J = res.begin(); 
        for(; J != res.end(); J++) {
            delete *J;
        }
        res = externalMind->message(op);
    }

    // At this point there is a bunch of conversion stuff that I don't
    // understand
    
    debug(std::cout << "Using " << res.size() << " ops from "
                    << ((NULL == externalMind) ? "local mind" : "external mind")
                    << std::endl << std::flush;);

    return res;
}

OpVector Character::mind2body(const RootOperation & op)
{
    debug( std::cout << "Character::mind2body" << std::endl << std::flush;);

    if (drunkness > 1.0) {
        return OpVector();
    }
    OpNo otype = opEnumerate(op, opMindLookup);
    OP_SWITCH(op, otype, mind)
    return OpVector();
}

OpVector Character::world2body(const RootOperation & op)
{
    debug( std::cout << "Character::world2body" << std::endl << std::flush;);
    return callOperation(op);
}

bool Character::world2mind(const RootOperation & op)
{
    debug( std::cout << "Character::world2mind" << std::endl << std::flush;);
    OpNo otype = opEnumerate(op, opW2mLookup);
    OP_SWITCH(op, otype, w2m)
    return false;
}

OpVector Character::externalMessage(const RootOperation & op)
{
    debug( std::cout << "Character::externalMessage" << std::endl << std::flush;);
    return externalOperation(op);
}

OpVector Character::operation(const RootOperation & op)
{
    debug( std::cout << "Character::operation" << std::endl << std::flush;);
    OpVector result = world2body(op);
    // set refno on result?
    if (!isAlive) {
        return result;
    }
    if (world2mind(op)) {
        OpVector mres2 = sendMind(op);
        for(OpVector::const_iterator I = mres2.begin(); I != mres2.end(); I++) {
            //RootOperation * mr2 = mind2_res.front();
            // Need to be very careful about what this actually does
            externalMessage(**I);
            delete *I;
        }
    }
    return result;
}

OpVector Character::externalOperation(const RootOperation & op)
{
    debug( std::cout << "Character::externalOperation" << std::endl << std::flush;);
    OpVector res = mind2body(op);
    
    // We require that the first op is the direct consequence of the minds
    // op, so it gets the same serialno
    OpVector::const_iterator I = res.begin();
    for(; I != res.end(); I++) {
        if (I == res.begin()) {
            (*I)->SetSerialno(op.GetSerialno());
        } else {
            world->setSerialnoOp(**I);
        }
        sendWorld(*I);
        // Don't delete br as it has gone into worlds queue
        // World will deal with it.
    }
    return OpVector();
}

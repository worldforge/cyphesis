// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

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

#include <common/Setup.h>
#include <common/Tick.h>
#include <common/Cut.h>
#include <common/Eat.h>
#include <common/Nourish.h>

#include <varconf/Config.h>

extern "C" {
    #include <stdlib.h>
}

#include "Character.h"
#include "Pedestrian.h"

#include "ExternalMind.h"
#include "Script.h"
#include "Python_API.h" // FIXME This must go
#include "World.h"

#include <server/WorldRouter.h>

#include <common/op_switch.h>
#include <common/const.h>
#include <common/debug.h>
#include <common/globals.h>

using Atlas::Message::Object;

static const bool debug_flag = false;

oplist Character::metabolise(double ammount = 1)
{
    // Currently handles energy
    // We should probably call this whenever the entity performs a movement.
    Object::MapType ent;
    ent["id"] = fullid;
    if ((status > (1.5 + energyLoss)) && (weight < maxWeight)) {
        status = status - energyLoss;
        ent["weight"] = weight + weightGain;
    }
    double energyUsed = energyConsumption * ammount;
    if ((status <= energyUsed) && (weight > weightConsumption)) {
        ent["status"] = status - energyUsed + energyGain;
        ent["weight"] = weight - weightConsumption;
    } else {
        ent["status"] = status - energyUsed;
    }
    if (drunkness > 0) {
        ent["drunkness"] = drunkness - 0.1;
    }

    Set * s = new Set(Set::Instantiate());
    s->SetTo(fullid);
    s->SetArgs(Object::ListType(1,ent));

    return oplist(1,s);
}

Character::Character() : movement(*new Pedestrian(*this)), autom(false),
                         drunkness(0.0), sex("female"), food(0),
                         maxWeight(100), isAlive(true),
                         mind(NULL), externalMind(NULL)
{
    isCharacter = true;
    weight = 60;
    location.bbox = Vector3D(0.25, 0.25, 1);
    location.bmedian = Vector3D(0, 0, 1);
    attributes["mode"] = Object("birth");
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

const Object & Character::operator[](const string & aname)
{
    if (aname == "drunkness") {
        attributes[aname] = Object(drunkness);
    } else if (aname == "sex") {
        attributes[aname] = Object(sex);
    }
    return Thing::operator[](aname);
}

void Character::set(const string & aname, const Object & attr)
{
    if ((aname == "drunkness") && attr.IsFloat()) {
        drunkness = attr.AsFloat();
    } else if ((aname == "sex") && attr.IsString()) {
        sex = attr.AsString();
    } else {
        Thing::set(aname, attr);
    }
}

void Character::addToObject(Object & obj) const
{
    Object::MapType & omap = obj.AsMap();
    omap["sex"] = Object(sex);
    Thing::addToObject(obj);
}

oplist Character::ImaginaryOperation(const Imaginary & op)
{
    Sight * s = new Sight(Sight::Instantiate());
    s->SetArgs(Object::ListType(1,op.AsObject()));
    return oplist(1,s);
}

oplist Character::SetupOperation(const Setup & op)
{
    debug( cout << "Character::tick" << endl << flush;);
    oplist res;
    debug( cout << "CHaracter::Operation(setup)" << endl << flush;);
    if (script->Operation("setup", op, res) != 0) {
        return res;
    }
    if (op.HasAttr("sub_to")) {
        debug( cout << "Has sub_to" << endl << flush;);
        return res;
    }

    mind = new BaseMind(fullid, name);
    string mind_class("NPCMind"), mind_package("mind.NPCMind");
    if (global_conf->findItem("mind", type)) {
        mind_package = global_conf->getItem("mind", type);
        mind_class = type + "Mind";
    }
    Create_PyMind(mind, mind_package, mind_class);

    oplist res2(2);
    Setup * s = new Setup(op);
    // THis is so not the right thing to do
    s->SetAttr("sub_to", Object("mind"));
    res2[0] = s;
    Look * l = new Look(Look::Instantiate());
    l->SetTo(world->fullid);
    res2[1] = l;
    if (location.ref != &world->gameWorld) {
        l = new Look(Look::Instantiate());
        l->SetTo(location.ref->fullid);
        res2.push_back(l);
    }
    l = new Look(Look::Instantiate());
    l->SetTo(fullid);
    res2.push_back(l);
    Tick * tick = new Tick(Tick::Instantiate());
    tick->SetTo(fullid);
    res2.push_back(tick);
    return res2;
}

oplist Character::TickOperation(const Tick & op)
{
    if (op.HasAttr("sub_to")) {
        debug( cout << "Has sub_to" << endl << flush;);
        return oplist();
    }
    debug(cout << "================================" << endl << flush;);
    const Object::ListType & args = op.GetArgs();
    if ((0 != args.size()) && (args.front().IsMap())) {
        // Deal with movement.
        const Object::MapType & arg1 = args.front().AsMap();
        Object::MapType::const_iterator I = arg1.find("serialno");
        if ((I != arg1.end()) && (I->second.IsInt())) {
            if (I->second.AsInt() < movement.m_serialno) {
                debug(cout << "Old tick" << endl << flush;);
                return oplist();
            }
        }
        Location ret_loc;
        Move * moveOp = movement.genMoveOperation(&ret_loc);
        if (moveOp) {
            oplist res(2);
            Object::MapType entmap;
            entmap["name"]=Object("move");
            entmap["serialno"]=Object(movement.m_serialno);
            Object ent(entmap);
            Tick * tickOp = new Tick(Tick::Instantiate());
            tickOp->SetTo(fullid);
            tickOp->SetFutureSeconds(movement.getTickAddition(ret_loc.coords));
            tickOp->SetArgs(Object::ListType(1,ent));
            res[0] = tickOp;
            res[1] = moveOp;
            return res;
        }
    } else {
        oplist res;
        script->Operation("tick", op, res);

        // DIGEST
        if ((food >= foodConsumption) && (status < 2)) {
            // It is important that the metabolise bit is done next, as this
            // handles the status change
            status = status + foodConsumption;
            food = food - foodConsumption;

            Object::MapType food_ent;
            food_ent["id"] = fullid;
            food_ent["food"] = food;
            Set s = Set::Instantiate();
            s.SetTo(fullid);
            s.SetArgs(Object::ListType(1,food_ent));

            Sight * si = new Sight(Sight::Instantiate());
            si->SetTo(fullid);
            si->SetArgs(Object::ListType(1,s.AsObject()));
            res.push_back(si);
        }

        // METABOLISE
        oplist mres = metabolise();
        for(oplist::const_iterator I = mres.begin(); I != mres.end(); I++) {
            res.push_back(*I);
        }
        
        // TICK
        Tick * tickOp = new Tick(Tick::Instantiate());
        tickOp->SetTo(fullid);
        tickOp->SetFutureSeconds(consts::basic_tick * 30);
        res.push_back(tickOp);
        return res;
    }
    return oplist();
}

oplist Character::TalkOperation(const Talk & op)
{
    debug( cout << "Character::OPeration(Talk)" << endl << flush;);
    Sound * s = new Sound(Sound::Instantiate());
    s->SetArgs(Object::ListType(1,op.AsObject()));
    return oplist(1,s);
}

oplist Character::EatOperation(const Eat & op)
{
    // This is identical to Foof::Operation(Eat &)
    // Perhaps animal should inherit from Food?
    oplist res;
    if (script->Operation("eat", op, res) != 0) {
        return res;
    }
    Object::MapType self_ent;
    self_ent["id"] = fullid;
    self_ent["status"] = -1;

    Set * s = new Set(Set::Instantiate());
    s->SetTo(fullid);
    s->SetArgs(Object::ListType(1,self_ent));

    const string & to = op.GetFrom();
    Object::MapType nour_ent;
    nour_ent["id"] = to;
    nour_ent["weight"] = weight;
    Nourish * n = new Nourish(Nourish::Instantiate());
    n->SetTo(to);
    n->SetArgs(Object::ListType(1,nour_ent));

    oplist res2(2);
    res2[0] = s;
    res2[1] = n;
    return res2;
}

oplist Character::NourishOperation(const Nourish & op)
{
    const Object::MapType & nent = op.GetArgs().front().AsMap();
    Object::MapType::const_iterator I = nent.find("weight");
    if (I == nent.end()) { return oplist(); }
    food = food + I->second.AsNum();

    Object::MapType food_ent;
    food_ent["id"] = fullid;
    food_ent["food"] = food;
    if ((I = nent.find("alcahol")) != nent.end()) {
        drunkness += I->second.AsNum() / weight;
        food_ent["drunkness"] = drunkness;
    }
    Set s = Set::Instantiate();
    s.SetArgs(Object::ListType(1,food_ent));

    Sight * si = new Sight(Sight::Instantiate());
    si->SetTo(fullid);
    si->SetArgs(Object::ListType(1,s.AsObject()));
    return oplist(1,si);
}

oplist Character::mindLoginOperation(const Login & op)
{
    return oplist();
}

oplist Character::mindActionOperation(const Action & op)
{
    Action *a = new Action(op);
    a->SetTo(fullid);
    return oplist(1,a);
}

oplist Character::mindSetupOperation(const Setup & op)
{
    Setup *s = new Setup(op);
    s->SetTo(fullid);
    s->SetAttr("sub_to", Object("mind"));
    return oplist(1,s);
}

oplist Character::mindTickOperation(const Tick & op)
{
    Tick *t = new Tick(op);
    t->SetTo(fullid);
    t->SetAttr("sub_to", Object("mind"));
    return oplist(1,t);
}

oplist Character::mindMoveOperation(const Move & op)
{
    debug( cout << "Character::mind_move_op" << endl << flush;);
    const Object::ListType & args = op.GetArgs();
    if ((0 == args.size()) || (!args.front().IsMap())) {
        cerr << "move op has no argument" << endl << flush;
        return oplist();
    }
    const Object::MapType & arg1 = args.front().AsMap();
    Object::MapType::const_iterator I = arg1.find("id");
    if ((I == arg1.end()) || !I->second.IsString()) {
        cerr << "Its got no id" << endl << flush;
    }
    const string & oname = I->second.AsString();
    edict_t::const_iterator J = world->eobjects.find(oname);
    if (J == world->eobjects.end()) {
        debug( cout << "This move op is for a phoney object" << endl << flush;);
        return oplist();
    }
    Move * newop = new Move(op);
    Entity * obj = J->second;
    if (obj != this) {
        debug( cout << "Moving something else. " << oname << endl << flush;);
        if ((obj->weight < 0) || (obj->weight > weight)) {
            debug( cout << "We can't move this. Just too heavy" << endl << flush;);
            delete newop;
            return oplist();
        }
        newop->SetTo(oname);
        return oplist(1,newop);
    }
    string location_ref;
    I = arg1.find("loc");
    if ((I != arg1.end()) && (I->second.IsString())) {
        location_ref = I->second.AsString();
    } else {
        debug( cout << "Parent not set" << endl << flush;);
    }
    Vector3D location_coords, location_vel, location_face;
    try {
        I = arg1.find("pos");
        if (I != arg1.end()) {
            location_coords = Vector3D(I->second.AsList());
        }

        I = arg1.find("velocity");
        if (I != arg1.end()) {
            location_vel = Vector3D(I->second.AsList());
        }

        I = arg1.find("face");
        if (I != arg1.end()) {
            location_face = Vector3D(I->second.AsList());
        }
    }
    catch (Atlas::Message::WrongTypeException) {
        cerr << "EXCEPTION: Malformed move operation from mind" <<endl<<flush;
    }

    if (!location_coords) {
        if (op.GetFutureSeconds() < 0) {
            newop->SetFutureSeconds(0);
        }
    } else {
        location_coords = location_coords +
            (Vector3D(((double)rand())/RAND_MAX, ((double)rand())/RAND_MAX, 0)
				* drunkness * 10);
    }
    // Print out a bunch of debug info
    debug( cout << ":" << location_ref << ":" << location.ref->fullid << ":" << endl << flush;);
    if (((location_ref == location.ref->fullid) || (location_ref.empty())) &&
        (newop->GetFutureSeconds() >= 0)) {
        // Movement within current ref. Work out the speed and stuff and
        // use movement object to track movement.
        //
        double vel_mag;
        if (!location_vel) {
            debug( cout << "\tVelocity default" << endl << flush;);
            vel_mag = consts::base_velocity;
        } else {
            debug( cout << "\tVelocity: " << location_vel << endl << flush;);
            vel_mag = location_vel.mag();
            if (vel_mag > consts::base_velocity) {
                vel_mag = consts::base_velocity;
            }
        }
        if (location_face) {
            location.face = location_face;
        }

        Vector3D direction;
        if (location_coords == location.coords) {
            location_coords = Vector3D();
        }
        if (!location_coords) {
            if (!location_vel || (location_vel==Vector3D(0,0,0))) {
                debug( cout << "\tUsing face for direction" << endl << flush;);
                direction=location.face;
            } else {
                debug( cout << "\tUsing velocity for direction"<<endl<<flush;);
                direction=location_vel;
            }
        } else {
            debug( cout << "\tUsing destination for direction"<< endl<< flush;);
            direction=location_coords-location.coords;
        }
        if (direction) {
            direction=direction.unitVector();
            debug( cout << "Direction: " << direction << endl << flush;);
        }
        if (!location_face) {
            location.face = direction;
        }
        Location ret_location;
        Move * moveOp = movement.genMoveOperation(&ret_location);
        const Location & current_location = (NULL!=moveOp) ? ret_location : location;
        movement.reset();
        if ((vel_mag==0) || !direction) {
            debug( cout << "\tMovement stopped" << endl << flush;);
            if (NULL != moveOp) {
                Object::ListType & args = moveOp->GetArgs();
                Object::MapType & ent = args.front().AsMap();
                ent["velocity"] = Vector3D(0,0,0).asObject();
                ent["mode"] = Object("standing");
                moveOp->SetArgs(args);
            } else {
                moveOp = movement.genFaceOperation(location);
            }
            delete newop;
            if (NULL != moveOp) {
                return oplist(1,moveOp);
            }
            return oplist();
        }
        Tick * tickOp = new Tick(Tick::Instantiate());
        Object::MapType ent;
        ent["serialno"] = Object(movement.m_serialno);
        ent["name"] = Object("move");
        Object::ListType args(1,ent);
        tickOp->SetArgs(args);
        tickOp->SetTo(fullid);
        // Need to add the arguments to this op before we return it
        // direction is already a unit vector
        debug( if (location_coords) { cout<<"\tUsing target"<<endl<<flush; } );
        movement.m_targetPos = location_coords;
        movement.m_velocity = direction * vel_mag;
        debug( cout << "Velocity " << vel_mag << endl << flush;);
        Move * moveOp2 = movement.genMoveOperation(NULL,current_location);
        tickOp->SetFutureSeconds(movement.getTickAddition(location.coords));
        debug( cout << "Next tick " << tickOp->GetFutureSeconds() << endl << flush;);
        if (NULL != moveOp2) {
            if (NULL != moveOp) {
                delete moveOp;
            }
            moveOp=moveOp2;
        }
        // return moveOp and tickOp;
        oplist res(2);
        res[0] = moveOp;
        res[1] = tickOp;
        delete newop;
        return res;
    }
    return oplist(1,newop);
}

oplist Character::mindSetOperation(const Set & op)
{
    Set * s = new Set(op);
    const Object::ListType & args = op.GetArgs();
    if (args.front().IsMap()) {
        const Object::MapType & amap = args.front().AsMap();
        Object::MapType::const_iterator I = amap.find("id");
        if (I != amap.end() && I->second.IsString()) {
            const string & opid = I->second.AsString();
            if (opid != fullid) {
                s->SetTo(opid);
            }
        }
    }
    return oplist(1,s);
}

oplist Character::mindSightOperation(const Sight & op)
{
    return oplist();
}

oplist Character::mindSoundOperation(const Sound & op)
{
    return oplist();
}

oplist Character::mindChopOperation(const Chop & op)
{
    return oplist();
}

oplist Character::mindCombineOperation(const Combine & op)
{
    return oplist();
}

oplist Character::mindCreateOperation(const Create & op)
{
    Create * c = new Create(op);
    return oplist(1,c);
}

oplist Character::mindDeleteOperation(const Delete & op)
{
    Delete * d = new Delete(op);
    return oplist(1,d);
}

oplist Character::mindDivideOperation(const Divide & op)
{
    return oplist();
}

oplist Character::mindFireOperation(const Fire & op)
{
    return oplist();
}

oplist Character::mindGetOperation(const Get & op)
{
    return oplist();
}

oplist Character::mindImaginaryOperation(const Imaginary & op)
{
    Imaginary * i = new Imaginary(op);
    return oplist(1,i);
}

oplist Character::mindInfoOperation(const Info & op)
{
    return oplist();
}

oplist Character::mindNourishOperation(const Nourish & op)
{
    return oplist();
}

oplist Character::mindTalkOperation(const Talk & op)
{
    debug( cout << "Character::mindOPeration(Talk)" << endl << flush;);
    Talk * t = new Talk(op);
    return oplist(1,t);
}

oplist Character::mindLookOperation(const Look & op)
{
    debug(cout << "Got look up from mind from [" << op.GetFrom()
               << "] to [" << op.GetTo() << "]" << endl << flush;);
    perceptive = true;
    Look * l = new Look(op);
    if (op.GetTo().size() == 0) {
        const Object::ListType & args = op.GetArgs();
        if (args.size() == 0) {
            l->SetTo(world->fullid);
        } else {
            if (args.front().IsMap()) {
                const Object::MapType & amap = args.front().AsMap();
                Object::MapType::const_iterator I = amap.find("id");
                if (I != amap.end() && I->second.IsString()) {
                    l->SetTo(I->second.AsString());
                }
            }
        }
    }
    debug( cout <<"    now to ["<<l->GetTo()<<"]"<<endl<<flush;);
    return oplist(1,l);
}

oplist Character::mindLoadOperation(const Load & op)
{
    return oplist();
}

oplist Character::mindSaveOperation(const Save & op)
{
    return oplist();
}

oplist Character::mindCutOperation(const Cut & op)
{
    Cut * c = new Cut(op);
    return oplist(1,c);
}

oplist Character::mindEatOperation(const Eat & op)
{
    Eat * e = new Eat(op);
    return oplist(1,e);
}

oplist Character::mindTouchOperation(const Touch & op)
{
    Touch * t = new Touch(op);
    // Work out what is being touched.
    const Object::ListType & args = op.GetArgs();
    if ((op.GetTo().size() == 0) || (args.size() != 0)) {
        if (args.size() == 0) {
            t->SetTo(world->fullid);
        } else {
            if (args.front().IsMap()) {
                const Object::MapType & amap = args.front().AsMap();
                Object::MapType::const_iterator I = amap.find("id");
                if (I != amap.end() && I->second.IsString()) {
                    t->SetTo(I->second.AsString());
                }
            } else if (args.front().IsString()) {
                t->SetTo(args.front().AsString());
            }
        }
    }
    // Pass the modified touch operation on to target.
    oplist res(2);
    res[0] = t;
    // Send action "touch"
    Action * a = new Action(Action::Instantiate());
    a->SetTo(fullid);
    Object::MapType amap;
    amap["id"] = fullid;
    amap["action"] = "touch";
    Object::ListType setArgs(1,Object(amap));
    a->SetArgs(setArgs);
    res[1] = a;
    return res;
}

oplist Character::mindAppearanceOperation(const Appearance & op)
{
    return oplist();
}

oplist Character::mindDisappearanceOperation(const Disappearance & op)
{
    return oplist();
}


oplist Character::mindErrorOperation(const Error & op)
{
    return oplist();
}

oplist Character::mindOtherOperation(const RootOperation & op)
{
    RootOperation * e = new RootOperation(op);
    return oplist(1,e);
}

oplist Character::w2mActionOperation(const Action & op)
{
    return oplist();
}

oplist Character::w2mLoginOperation(const Login & op)
{
    return oplist();
}

oplist Character::w2mChopOperation(const Chop & op)
{
    return oplist();
}

oplist Character::w2mCreateOperation(const Create & op)
{
    return oplist();
}

oplist Character::w2mCutOperation(const Cut & op)
{
    return oplist();
}

oplist Character::w2mDeleteOperation(const Delete & op)
{
    return oplist();
}

oplist Character::w2mEatOperation(const Eat & op)
{
    return oplist();
}

oplist Character::w2mFireOperation(const Fire & op)
{
    return oplist();
}

oplist Character::w2mMoveOperation(const Move & op)
{
    return oplist();
}

oplist Character::w2mSetOperation(const Set & op)
{
    return oplist();
}

oplist Character::w2mLookOperation(const Look & op)
{
    return oplist();
}

oplist Character::w2mLoadOperation(const Load & op)
{
    return oplist();
}

oplist Character::w2mSaveOperation(const Save & op)
{
    return oplist();
}

oplist Character::w2mDivideOperation(const Divide & op)
{
    return oplist();
}

oplist Character::w2mCombineOperation(const Combine & op)
{
    return oplist();
}

oplist Character::w2mGetOperation(const Get & op)
{
    return oplist();
}

oplist Character::w2mImaginaryOperation(const Imaginary & op)
{
    return oplist();
}

oplist Character::w2mInfoOperation(const Info & op)
{
    return oplist();
}

oplist Character::w2mTalkOperation(const Talk & op)
{
    return oplist();
}

oplist Character::w2mNourishOperation(const Nourish & op)
{
    return oplist();
}

oplist Character::w2mAppearanceOperation(const Appearance & op)
{
    if (drunkness > 1.0) {
        return oplist();
    }
    Appearance * a = new Appearance(op);
    return oplist(1,a);
}

oplist Character::w2mDisappearanceOperation(const Disappearance & op)
{
    if (drunkness > 1.0) {
        return oplist();
    }
    Disappearance * d = new Disappearance(op);
    return oplist(1,d);
}

oplist Character::w2mErrorOperation(const Error & op)
{
    Error * e = new Error(op);
    return oplist(1,e);
}

oplist Character::w2mOtherOperation(const RootOperation & op)
{
    RootOperation * r = new RootOperation(op);
    return oplist(1,r);
}

oplist Character::w2mSetupOperation(const Setup & op)
{
    if (op.HasAttr("sub_to")) {
        Setup * s = new Setup(op);
        return oplist(1,s);
    }
    return oplist();
}

oplist Character::w2mTickOperation(const Tick & op)
{
    if (op.HasAttr("sub_to")) {
        Tick * t = new Tick(op);
        return oplist(1,t);
    }
    return oplist();
}

oplist Character::w2mSightOperation(const Sight & op)
{
    if (drunkness > 1.0) {
        return oplist();
    }
    Sight * s = new Sight(op);
    return oplist(1,s);
}

oplist Character::w2mSoundOperation(const Sound & op)
{
    if (drunkness > 1.0) {
        return oplist();
    }
    Sound * s = new Sound(op);
    return oplist(1,s);
}

oplist Character::w2mTouchOperation(const Touch & op)
{
    if (drunkness > 1.0) {
        return oplist();
    }
    Touch * t = new Touch(op);
    return oplist(1,t);
}

oplist Character::sendMind(const RootOperation & op)
{
    debug( cout << "Character::sendMind" << endl << flush;);
    if (mind == NULL) {
        return oplist();
    }
    oplist local_res = mind->message(op);
    oplist external_res;

    if (NULL != externalMind) {
        debug( cout << "Sending to external mind" << endl << flush;);
        external_res = externalMind->message(op);
    } else {
        if (!autom) {
            debug( cout << "Turning automatic on for " << fullid << endl << flush;);
            autom = true;
        }
    }
    debug(cout << "Using " << local_res.size() << " ops from "
               << (autom ? "local mind" : "external mind")
               << endl << flush;);

    // This is the list that is to be passed back to the world
    const oplist & res = autom ? local_res : external_res;
    // This list of ops is to be discarded
    const oplist & dump_res = autom ? external_res : local_res ;
    for(oplist::const_iterator J = dump_res.begin(); J != dump_res.end(); J++) {
        delete *J;
    }
    // At this point there is a bunch of conversion stuff that I don't
    // understand
    
    return res;
}

oplist Character::mind2body(const RootOperation & op)
{
    debug( cout << "Character::mind2body" << endl << flush;);
    RootOperation newop(op);

    if ((newop.GetTo().size() == 0) &&
        (op.GetParents().front().AsString() != "look")) {
       newop.SetTo(fullid);
    }
    if (drunkness > 1.0) {
        return oplist();
    }
    op_no_t otype = opEnumerate(newop);
    OP_SWITCH(newop, otype, mind)
}

oplist Character::world2body(const RootOperation & op)
{
    debug( cout << "Character::world2body" << endl << flush;);
    return callOperation(op);
}

oplist Character::world2mind(const RootOperation & op)
{
    debug( cout << "Character::world2mind" << endl << flush;);
    op_no_t otype = opEnumerate(op);
    OP_SWITCH(op, otype, w2m)
}

oplist Character::externalMessage(const RootOperation & op)
{
    debug( cout << "Character::externalMessage" << endl << flush;);
    return externalOperation(op);
}

oplist Character::operation(const RootOperation & op)
{
    debug( cout << "Character::operation" << endl << flush;);
    oplist result = world2body(op);
    // set refno on result?
    if (!isAlive) {
        return result;
    }
    oplist mres = world2mind(op);
    // set refno on mres?
    for(oplist::const_iterator I = mres.begin(); I != mres.end(); I++) {
        //RootOperation * mr = mind_res.front();
        oplist mres2 = sendMind(**I);
        for(oplist::const_iterator J = mres2.begin(); J != mres2.end(); J++) {
            //RootOperation * mr2 = mind2_res.front();
            // Need to be very careful about what this actually does
            externalMessage(**J);
            delete *J;
        }
        delete *I;
    }
    return result;
}

oplist Character::externalOperation(const RootOperation & op)
{
    debug( cout << "Character::externalOperation" << endl << flush;);
    oplist body_res = mind2body(op);
    // set refno on body_res?
    
    for(oplist::const_iterator I = body_res.begin(); I != body_res.end(); I++) {
        sendWorld(*I);
        // Don't delete br as it has gone into worlds queue
        // World will deal with it.
    }
    return oplist();
}

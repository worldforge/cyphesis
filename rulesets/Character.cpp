#include <Atlas/Message/Object.h>
#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/Operation/Login.h>
#include <Atlas/Objects/Operation/Create.h>
#include <Atlas/Objects/Operation/Sound.h>
#include <Atlas/Objects/Operation/Create.h>
#include <Atlas/Objects/Operation/Set.h>
#include <Atlas/Objects/Operation/Delete.h>
#include <Atlas/Objects/Operation/Look.h>
#include <Atlas/Objects/Operation/Create.h>
#include <Atlas/Objects/Operation/Talk.h>
#include <Atlas/Objects/Operation/Touch.h>
#include <Atlas/Objects/Operation/Sight.h>
#include <Atlas/Objects/Operation/Move.h>
#include <Atlas/Objects/Operation/Error.h>

#include <varconf/Config.h>

extern "C" {
    #include <stdlib.h>
}

// #define DEBUG_MOVEMENT


#include "Character.h"
#include "Thing.h"

#include "BaseMind.h"
#include "ExternalMind.h"

#include <server/WorldRouter.h>

#include <modules/Location.h>

#include <common/WorldInfo.h>
#include <common/const.h>

using Atlas::Message::Object;

static int debug_movement=0;
static int debug_character=0;

MovementInfo::MovementInfo(Character * body) : body(body)
{
    serialno=-1;
    reset();
    debug_movement && cout << "MOVEMENTINFO: " << last_movement_time <<  endl << flush;
}

void MovementInfo::reset()
{
    serialno = serialno+1;
    target_location=Vector3D();
    updated_location=Vector3D();
    velocity=Vector3D(0,0,0);
    last_movement_time=world_info::time;
}

bool MovementInfo::update_needed(const Location & location) const
{
    return((velocity!=Vector3D(0,0,0))||(location.velocity!=Vector3D(0,0,0)));
}

Move * MovementInfo::gen_face_operation(Location & loc)
{
    if (face != loc.face) {
        face = loc.face;
        debug_movement && cout << "Turning" << endl << flush;
        Move * moveOp = new Move;
        *moveOp = Move::Instantiate();
        moveOp->SetTo(body->fullid);
        Object::MapType _map;
        Object ent(_map);
        Object::MapType & entmap = ent.AsMap();
        entmap["id"] = body->fullid;
        loc.addObject(&ent);
        Object::ListType args(1,ent);
        moveOp->SetArgs(args);
        return moveOp;
    }
    return(NULL);
}

Move * MovementInfo::gen_move_operation(Location * rloc)
{
    return(gen_move_operation(rloc, body->location));
}

Move * MovementInfo::gen_move_operation(Location * rloc, Location & loc)
{
        debug_movement && cout << "gen_move_operation: status: MovementInfo(" << serialno
             << "," << target_location << "," << velocity << ","
             << last_movement_time << ")" << endl << flush;
    if (update_needed(loc)) {
        debug_movement && cout << "gen_move_operation: Update needed..." << endl << flush;
        double current_time=world_info::time;
        double time_diff=current_time-last_movement_time;
        debug_movement && cout << "time_diff:" << time_diff << endl << flush;
        last_movement_time=current_time;
        Location new_loc=loc;
        Object::MapType _map;
        Object ent(_map);
        Object::MapType & entmap = ent.AsMap();
        new_loc.velocity=velocity;
        Move * moveOp = new Move;
        *moveOp = Move::Instantiate();
        moveOp->SetTo(body->fullid);
        entmap["id"] = body->fullid;
        double vel_mag = velocity.mag();
        double speed_ratio;
        if (vel_mag == 0.0) {
            speed_ratio = 0.0;
        } else {
            speed_ratio = vel_mag/consts::base_velocity;
        }
        string mode;
        if (speed_ratio > 0.5) {
            mode = string("running");
        } else if (speed_ratio > 0.05) {
            mode = string("walking");
        } else {
            mode = string("standing");
        }
        debug_movement && cout << "Mode set to " << mode << endl << flush;

        face = loc.face;

        entmap["mode"] = Object(mode);
        if (!velocity) {
            debug_movement && cout << "only velocity changed." << endl << flush;
            new_loc.addObject(&ent);
            Object::ListType args(1,ent);
            moveOp->SetArgs(args);
            if (NULL != rloc) {
                *rloc = new_loc;
            }
            return(moveOp);
        }
        Vector3D new_coords;
        if (updated_location) {
            new_coords=updated_location+(velocity*time_diff);
        } else {
            new_coords=loc.coords+(velocity*time_diff);
        }
        if (target_location) {
            Vector3D new_coords2 = new_coords+velocity/consts::basic_tick/10.0;
            double dist=target_location.distance(new_coords);
            double dist2=target_location.distance(new_coords2);
            debug_movement && cout << "dist: " << dist << "," << dist2 << endl << flush;
            if (dist2>dist) {
                debug_movement && cout << "target achieved";
                new_coords=target_location;
                reset();
                entmap["mode"] = Object("standing");
                new_loc.velocity=velocity;
            }
        }
        new_loc.coords = new_coords;
        updated_location = new_coords;
        debug_movement && cout << "new coordinates: " << new_coords << endl << flush;
        new_loc.addObject(&ent);
        Message::Object::ListType args2(1,ent);
        moveOp->SetArgs(args2);
        if (NULL != rloc) {
            *rloc = new_loc;
        }
        return(moveOp);
    }
    return(NULL);
}

double MovementInfo::get_tick_addition(const Vector3D & coordinates) const
{
    double basic_distance=velocity.mag()*consts::basic_tick;
    if (target_location) {
        double distance=coordinates.distance(target_location);
        debug_movement && cout << "basic_distance: " << basic_distance << endl << flush;
        debug_movement && cout << "distance: " << distance << endl << flush;
        if (basic_distance>distance) {
            debug_movement && cout << "\tshortened tick" << endl << flush;
            return distance/basic_distance*consts::basic_tick;
        }
    }
    return consts::basic_tick;
}

Character::Character() : movement(this), autom(0),
                         mind(NULL), external_mind(NULL), player(NULL),
                         drunkness(0.0)
{
    attributes["weight"] = Object(double(60.0));
    attributes["sex"] = Object("female");
    is_character = true;
}

Character::~Character()
{
    if (mind != NULL) {
        delete mind;
    }
    if (external_mind != NULL) {
        delete external_mind;
    }
}

extern varconf::Config * global_conf;

oplist Character::Operation(const Setup & op)
{
    debug_character && cout << "Character::tick" << endl << flush;
    oplist res;
    debug_character && cout << "CHaracter::Operation(setup)" << endl << flush;
    if (script_Operation("setup", op, res) != 0) {
        return(res);
    }
    if (op.HasAttr("sub_to")) {
        debug_character && cout << "Has sub_to" << endl << flush;
        return(res);
    }

    mind = new BaseMind(fullid, name);
    string mind_class("NPCMind"), mind_package("mind.NPCMind");
    if (global_conf->findItem("minds", type)) {
        mind_package = global_conf->getItem("minds", type);
        mind_class = type + "Mind";
    }
    Create_PyThing(mind, mind_package, mind_class);

    Setup * s = new Setup(op);
    // THis is so not the right thing to do
    s->SetAttr("sub_to", Message::Object("mind"));
    res.push_back(s);
    Look * l = new Look();
    *l = Look::Instantiate();
    l->SetTo(world->fullid);
    res.push_back(l);
    if (location.parent != world) {
        l = new Look();
        *l = Look::Instantiate();
        l->SetTo(location.parent->fullid);
        res.push_back(l);
    }
    l = new Look();
    *l = Look::Instantiate();
    l->SetTo(fullid);
    res.push_back(l);
    RootOperation * tick = new Tick;
    *tick = Tick::Instantiate();
    tick->SetTo(fullid);
    res.push_back(tick);
    return(res);
}

oplist Character::Operation(const Tick & op)
{
    oplist res;
    if (op.HasAttr("sub_to")) {
        debug_character && cout << "Has sub_to" << endl << flush;
        return(res);
    }
#ifdef DEBUG_MOVEMENT
        cout << "================================" << endl << flush;
#endif /* DEBUG_MOVEMENT */
    const Message::Object::ListType & args = op.GetArgs();
    if ((0 != args.size()) && (args.front().IsMap())) {
        Message::Object::MapType arg1 = args.front().AsMap();
        if ((arg1.find("serialno") != arg1.end()) &&
           (arg1["serialno"].IsInt())) {
            if (arg1["serialno"].AsInt() < movement.serialno) {
#ifdef DEBUG_MOVEMENT
                    cout << "Old tick" << endl << flush;
#endif /* DEBUG_MOVEMENT */
                return(res);
            }
        }
        Location ret_loc;
        RootOperation * moveOp = movement.gen_move_operation(&ret_loc);
        if (moveOp) {
            Message::Object::MapType entmap;
            entmap["name"]=Message::Object("move");
            entmap["serialno"]=Message::Object(movement.serialno);
            Message::Object ent(entmap);
            RootOperation * tickOp = new Tick();
            *tickOp = Tick::Instantiate();
            tickOp->SetTo(fullid);
            tickOp->SetFutureSeconds(movement.get_tick_addition(ret_loc.coords));
            tickOp->SetArgs(Message::Object::ListType(1,ent));
            res.push_back(tickOp);
            res.push_back(moveOp);
        }
    } else {
        // Could implement quite a lot of Animal.py here.
        script_Operation("tick", op, res);
        RootOperation * tickOp = new Tick();
        *tickOp = Tick::Instantiate();
        tickOp->SetTo(fullid);
        tickOp->SetFutureSeconds(consts::basic_tick);
        res.push_back(tickOp);
    }
    return(res);
}

oplist Character::Operation(const Talk & op)
{
    debug_character && cout << "Character::OPeration(Talk)" << endl << flush;
    Sound * s = new Sound();
    *s = Sound::Instantiate();
    Message::Object::ListType args(1,op.AsObject());
    s->SetArgs(args);
    return(oplist(1,s));
}

oplist Character::Mind_Operation(const Setup & op)
{
    Setup *s = new Setup(op);
    s->SetTo(fullid);
    s->SetAttr("sub_to", Message::Object("mind"));
    return(oplist(1,s));
}

oplist Character::Mind_Operation(const Tick & op)
{
    Tick *t = new Tick(op);
    t->SetTo(fullid);
    t->SetAttr("sub_to", Message::Object("mind"));
    return(oplist(1,t));
}

oplist Character::Mind_Operation(const Move & op)
{
    debug_movement && cout << "Character::mind_move_op" << endl << flush;
    oplist res;
    Move * newop = new Move(op);
    const Message::Object::ListType & args = op.GetArgs();
    if ((0 == args.size()) || (!args.front().IsMap())) {
        cerr << "move op has no argument" << endl << flush;
    }
    Message::Object::MapType arg1 = args.front().AsMap();
    if ((arg1.find("id") == arg1.end()) || !arg1["id"].IsString()) {
        cerr << "Its got no id" << endl << flush;
    }
    string & oname = arg1["id"].AsString();
    if (world->fobjects.find(oname) == world->fobjects.end()) {
        debug_movement && cout << "This move op is for a phoney object" << endl << flush;
        delete newop;
        return res;
    }
    Thing * obj = (Thing *)world->fobjects[oname];
    if (obj != this) {
        debug_movement && cout << "Moving something else. " << oname << endl << flush;
        try {
            double weight = attributes["weight"].AsFloat();
            Object & ow = (*obj)["weight"];
            double oweight;
            if (ow.IsInt()) {
                oweight = double(ow.AsInt());
            } else {
                oweight = ow.AsFloat();
            }
            if ((oweight < 0) || (oweight > weight)) {
                debug_movement && cout << "We can't move this. Just too heavy" << endl << flush;
                delete newop;
                return(res);
            }
        }
        catch (...) {
            cerr << "EXCEPTION: caught while checking " << obj->fullid
                 << " weight for movement of " << fullid << endl << flush;
        }
        newop->SetTo(oname);
        res.push_back(newop);
        return(res);
    }
    string location_parent;
    if ((arg1.find("loc") != arg1.end()) && (arg1["loc"].IsString())) {
        location_parent = arg1["loc"].AsString();
    } else {
        debug_movement && cout << "Parent not set" << endl << flush;
    }
    Vector3D location_coords;
    if ((arg1.find("pos") != arg1.end()) && (arg1["pos"].IsList())) {
        Message::Object::ListType vector = arg1["pos"].AsList();
        if (vector.size()==3) {
            try {
                double x = vector.front().AsFloat();
                vector.pop_front();
                double y = vector.front().AsFloat();
                vector.pop_front();
                double z = vector.front().AsFloat();
                location_coords = Vector3D(x, y, z);
                debug_movement && cout << "Got new format coords: " << location_coords << endl << flush;
            }
            catch (Message::WrongTypeException) {
                cerr << "EXCEPTION: Malformed pos move operation" << endl << flush;
            }
        }
    }

    Vector3D location_vel;
    if ((arg1.find("velocity") != arg1.end()) && (arg1["velocity"].IsList())) {
        Message::Object::ListType vector = arg1["velocity"].AsList();
        if (vector.size()==3) {
            try {
                double x = vector.front().AsFloat();
                vector.pop_front();
                double y = vector.front().AsFloat();
                vector.pop_front();
                double z = vector.front().AsFloat();
                location_vel = Vector3D(x, y, z);
                debug_movement && cout << "Got new format velocity: " << location_vel << endl << flush;
            }
            catch (Message::WrongTypeException) {
                cerr << "EXCEPTION: Malformed vel move operation" << endl << flush;
            }
        }
    }
    Vector3D location_face;
    if ((arg1.find("face") != arg1.end()) && (arg1["face"].IsList())) {
        Message::Object::ListType vector = arg1["face"].AsList();
        if (vector.size()==3) {
            try {
                double x = vector.front().AsFloat();
                vector.pop_front();
                double y = vector.front().AsFloat();
                vector.pop_front();
                double z = vector.front().AsFloat();
                location_face = Vector3D(x, y, z);
                debug_movement && cout << "Got new format face: " << location_face << endl << flush;
            }
            catch (Message::WrongTypeException) {
                cerr << "EXCEPTION: Malformed face move operation" << endl << flush;
            }
        }
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
    double vel_mag;
    // Print out a bunch of debug info
    debug_movement && cout << ":" << location_parent << ":" << world->fullid << ":" << location.parent->fullid << ":" << endl << flush;
    if ( (location_parent==world->fullid) &&
         (location_parent==location.parent->fullid) &&
         (newop->GetFutureSeconds() >= 0) ) {
        // Movement within current parent. Work out the speed and stuff and
        // use movementinfo to track movement.
        if (!location_vel) {
            debug_movement && cout << "\tVelocity default" << endl << flush;
            vel_mag=consts::base_velocity;
        } else {
            vel_mag=location_vel.mag();
            if (vel_mag > consts::base_velocity) {
                vel_mag = consts::base_velocity;
            }
            debug_movement && cout << "\tVelocity given: " << location_vel
                                   << "," << vel_mag << endl << flush;
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
                debug_movement && cout << "\tUsing face for direction" << endl << flush;
                direction=location.face;
            } else {
                debug_movement && cout << "\tUsing velocity for direction" << endl << flush;
                direction=location_vel;
            }
        } else {
            debug_movement && cout << "\tUsing destination coords for direction" << endl << flush;
            direction=location_coords-location.coords;
        }
        if (!direction) {
            debug_movement && cout << "No direction" << endl << flush;
        } else {
            direction=direction.unit_vector();
            debug_movement && cout << "Direction: " << direction << endl << flush;
        }
        if (!location_face) {
            location.face = direction;
        }
        Location ret_location;
        Location current_location;
        RootOperation * moveOp = movement.gen_move_operation(&ret_location);
        if (NULL!=moveOp) {
            current_location = ret_location;
        } else {
            current_location = location;
        }
        movement.reset();
        if ((vel_mag==0) || !direction) {
            debug_movement && cout << "\tMovement stopped" << endl << flush;
            if (NULL != moveOp) {
                Message::Object::ListType & args = moveOp->GetArgs();
                Message::Object::MapType & ent = args.front().AsMap();
                Message::Object::ListType velocity;
                velocity.push_back(Message::Object(0.0));
                velocity.push_back(Message::Object(0.0));
                velocity.push_back(Message::Object(0.0));
                ent["velocity"]=Message::Object(velocity);
                ent["mode"]=Message::Object("standing");
                moveOp->SetArgs(args);
                res.push_back(moveOp);
            } else {
                moveOp = movement.gen_face_operation(location);
                if (NULL != moveOp) {
                    res.push_back(moveOp);
                }
            }
            delete newop;
            return(res);
        }
        RootOperation * tickOp = new Tick;
        *tickOp = Tick::Instantiate();
        Message::Object::MapType ent;
        ent["serialno"] = Message::Object(movement.serialno);
        ent["name"] = Message::Object("move");
        Message::Object::ListType args(1,ent);
        tickOp->SetArgs(args);
        tickOp->SetTo(fullid);
        // Need to add the arguments to this op before we return it
        // direction is already a unit vector
        if (!location_coords) {
            debug_movement && cout << "\tNo target location" << endl << flush;
            movement.target_location = Vector3D();
        } else {
            debug_movement && cout << "\tUsing target location" << endl << flush;
            movement.target_location = location_coords;
        }
        movement.velocity=direction*vel_mag;
        debug_movement && cout << "Velocity " << vel_mag << endl << flush;
        RootOperation * moveOp2 = movement.gen_move_operation(NULL,current_location);
        tickOp->SetFutureSeconds(movement.get_tick_addition(location.coords));
        debug_movement && cout << "Next tick " << tickOp->GetFutureSeconds() << endl << flush;
        if (NULL!=moveOp2) {
            if (NULL!=moveOp) {
                delete moveOp;
            }
            moveOp=moveOp2;
        }
        // return moveOp and tickOp;
        res.push_back(moveOp);
        res.push_back(tickOp);
        delete newop;
        return(res);
    }
    res.push_back(newop);
    return(res);
}

oplist Character::Mind_Operation(const Set & op)
{
    Set * s = new Set(op);
    const Message::Object::ListType & args = op.GetArgs();
    if (args.front().IsMap()) {
        Message::Object::MapType amap = args.front().AsMap();
        if (amap.find("id") != amap.end() && amap["id"].IsString()) {
            string opid = amap["id"].AsString();
            if (opid != fullid) {
                s->SetTo(opid);
            }
        }
    }
    return(oplist(1,s));
}

oplist Character::Mind_Operation(const Create & op)
{
    // We need to call, THE THING FACTORY! Or maybe not
    // This currently does nothing, so characters are not able to directly
    // create objects. By and large a tool must be used. This should at some
    // point be able to send create operations on to other entities
    debug_character && cout << "Character::Mind_Operation(Create)" << endl << flush;
    oplist res;
    return(res);
}

oplist Character::Mind_Operation(const Delete & op)
{
    Delete * d = new Delete(op);
    return(oplist(1,d));
}

oplist Character::Mind_Operation(const Talk & op)
{
    debug_character && cout << "Character::Mind_OPeration(Talk)" << endl << flush;
    Talk * t = new Talk(op);
    return(oplist(1,t));
}

oplist Character::Mind_Operation(const Look & op)
{
    debug_character && cout << "Got look up from mind from [" << op.GetFrom()
                            << "] to [" << op.GetTo() << "]" << endl << flush;
    Look * l = new Look(op);
    if (op.GetTo().size() == 0) {
        const Message::Object::ListType & args = op.GetArgs();
        if (args.size() == 0) {
            l->SetTo(world->fullid);
        } else {
            if (args.front().IsMap()) {
                Message::Object::MapType amap = args.front().AsMap();
                if (amap.find("id") != amap.end() && amap["id"].IsString()) {
                    l->SetTo(amap["id"].AsString());
                }
            }
        }
    }
    debug_character && cout <<"    now to ["<<l->GetTo()<<"]"<<endl<<flush;
    return(oplist(1,l));
}

oplist Character::Mind_Operation(const Cut & op)
{
    Cut * c = new Cut(op);
    return(oplist(1,c));
}

oplist Character::Mind_Operation(const Eat & op)
{
    Eat * e = new Eat(op);
    return(oplist(1,e));
}

oplist Character::Mind_Operation(const Touch & op)
{
    oplist res;
    Touch * t = new Touch(op);
    // Work out what is being touched.
    const Message::Object::ListType & args = op.GetArgs();
    if ((op.GetTo().size() == 0) || (args.size() != 0)) {
        if (args.size() == 0) {
            t->SetTo(world->fullid);
        } else {
            if (args.front().IsMap()) {
                Message::Object::MapType amap = args.front().AsMap();
                if (amap.find("id") != amap.end() && amap["id"].IsString()) {
                    t->SetTo(amap["id"].AsString());
                }
            } else if (args.front().IsString()) {
                t->SetTo(args.front().AsString());
            }
        }
    }
    // Pass the modified touch operation on to target.
    res.push_back(t);
    // Set our mode to "touching"
    Set * s = new Set(Set::Instantiate());
    s->SetTo(fullid);
    Object::MapType amap;
    amap["id"] = fullid;
    amap["mode"] = "touching";
    Object::ListType setArgs(1,Object(amap));
    s->SetArgs(setArgs);
    res.push_back(s);
    return res;
}

oplist Character::W2m_Operation(const Error & op)
{
    Error * e = new Error(op);
    return(oplist(1,e));
}

oplist Character::W2m_Operation(const Setup & op)
{
    oplist res;
    if (op.HasAttr("sub_to")) {
        Setup * s = new Setup(op);
        res.push_back(s);
    }
    return(res);
}

oplist Character::W2m_Operation(const Tick & op)
{
    oplist res;
    if (op.HasAttr("sub_to")) {
        Tick * t = new Tick(op);
        res.push_back(t);
    }
    return(res);
}

oplist Character::W2m_Operation(const Sight & op)
{
    oplist res;
    if (drunkness > 1.0) {
        return(res);
    }
    Sight * s = new Sight(op);
    res.push_back(s);
    return(res);
}

oplist Character::W2m_Operation(const Sound & op)
{
    oplist res;
    if (drunkness > 1.0) {
        return(res);
    }
    Sound * s = new Sound(op);
    res.push_back(s);
    return(res);
}

oplist Character::W2m_Operation(const Touch & op)
{
    oplist res;
    if (drunkness > 1.0) {
        return(res);
    }
    Touch * t = new Touch(op);
    res.push_back(t);
    return(res);
}

oplist Character::send_mind(const RootOperation & op)
{
    debug_character && cout << "Character::send_mind" << endl << flush;
    oplist res;
    if (mind == NULL) {
        return(res);
    }
    oplist local_res;
    oplist external_res;
    local_res = mind->message(op);

    if ((NULL != external_mind) && (NULL != external_mind->connection)) {
        debug_character && cout << "Sending to external mind" << endl << flush;
        external_res = external_mind->message(op);
        // If there is some kinf of error in the connection, we turn autom on
    } else {
        //return(*(RootOperation **)NULL);
        if (autom == 0) {
            debug_character && cout << "Turning automatic on for " << fullid << endl << flush;
            autom = 1;
            if (external_mind != NULL) {
                delete external_mind;
                external_mind = NULL;
            }
        }
    }
    if (autom) {
        debug_character && cout << "Using " << local_res.size() << " ops from local mind"
             << endl << flush;
        res = local_res;
    } else {
        res = external_res;
        debug_character && cout << "Using " << local_res.size() << " ops from external mind"
             << endl << flush;
    }
    // At this point there is a bunch of conversion stuff that I don't
    // understand
    
    return(res);
}

oplist Character::mind2body(const RootOperation & op)
{
    debug_character && cout << "Character::mind2body" << endl << flush;
    RootOperation newop(op);
    oplist res;

    if ((newop.GetTo().size() == 0) &&
        (op.GetParents().front().AsString() != "look")) {
       newop.SetTo(fullid);
    }
    if (drunkness > 1.0) {
        return(res);
    }
    op_no_t otype = op_enumerate(&newop);
    OP_SWITCH(newop, otype, res, Mind_)
    // Set refno?
    // do debugging?
    //Nothing done yet, must try harder
    return(res);
}

oplist Character::world2body(const RootOperation & op)
{
    debug_character && cout << "Character::world2body" << endl << flush;
    oplist res = call_operation(op);
    // Set refno?
    // do debugging?
    //Nothing done yet, must try harder
    return(res);
}

oplist Character::world2mind(const RootOperation & op)
{
    debug_character && cout << "Character::world2mind" << endl << flush;
    op_no_t otype = op_enumerate(&op);
    oplist res;
    OP_SWITCH(op, otype, res, W2m_)
    // Set refno?
    // do debugging?
    //Nothing done yet, must try harder
    return(res);
}

oplist Character::external_message(const RootOperation & op)
{
    debug_character && cout << "Character::external_message" << endl << flush;
    return external_operation(op);
}

oplist Character::operation(const RootOperation & op)
{
    debug_character && cout << "Character::operation" << endl << flush;
    oplist result = world2body(op);
    oplist mind_res = world2mind(op);
    while (mind_res.size() != 0) {
        RootOperation * mr = mind_res.front();
        oplist mind2_res = send_mind(*mr);
        while (mind2_res.size() != 0) {
            RootOperation * mr2 = mind2_res.front();
            // Need to be very careful about what this actually does
            external_message(*mr2);
            mind2_res.pop_front();
            delete mr2;
        }
        mind_res.pop_front();
        delete mr;
    }
    return result;
}

oplist Character::external_operation(const RootOperation & op)
{
    debug_character && cout << "Character::external_operation" << endl << flush;
    oplist body_res = mind2body(op);
    
    while (body_res.size() != 0) {
        RootOperation * br = body_res.front();
        send_world(br);
        body_res.pop_front();
        // Don't delete br as it has gone into worlds queue
        // World will deal with it.
    }
    oplist res;
    return(res);
}

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

extern "C" {
    #include <stdlib.h>
}


#include "Character.h"
#include "Thing.h"

#include "BaseMind.h"
#include "ExternalMind.h"

#include <server/WorldRouter.h>

#include <modules/Location.h>

#include <common/WorldInfo.h>
#include <common/const.h>

static int debug_movement=0;

MovementInfo::MovementInfo(Character * body) : body(body)
{
    serialno=-1;
    reset();
}

void MovementInfo::reset()
{
    serialno = MovementInfo::serialno+1;
    target_location=Vector3D();
    velocity=Vector3D();
    last_movement_time=world_info::time;
}

bool MovementInfo::update_needed(const Location & location)
{
    return((velocity!=Vector3D(0,0,0))||(location.velocity!=Vector3D(0,0,0)));
}

RootOperation * MovementInfo::gen_move_operation(Location * rloc)
{
    return(gen_move_operation(rloc, body->location));
}

RootOperation * MovementInfo::gen_move_operation(Location * rloc, Location & loc)
{
    if (debug_movement) {
        cout << "gen_move_operation: status:" << endl << flush;
    }
    if (update_needed(loc)) {
        if (debug_movement) {
            cout << "gen_move_operation: Update needed...";
        }
        double current_time=world_info::time;
        double time_diff=current_time-last_movement_time;
        if (debug_movement) {
            cout << "time_diff:" << time_diff << endl << flush;
        }
        last_movement_time=current_time;
        Location new_loc=loc;
        Message::Object * ent = new Message::Object;
        new_loc.velocity=velocity;
        Move * moveOp = new Move;
        moveOp->SetTo(body->fullid);
        new_loc.addObject(ent);
        list<Message::Object> args(1,*ent);
        moveOp->SetArgs(args);
        if (!velocity) {
            if (debug_movement) {
                cout << "only velocity changed..." << endl << flush;
            }
            if (NULL != rloc) {
                *rloc = new_loc;
            }
            return moveOp;
        }
        Vector3D new_coords=loc.coords+(velocity*time_diff);
        if (!target_location) {
            Vector3D new_coords2 = new_coords+velocity/consts::basic_tick/10.0;
            double dist=target_location.distance(new_coords);
            double dist2=target_location.distance(new_coords2);
            if (debug_movement) {
                cout << "dist:" << endl << flush; //,dist,dist2;
            }
            if (dist2>dist) {
                if (debug_movement) {
                    cout << "target achieved";
                }
                new_coords=target_location;
                reset();
                new_loc.velocity=velocity;
            }
        }
        new_loc.coords=new_coords;
        if (debug_movement) {
            cout << "new coordinates:" << endl << flush; //new_loc.coordinates;
        }
        new_loc.addObject(ent);
        list<Message::Object> args2(1,*ent);
        moveOp->SetArgs(args2);
        if (NULL != rloc) {
            *rloc = new_loc;
        }
        return(moveOp);
    }
    return(NULL);
}

double MovementInfo::get_tick_addition(const Vector3D & coordinates)
{
    double basic_distance=velocity.mag()*consts::basic_tick;
    if (!(!target_location)) {
        double distance=coordinates.distance(target_location);
        if (debug_movement) {
            cout << "basic_distance:" << endl << flush; //basic_distance,;
            cout << "distance:" << endl << flush; //distance;
        }
        if (basic_distance>distance) {
            if (debug_movement) {
                cout << "\tshortened tick" << endl << flush;
            }
            return distance/basic_distance*consts::basic_tick;
        }
    }
    return consts::basic_tick;
}

Character::Character() : movement(this), sex("female"), autom(0), drunkness(0.0)
{
    weight = 60.0;
    is_character = 1;
}

RootOperation * Character::Operation(const Setup & op)
{
    cout << "CHaracter::Operation(setup)" << endl << flush;
    // HasAttr() is logically backwards
    if (!op.HasAttr("sub_to")) {
        cout << "Has sub_to" << endl << flush;
        return(NULL);
    }

    mind = new BaseMind();

    Setup * s = new Setup(op);
    // THis is so not the right thing to do
    s->SetAttr("sub_to", Message::Object("mind"));
    return(s);
    // We also need to return a look op, but don't support multiple ops yet
}

RootOperation * Character::Operation(const Tick & op)
{
    // HasAttr() is logically backwards
    if (!op.HasAttr("sub_to")) {
        cout << "Has sub_to" << endl << flush;
        return(NULL);
    }
    if (debug_movement) {
        cout << "================================" << endl << flush;
    }
    const Message::Object::ListType & args = op.GetArgs();
    if ((0 != args.size()) && (args.front().IsMap())) {
        Message::Object::MapType arg1 = args.front().AsMap();
        if ((arg1.find("serialno") != arg1.end()) &&
           (arg1["serialno"].IsInt())) {
            if (arg1["serialno"].AsInt() < movement.serialno) {
                if (debug_movement) {
                    cout << "Old tick" << endl << flush;
                }
                return(NULL);
            }
        }
        Location ret_loc;
        RootOperation * moveOp = movement.gen_move_operation(&ret_loc);
        if (moveOp) {
            Message::Object::MapType entmap;
            entmap["name"]=Message::Object("move");
            entmap["serialno"]=Message::Object(movement.serialno);
            Message::Object end(entmap);
            RootOperation * tickOp = new Tick();
            tickOp->SetTo(fullid);
            tickOp->SetFutureSeconds(movement.get_tick_addition(ret_loc.coords));
            // Arrrg we need to return both the tickOP and the moveOp!
            return(tickOp); // + moveOp
        }
        return(moveOp);
    }
    return(NULL);
}

RootOperation * Character::Operation(const Talk & op)
{
    Sound * s = new Sound();
    *s = Sound::Instantiate();
    Message::Object::ListType args(1,op.AsObject());
    s->SetArgs(args);
    return(s);
}

RootOperation * Character::Mind_Operation(const Setup & op)
{
    Setup *s = new Setup(op);
    s->SetTo(fullid);
    s->SetAttr("sub_to", Message::Object("mind"));
    return(s);
}

RootOperation * Character::Mind_Operation(const Tick & op)
{
    Tick *t = new Tick(op);
    t->SetTo(fullid);
    t->SetAttr("sub_to", Message::Object("mind"));
    return(t);
}

RootOperation * Character::Mind_Operation(const Move & op)
{
    Move * newop = new Move(op);
    const Message::Object::ListType & args = op.GetArgs();
    if ((0 == args.size()) || (!args.front().IsMap())) {
        cout << "move op has no argument" << endl << flush;
    }
    Message::Object::MapType arg1 = args.front().AsMap();
    if ((arg1.find("id") == arg1.end()) || !arg1["id"].IsString()) {
        cout << "Its got no id" << endl << flush;
    }
    string & oname = arg1["id"].AsString();
    if (world->fobjects.find(oname) == world->fobjects.end()) {
        cout << "This move op is for a phoney object" << endl << flush;
    }
    Thing * obj = (Thing *)world->fobjects[oname];
    if ((obj->weight < 0) || (obj->weight > weight)) {
        // We can't move this. Just too heavy
        return(NULL);
    }
    string location_parent("");
    if ((arg1.find("loc") != arg1.end()) && (arg1["loc"].IsString())) {
        location_parent = arg1["parent"].AsString();
    }
    Vector3D location_coords;
    if ((arg1.find("pos") != arg1.end()) && (arg1["pos"].IsList())) {
        Message::Object::ListType vector = arg1["pos"].AsList();
        if (vector.size()==3) {
            try {
                int x = vector.front().AsInt();
                vector.pop_front();
                int y = vector.front().AsInt();
                vector.pop_front();
                int z = vector.front().AsInt();
                location_coords = Vector3D(x, y, z);
            }
            catch (Message::WrongTypeException) {
            }
        }
    }

    Vector3D location_vel;
    if ((arg1.find("velocity") != arg1.end()) && (arg1["velocity"].IsList())) {
        Message::Object::ListType vector = arg1["velocity"].AsList();
        if (vector.size()==3) {
            try {
                int x = vector.front().AsInt();
                vector.pop_front();
                int y = vector.front().AsInt();
                vector.pop_front();
                int z = vector.front().AsInt();
                location_vel = Vector3D(x, y, z);
            }
            catch (Message::WrongTypeException) {
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
    if ( (location_parent==world->fullid) &&
         (location_parent==location.parent->fullid) &&
         (newop->GetFutureSeconds() >= 0) ) {
        // Movement within current parent. Work out the speed and stuff and
        // use movementinfo to track movement.
        if (!location_vel) {
            if (debug_movement) {
                cout << "\tVelocity default" << endl << flush;
            }
            vel_mag=consts::base_velocity;
        } else {
            if (debug_movement) {
                cout << "\tVelocity given" << endl << flush;
            }
            vel_mag=location_vel.mag();
            if (vel_mag > consts::base_velocity) {
                vel_mag = consts::base_velocity;
            }
        }
        Vector3D direction;
        if (!location_coords) {
            if (debug_movement) {
                cout << "\tUsing velocity for direction" << endl << flush;
            }
            direction=location_vel;
        } else {
            if (debug_movement) {
                cout << "\tDestination coordinates given" << endl << flush;
            }
            direction=location_coords-location.coords;
        }
        direction=direction.unit_vector();
        
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
            if (debug_movement) {
                cout << "\tMovement stopped" << endl << flush;
            }
            if (NULL != moveOp) {
                Message::Object::ListType & args = moveOp->GetArgs();
                Message::Object::MapType & ent = args.front().AsMap();
                Message::Object::ListType velocity;
                velocity.push_back(Message::Object(0.0));
                velocity.push_back(Message::Object(0.0));
                velocity.push_back(Message::Object(0.0));
                ent["vel"]=Message::Object(velocity);
            }
            return(moveOp);
        }
        RootOperation * tickOp = new Tick;
        Message::Object::MapType ent;
        ent["serialno"] = Message::Object(movement.serialno);
        ent["name"] = Message::Object("move");
        Message::Object::ListType args(1,ent);
        tickOp->SetArgs(args);
        tickOp->SetTo(fullid);
        // Need to add the arguments to this op before we return it
        // direction is already a unit vector
        if (!location_coords) {
            if (debug_movement) {
                cout << "\tNo target location" << endl << flush;
            }
            movement.target_location = Vector3D(0,0,0);
        } else {
            if (debug_movement) {
                cout << "\tUsing target location" << endl << flush;
            }
            movement.target_location = location_coords;
        }
        movement.velocity=direction*vel_mag;
        if (debug_movement) {
            cout << "Velocity " << vel_mag << endl << flush;
        }
        RootOperation * moveOp2 = movement.gen_move_operation(NULL,current_location);
        tickOp->SetFutureSeconds(movement.get_tick_addition(location.coords));
        if (debug_movement) {
            cout << "Next tick " << tickOp->GetFutureSeconds() << endl << flush;
        }
        if (NULL!=moveOp2) {
            moveOp=moveOp2;
        }
        // return moveOp and tickOp;
        return(moveOp);
    }
    return(newop);
}

RootOperation * Character::Mind_Operation(const Set & op)
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
    return(s);
}

RootOperation * Character::Mind_Operation(const Create & op)
{
    // We need to call, THE THING FACTORY!
    return(NULL);
}

#if 0
bad_type Character::mind_create_operation(bad_type op)
{
    if (not init.security_flag) {
        ent=op[0];
        ent.status=0.1;
        return op;
    }
}
#endif

RootOperation * Character::Mind_Operation(const Delete & op)
{
    Delete * d = new Delete(op);
    return(d);
}

RootOperation * Character::Mind_Operation(const Talk & op)
{
    Talk * t = new Talk(op);
    return(t);
}

RootOperation * Character::Mind_Operation(const Look & op)
{
    Look * l = new Look(op);
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
    return(l);
}

RootOperation * Character::Mind_Operation(const Cut & op)
{
    Cut * c = new Cut(op);
    return(c);
}

RootOperation * Character::Mind_Operation(const Eat & op)
{
    Eat * e = new Eat(op);
    return(e);
}

RootOperation * Character::Mind_Operation(const Touch & op)
{
    const Message::Object::ListType & args = op.GetArgs();
    if (args.front().IsMap()) {
        Message::Object::MapType amap = args.front().AsMap();
        if (amap.find("id") != amap.end() && amap["id"].IsString()) {
            string opid = amap["id"].AsString();
            if (opid != fullid) {
                Touch * t = new Touch(op);
                t->SetTo(opid);
                return(t);
            }
        }
    }
    return(NULL);
}

RootOperation * Character::W2m_Operation(const Setup & op)
{
    if (op.HasAttr("sub_to")) {
        Setup * s = new Setup(op);
        return(s);
    }
    return(NULL);
}

RootOperation * Character::W2m_Operation(const Tick & op)
{
    if (op.HasAttr("sub_to")) {
        Tick * t = new Tick(op);
        return(t);
    }
    return(NULL);
}

RootOperation * Character::W2m_Operation(const Sight & op)
{
    if (drunkness > 1.0) {
        return(NULL);
    }
    Sight * s = new Sight(op);
    return(s);
}

RootOperation * Character::W2m_Operation(const Sound & op)
{
    if (drunkness > 1.0) {
        return(NULL);
    }
    Sound * s = new Sound(op);
    return(s);
}

RootOperation * Character::W2m_Operation(const Touch & op)
{
    if (drunkness > 1.0) {
        return(NULL);
    }
    Touch * t = new Touch(op);
    return(t);
}

RootOperation * Character::send_mind(RootOperation & msg)
{
    cout << "Character::send_mind" << endl << flush;
    if (mind == NULL) {
        return(NULL);
    }
    RootOperation * local_res;
    RootOperation * external_res = NULL;
    RootOperation * res = NULL;
    local_res = mind->message(msg);

    if ((NULL != external_mind) && (NULL != external_mind->connection)) {
        cout << "Sending to external mind" << endl << flush;
        external_res = external_mind->message(msg);
        // If there is some kinf of error in the connection, we turn autom on
    } else {
        return(*(RootOperation **)NULL);
        if (autom == 0) {
            autom = 1;
        }
    }
    if (autom) {
        res = local_res;
    } else {
        res = external_res;
    }
    // At this point there is a bunch of conversion stuff that I don't
    // understand
    
    return(res);
}

RootOperation * Character::mind2body(const RootOperation & op)
{
    cout << "Character::mind2body" << endl << flush;
    RootOperation newop = op;

    if (newop.GetTo().size() == 0) {
       newop.SetTo(fullid);
    }
    if (drunkness > 1.0) {
        return(NULL);
    }
    op_no_t otype = op_enumerate(&newop);
    RootOperation * res = NULL;
    OP_SWITCH(op, otype, res, Mind_)
    // Set refno?
    // do debugging?
    //Nothing done yet, must try harder
    return(res);
}

RootOperation * Character::world2body(const RootOperation & op)
{
    cout << "Character::world2body" << endl << flush;
    RootOperation * res = BaseEntity::operation(op);
    // Set refno?
    // do debugging?
    //Nothing done yet, must try harder
    return(res);
}

RootOperation * Character::world2mind(const RootOperation & op)
{
    cout << "Character::world2mind" << endl << flush;
    op_no_t otype = op_enumerate(&op);
    RootOperation * res = NULL;
    OP_SWITCH(op, otype, res, W2m_)
    // Set refno?
    // do debugging?
    //Nothing done yet, must try harder
    return(res);
}

RootOperation * Character::external_message(const RootOperation & op)
{
    cout << "Character::external_message" << endl << flush;
    return external_operation(op);
}

RootOperation * Character::operation(const RootOperation & op)
{
    cout << "Character::operation" << endl << flush;
    RootOperation * result = world2body(op);
    RootOperation * mind_res = world2mind(op);
    if (mind_res != NULL) {
        mind_res = send_mind(*mind_res);
        if (mind_res != NULL) {
            // Need to be very careful about what this actually does
            external_message(*mind_res);
        }
    }
    return result;
}

RootOperation * Character::external_operation(const RootOperation & op)
{
    cout << "Character::external_operation" << endl << flush;
    send_world(mind2body(op));
    return(NULL);
}

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


#include "Character.h"
#include "Thing.h"

#include "BaseMind.h"
#include "ExternalMind.h"

#include <server/WorldRouter.h>

MovementInfo::MovementInfo() { }

#if 0
// debug_movement=0;

    MovementInfo::MovementInfo()
    {
        MovementInfo::serialno=-1;
        MovementInfo::reset();
    }

    bad_type MovementInfo::reset()
    {
        MovementInfo::serialno=MovementInfo::serialno+1;
        MovementInfo::target_location=Vector3D(0,0,0);
        MovementInfo::velocity=Vector3D(0,0,0);
        MovementInfo::last_movement_time=world_info.time.s;
    }

    bad_type MovementInfo::update_needed(bad_type location)
    {
        if (MovementInfo::velocity or location.velocity) {
            return 1;
        }
    }

    bad_type MovementInfo::gen_move_operation(bad_type body, bad_type loc=None)
    {
        if (not loc) {
            loc=body.location;
        }
        if (debug_movement) {
            print "gen_move_operation: status:",this;
        }
        if (MovementInfo::update_needed(loc)) {
            if (debug_movement) {
                print "gen_move_operation: Update needed...";
            }
            current_time=world_info.time.s;
            time_diff=current_time-MovementInfo::last_movement_time;
            if (debug_movement) {
                print "time_diff:",time_diff;
            }
            MovementInfo::last_movement_time=current_time;
            new_loc=loc.copy();
            ent=Entity(body.id,location=new_loc);
            moveOp=Operation("move",ent,to=body);
            new_loc.velocity=MovementInfo::velocity;
            if (not MovementInfo::velocity) {
                if (debug_movement) {
                    print "only velocity changed...";
                }
                return moveOp;
            }
            new_coords=loc.coordinates+MovementInfo::velocity*time_diff;
            if (MovementInfo::target_location) {
                new_coords2=new_coords+MovementInfo::velocity/const.basic_tick/10.0;
                dist=MovementInfo::target_location.distance(new_coords);
                dist2=MovementInfo::target_location.distance(new_coords2);
                if (debug_movement) {
                    print "dist:",dist,dist2;
                }
                if (dist2>dist) {
                    if (debug_movement) {
                        print "target achieved";
                    }
                    new_coords=MovementInfo::target_location;
                    MovementInfo::reset();
                    new_loc.velocity=MovementInfo::velocity;
                }
            }
            new_loc.coordinates=new_coords;
            if (debug_movement) {
                print "new coordinates:",new_loc.coordinates;
            }
            return moveOp;
        }
    }

    bad_type MovementInfo::get_tick_addition(bad_type coordinates)
    {
        basic_distance=MovementInfo::velocity.mag()*const.basic_tick;
        if (MovementInfo::target_location) {
            distance=coordinates.distance(MovementInfo::target_location);
            if (debug_movement) {
                print "basic_distance:",basic_distance,;
                print "distance:",distance;
            }
            if (basic_distance>distance) {
                if (debug_movement) {
                    print "\tshortened tick";
                }
                return distance/basic_distance*const.basic_tick;
            }
        }
        return const.basic_tick;
    }


    bad_type MovementInfo::__str__()
    {
        return "MovementInfo(%i,%s,%s,%s)" % \;
               (MovementInfo::serialno,MovementInfo::target_location,MovementInfo::velocity,;
                MovementInfo::last_movement_time);
    }
#endif


//"This is generic body class that all characters inherit from";
Character::Character() : movement(), sex("female"), drunkness(0.0), autom(0)
{
    weight = 60.0;
    is_character = 1;
    //movement=MovementInfo();
}

RootOperation * Character::Operation(const Setup & op)
{
    cout << "CHaracter::Operation(setup)" << endl << flush;
    if (!op.HasAttr("sub_to")) {
        cout << "Has sub_to" << endl << flush;
        return(NULL);
    }

    mind = new BaseMind();

    Setup * s = new Setup(op);
    // Man this is fxored up
    // THis is so not the right thing to do
    s->SetAttr("sub_to", Message::Object("mind"));
    return(s);
    // We also need to return a look op, but don't support multiple ops yet
}

#if 0
bad_type Character::setup_operation(bad_type op)
{
    log.debug(4,"Character.setup_operation:",op);
    if (hasattr(op,"sub_to")) {
        return None ; //meant for mind;
    }
    Character::mind=NPCMind(id=Character::id, body=this);
    opMindSetup=Operation("setup",to=this,sub_to=Character::mind);
    return opMindSetup + Operation("look");
}
#endif

RootOperation * Character::Operation(const Tick & op)
{
    return(NULL);
    //This is going to be a tough one
}

#if 0
bad_type Character::tick_operation(bad_type op)
{
    log.debug(4,"Character.tick_operation:",op);
    if (hasattr(op,"sub_to")) {
        return None ; //meant for mind;
    }
    time=world_info.time.s;
    if (debug_movement) {
        print "="*60;
    }
    if (len(op)) {
        arg1=op[0];
        if (arg1.serialno<Character::movement.serialno) {
            if (debug_movement) {
                print "Old tick!:",arg1.serialno;
            }
            return;
        }
        moveOp=Character::movement.gen_move_operation(this);
        if (moveOp) {
            ent=Entity("move",serialno=Character::movement.serialno);
            tickOp=Operation("tick",ent,to=this);
            tickOp.time.sadd=Character::movement.get_tick_addition(\;
                moveOp[0].location.coordinates);
            if (debug_movement) {
                print "Next tick:",tickOp.time.sadd;
            }
            return moveOp+tickOp;
        }
        return moveOp;
    }
}
#endif

RootOperation * Character::Operation(const Talk & op)
{
    Sound * s = new Sound();
    *s = Sound::Instantiate();
    Message::Object::ListType args(1,op.AsObject());
    s->SetArgs(args);
    return(s);
}

#if 0
bad_type Character::talk_operation(bad_type op)
{
    return Operation("sound",op);
}

// WHat to do with this?
bad_type Character::thought_operation(bad_type op)
{
    return Operation("telepathy",op);
}

bad_type Character::goal_info_operation(bad_type op)
{
    return Operation("telepathy",op);
}
#endif

RootOperation * Character::Mind_Operation(const Setup & op)
{
    Setup *s = new Setup(op);
    s->SetTo(fullid);
    s->SetAttr("sub_to", Message::Object("mind"));
    return(s);
}

#if 0
bad_type Character::mind_setup_operation(bad_type op)
{
    log.debug(4,"Character.mind_setup_operation:",op);
    op.to=this;
    op.sub_to=Character::mind;
    return op;
}
#endif

RootOperation * Character::Mind_Operation(const Tick & op)
{
    Tick *t = new Tick(op);
    t->SetTo(fullid);
    t->SetAttr("sub_to", Message::Object("mind"));
    return(t);
}

#if 0
bad_type Character::mind_tick_operation(bad_type op)
{
    log.debug(4,"Character.mind_tick_operation:",op);
    op.to=this;
    op.sub_to=Character::mind;
    return op;
}
#endif

RootOperation * Character::Mind_Operation(const Move & op)
{
    // A whole load of scary shit *************
    return(NULL);
}

#if 0
bad_type Character::mind_move_operation(bad_type op)
{
    ent_id=op[0].id;
    if (ent_id!=Character::id) {
        op.to=Character::world.server.id_dict[ent_id];
        if (not hasattr(op.to, "weight")) {
            return;
        }
        if (not hasattr(this, "weight")) {
            return;
        }
        if (Character::weight < op.to.weight) {
            return;
        }
        return op;
    }
    location=op[0].location;

    if (location.coordinates) {
        location.coordinates=location.coordinates+\;
                              Vector3D(random(),random(),0.0)*Character::drunkness*10.0;
    }
    else {
        if (op.time.sadd<0.0) {
            op.time.sadd=0.0;
        }
    }

    if (debug_movement) {
        print "-"*60;
        print "Current:",this,world_info.time.s,`Character::location`,;
        print "Velocity:",Character::location.velocity;
        print "Requested:",`location`,"Velocity:",location.velocity;
        if (location.coordinates) {
            print "Distance:", Character::location.coordinates.distance(location.coordinates);
        }
    }
    if (Character::location.parent==Character::world==location.parent and op.time.sadd>=0.0) {
        if (debug_movement) {
            print "Velocity handling...";
        }
        if (location.velocity) {
            if (debug_movement) {
                print "\tVelocity given";
            }
            velocityMagnitude=min(const.base_velocity,;
                                  location.velocity.mag());
        }
        else {
            if (debug_movement) {
                print "\tUsing default";
            }
            velocityMagnitude=const.base_velocity;
        }
        if (debug_movement) {
            print "Coordinates handling...";
        }
        if (location.coordinates) {
            if (debug_movement) {
                print "\tTarget coordinates given";
            }
            direction=location.coordinates-Character::location.coordinates;
        }
        else {
            if (debug_movement) {
                print "\tUsing velocity as direction";
            }
            direction=location.velocity;
        }

        moveOp=Character::movement.gen_move_operation(this);
        if (moveOp) {
            currentLocation=moveOp[0].location;
        }
        else {
            currentLocation=Character::location;
        }
        Character::movement.reset();
        if (not (velocityMagnitude and direction)) {
            if (debug_movement) {
                print "Movement stopped";
            }
            if (moveOp) {
                moveOp[0].location.velocity=Vector3D(0.0,0.0,0.0);
            }
            return moveOp;
        }

        ent=Entity("move",serialno=Character::movement.serialno);
        tickOp=Operation("tick",ent,to=this);

        direction=direction/direction.mag();

        if (location.coordinates) {
            if (debug_movement) {
                print "\tUsing target location:",location.coordinates;
            }
            Character::movement.target_location=location.coordinates;
        }
        else {
            if (debug_movement) {
                print "\tNot target location";
            }
            Character::movement.target_location=Vector3D(0,0,0);
        }
        Character::movement.velocity=direction*velocityMagnitude;
        if (debug_movement) {
            print "Velocity:",velocityMagnitude,Character::movement.velocity;
        }
        moveOp2=Character::movement.gen_move_operation(this,currentLocation);
        tickOp.time.sadd=Character::movement.get_tick_addition(Character::location.coordinates);
        if (debug_movement) {
            print "Next tick:",tickOp.time.sadd;
        }
        if (moveOp2) {
            moveOp=moveOp2;
        }
        resultOp=moveOp+tickOp;
        return resultOp;
    }

    return op;
}
#endif

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

#if 0
bad_type Character::mind_set_operation(bad_type op)
{
    ent_id=op[0].id;
    if (ent_id!=Character::id) {
        op.to=Character::world.server.id_dict[ent_id];
        return op;
    }
    return op;
}
#endif

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

#if 0
bad_type Character::mind_delete_operation(bad_type op)
{
    return op;
}

bad_type Character::mind_talk_operation(bad_type op)
{
    return op;
}

bad_type Character::mind_goal_info_operation(bad_type op)
{
    return op;
}

bad_type Character::mind_thought_operation(bad_type op)
{
    return op;
}
#endif

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

#if 0
bad_type Character::mind_look_operation(bad_type op)
{
    if (len(op)==0) {
        op.to=Character::world;
    }
    else {
        ent=op[0];
        op.to=Character::world.server.id_dict[ent.id];
    }
    return op;
}
#endif

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

#if 0
bad_type Character::mind_cut_operation(bad_type op)
{
    return op;
}

bad_type Character::mind_eat_operation(bad_type op)
{
    return op;
}
#endif

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

#if 0
bad_type Character::mind_touch_operation(bad_type op)
{
     ent_id=op[0].id;
     if (ent_id!=Character::id) {
         op.to=Character::world.server.id_dict[ent_id];
         return op;
     }
}

bad_type Character::mind_extinguish_operation(bad_type op)
{
    return op;
}

bad_type Character::mind_imaginary_operation(bad_type op)
{
    return op;
}
#endif

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

#if 0
bad_type Character::w2m_setup_operation(bad_type op)
{
    log.debug(4,"Character.w2m_setup_operation:",op);
    if (hasattr(op,"sub_to")) {
        return op ; //meant for mind;
    }
}

bad_type Character::w2m_tick_operation(bad_type op)
{
    log.debug("Character.w2m_tick_operation:",op);
    if (hasattr(op,"sub_to")) {
        return op ; //meant for mind;
    }
}
#endif

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

#if 0
bad_type Character::w2m_sight_operation(bad_type op)
{
    if (Character::drunkness>1.0) {
        return None;
    }
    return op;
}

bad_type Character::w2m_sound_operation(bad_type op)
{
    if (Character::drunkness>1.0) {
        return None;
    }
    return op;
}

bad_type Character::w2m_touch_operation(bad_type op)
{
    if (Character::drunkness>1.0) {
        return None;
    }
    return op;
}
#endif

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

#if 0
bad_type Character::send_mind(bad_type msg)
{
    if (not hasattr(this,"mind")) {
        return;
    }
    if (msg) {
        msg.internal2atlas();
    }
    if (init.security_flag) {
        local_res = None;
    }
    else {
        local_res=Character::mind.message(msg);
    }
    if (local_res) {
        local_res.internal2atlas();
    }
    external_res=None;
    try {
        em=Character::external_mind;
    }
    catch (AttributeError) {
        em=None;
    }
    if (em and em.connection) {
        try {
            external_res=em.message(msg);
        }
        catch (ConnectionError) {
            if (not Character::auto and not init.security_flag) {
                log.inform("Auto flag on for "+Character::id,msg);
                Character::auto=1;
            }
        }
    }
    else {
        if (not Character::auto and not init.security_flag) {
            log.inform("Auto flag on for "+Character::id,msg);
            Character::auto=1;
        }
    }
    if (Character::auto and not init.security_flag) {
        if (local_res==external_res==None) {
            res=None;
        }
        else {
            res=local_res+external_res;
        }
    }
    else {
        res=external_res;
    }
    conversion_result=None;
    if (msg: conversion_result=msg.atlas2internal(Character:) {
        world.server.id_dict);
    }
    if (conversion_result) {
        raise KeyError,conversion_result;
    }
    if (res: conversion_result=res.atlas2internal(Character:) {
        world.server.id_dict);
    }
    if (conversion_result) {
        raise KeyError,conversion_result;
    }
    return res;
}
#endif

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
#if 0
    switch (otype) {
        case OP_LOGIN:
            res = Mind_Operation((const Login &)op);
            break;
        case OP_CREATE:
            res = Mind_Operation((const Create &)op);
            break;
        case OP_DELETE:
            res = Mind_Operation((const Delete &)op);
            break;
        case OP_MOVE:
            res = Mind_Operation((const Move &)op);
            break;
        case OP_SET:
            res = Mind_Operation((const Set &)op);
            break;
        case OP_SIGHT:
            res = Mind_Operation((const Sight &)op);
            break;
        case OP_SOUND:
            res = Mind_Operation((const Sound &)op);
            break;
        case OP_TOUCH:
            res = Mind_Operation((const Touch &)op);
            break;
        case OP_TICK:
            res = Mind_Operation((const Tick &)op);
            break;
        case OP_LOOK:
            res = Mind_Operation((const Look &)op);
            break;
        case OP_LOAD:
            res = Mind_Operation((const Load &)op);
            break;
        case OP_SAVE:
            res = Mind_Operation((const Save &)op);
            break;
        case OP_SETUP:
            res = Mind_Operation((const Setup &)op);
            break;
        default:
            cout << "nothing doing here in character mind_" << endl;
            res = Mind_Operation(op);
            break;
    }
#endif
    // Set refno?
    // do debugging?
    //Nothing done yet, must try harder
    return(res);
}

#if 0
bad_type Character::mind2body(bad_type op)
{
    if (op.to==None) {
        op.to=this;
    }
    if (Character::drunkness>1.0) {
        return None;
    }
    operation_method=Character::find_operation(op.id,"mind_");
    res=operation_method(op);
    Character::set_refno(res,op);
    Character::debug(res,"mind2body");
    return res;
}
#endif

RootOperation * Character::world2body(const RootOperation & op)
{
    cout << "Character::world2body" << endl << flush;
    RootOperation * res = BaseEntity::operation(op);
    // Set refno?
    // do debugging?
    //Nothing done yet, must try harder
    return(res);
}

#if 0
bad_type Character::world2body(bad_type op)
{
    res=Character::call_operation(op);
    Character::set_refno(res,op);
    Character::debug(res,"world2body");
    return res;
}
#endif

RootOperation * Character::world2mind(const RootOperation & op)
{
    cout << "Character::world2mind" << endl << flush;
    op_no_t otype = op_enumerate(&op);
    RootOperation * res = NULL;
    OP_SWITCH(op, otype, res, W2m_)
#if 0
    switch (otype) {
        case OP_LOGIN:
            res = W2m_Operation((const Login &)op);
            break;
        case OP_CREATE:
            res = W2m_Operation((const Create &)op);
            break;
        case OP_DELETE:
            res = W2m_Operation((const Delete &)op);
            break;
        case OP_MOVE:
            res = W2m_Operation((const Move &)op);
            break;
        case OP_SET:
            res = W2m_Operation((const Set &)op);
            break;
        case OP_SIGHT:
            res = W2m_Operation((const Sight &)op);
            break;
        case OP_SOUND:
            res = W2m_Operation((const Sound &)op);
            break;
        case OP_TOUCH:
            res = W2m_Operation((const Touch &)op);
            break;
        case OP_TICK:
            res = W2m_Operation((const Tick &)op);
            break;
        case OP_LOOK:
            res = W2m_Operation((const Look &)op);
            break;
        case OP_LOAD:
            res = W2m_Operation((const Load &)op);
            break;
        case OP_SAVE:
            res = W2m_Operation((const Save &)op);
            break;
        case OP_SETUP:
            res = W2m_Operation((const Setup &)op);
            break;
        default:
            cout << "nothing doing here in character w2m" << endl;
            res = W2m_Operation(op);
            break;
    }
#endif
    // Set refno?
    // do debugging?
    //Nothing done yet, must try harder
    return(res);
}

#if 0
bad_type Character::world2mind(bad_type op)
{
    operation_method=Character::find_operation(op.id,"w2m_");
    res=operation_method(op);
    Character::debug(res,"world2mind");
    return res;
}
#endif

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

#if 0
bad_type Character::operation(bad_type op)
{
    result=Character::world2body(op);
    mind_result=Character::send_mind(Character::world2mind(op));
    Character::external_message(mind_result);
    return result;
}

bad_type Character::external_operation(bad_type op)
{
    Character::send_world(Character::mind2body(op));
}
#endif

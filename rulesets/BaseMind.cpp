#include <Atlas/Message/Object.h>
#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/Operation/Login.h>

#include "BaseMind.h"


BaseMind::BaseMind()
{
    //map=MemMap();
    //BaseMind::map.add_object(this);
    //BaseMind::time=WorldTime();
    //if (const.debug_level>=1) {
        //BaseMind::log_fp=open("mind_"+BaseMind::id+".log","w");
    //}
    //else {
        //BaseMind::log_fp=None;
    //}
}

#if 0
bad_type BaseMind::sight_create_operation(bad_type original_op, bad_type op)
{
    BaseMind::map.add(op[0]);
    log.debug(3,str(op[0])+"\nsight_create_operation: "+str(BaseMind::map.things));
}

bad_type BaseMind::sight_delete_operation(bad_type original_op, bad_type op)
{
    id=op[0];
    if (type(id)!=StringType) {
        id=id.id;
    }
    BaseMind::map.delete(id);
    log.debug(3,"sight_delete_operation: "+str(id));
}

bad_type BaseMind::sight_set_operation(bad_type original_op, bad_type op)
{
    BaseMind::map.update(op[0]);
    log.debug(3,"sight_set_operation: "+str(BaseMind::map.get(op[0].id)));
}

bad_type BaseMind::sight_move_operation(bad_type original_op, bad_type op)
{
    BaseMind::map.update(op[0]);
    log.debug(3,"sight_move_operation: "+str(op[0].location));
}

bad_type BaseMind::sight_undefined_operation(bad_type original_op, bad_type op)
{
    pass;
}


bad_type BaseMind::sight_operation(bad_type op)
{
    op2=op[0];
    if (op2.get_name()=="ent") {
        BaseMind::map.add(op2);
        return None;
    }
    operation_method=BaseMind::find_operation(op2.id,"sight_",;
                                         BaseMind::sight_undefined_operation);
    log.debug(3,"sight: "+str(operation_method));
    return operation_method(op,op2);
}

bad_type BaseMind::sound_undefined_operation(bad_type original_op, bad_type op)
{
    pass;
}

bad_type BaseMind::sound_operation(bad_type op)
{
    op2=op[0];
    operation_method=BaseMind::find_operation(op2.id,"sound_",;
                                         BaseMind::sound_undefined_operation);
    log.debug(3,"sound: "+str(operation_method));
    return operation_method(op,op2);
}
#endif

oplist BaseMind::Operation(const Sound & op)
{
    // Deliver argument to Sound_ things
    oplist res;
    return(res);
}

oplist BaseMind::Operation(const Sight & op)
{
    // Deliver argument to Sight_ things
    oplist(res);
    return(res);
}

RootOperation * BaseMind::get_op_name_and_sub(RootOperation & op, string & name)
{
#if 0
    event_name = op.id;
    sub_op = op;
    while (len(sub_op) and sub_op[0].get_name()=="op") {
        sub_op = sub_op[0];
        event_name = event_name + "_" + sub_op.id;
    }
    return event_name, sub_op;
#endif
    return(NULL);
}

int BaseMind::call_triggers(RootOperation & op)
{
    return(0);
}

oplist BaseMind::operation(RootOperation & op)
{
    // This might end up being quite tricky to do
    oplist res;
    return(res);
}

#if 0
bad_type BaseMind::operation(bad_type op)
{
    if (hasattr(this,"lock")) {
        return;
    }
    if (BaseMind::log_fp) {
        BaseMind::log_fp.write("receiving:\n");
        BaseMind::log_fp.write(str(op)+"\n");
    }
    reply=Message();
    res=op.atlas2internal(BaseMind::map.things);
    if (res) {
        log.debug(3,str(BaseMind::id)+" : new id: "+str(res));
        for (/*(err_op,attr,id) in res*/) {
            obj=BaseMind::map.add_id(id);
            if (type(err_op)==InstanceType) {
                setattr(err_op,attr,obj);
            }
            else {
                err_op[attr]=obj;
            }
            log.debug(3,str(obj));
        }
    }
    while (1) {
        look=BaseMind::map.look_id();
        if (not look) {
            break;
        }
        look.from_=this;
        reply.append(look);
    }
    if (hasattr(op.time,"dateTime")) {
        BaseMind::time=op.time.dateTime;
    }
    reply=reply+BaseMind::call_operation(op);
    reply=reply+BaseMind::call_triggers(op);
    op.internal2atlas();
    if (BaseMind::log_fp) {
        BaseMind::log_fp.write("sending:\n");
        BaseMind::log_fp.write(str(reply)+"\n");
    }
    return reply;
}
#endif

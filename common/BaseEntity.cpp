#include "BaseEntity.h"



#if 0
bad_type get_dict_func(bad_type dict, bad_type func_str, bad_type func_undefined)
{
    try {
        return dict[func_str];
    }
    catch (KeyError) {
        try {
            func=dict[func_str]=getattr(self,func_str);
        }
        catch (AttributeError) {
            func=dict[func_str]=func_undefined;
        }
        return func;
    }
}
#endif

//base entity for IG and OOG things
BaseEntity::BaseEntity() : id(NO_ID), stamp(0.0), deleted(0), omnipresent(0) {
    //type",[BaseEntity::get_type()]);
    //if (kw.get("location",1)==None) {
        //del kw["location"];
    //}
    //instance",[]);
    //if (BaseEntity::type==[]: BaseEntity::type=[BaseEntity:) {
        //get_type()];
    //}
    //op_dict={};
}

#if 0
bad_type BaseEntity::super()
{
    return BaseEntity::__class__.__bases__[0];
}

bad_type BaseEntity::base_init(bad_type kw)
{
    try {
        class_=BaseEntity::base;
    }
    catch (AttributeError) {
        class_=BaseEntity::__class__;
    }
    base=class_.__bases__;
    if (base) {
        BaseEntity::base=base[0];
        apply(BaseEntity::base.__init__,(self,),kw);
    }
    else {
        del BaseEntity::base;
    }
}

bad_type BaseEntity::destroy()
{
    if (BaseEntity::deleted) {
        return;
    }
    for (/*obj in BaseEntity::contains*/) {
        if (not obj.deleted) {
            obj.location.parent=BaseEntity::location.parent;
        }
    }
    if (BaseEntity::location and BaseEntity::in BaseEntity::location.parent.contains) {
        BaseEntity::location.parent.contains.remove(self);
    }
}
bad_type BaseEntity::__repr__() {
    if (hasattr(self,"type") and len(BaseEntity::type)>=1) {
        return BaseEntity::type[0]+"("+`BaseEntity::id`+")";
    }
    return BaseEntity::__class__.__name__+"('')";
}

bad_type BaseEntity::get_type()
{
    return string.lower(self.__class__.__name__);
}
#endif

Vector3D BaseEntity::get_xyz()
{
    Location l=location;
    //if (!l) {
        //return Vector3D(0.0,0.0,0.0);
    //}
    if (l.parent) {
        return l.coords+l.parent->get_xyz();
    } else {
        return l.coords;
    }
}

bad_type BaseEntity::as_entity()
{
    Entity e=Entity(BaseEntity::id);
    for (char * a;;/*a in BaseEntity::attributes*/) {
        if (a=="location") {
            Location loc=location;
            //if (loc) {
                //e.location=loc.copy();
            //}
        } else {
            //setattr(e,a,getattr(BaseEntity::a));
        }
    }
    return e;
}

bad_type BaseEntity::message(bad_type msg, bad_type op_method=None)
{
    if (None==op_method) {
        //op_method=operation;
    }
    if (! msg) {
        return None;
    }
    //if (msg.get_name()=="op") {
        //return op_method(msg);
    //}
    else {
#if 0
        bad_type res_msg=None;
        for (/*op in msg*/) {
            res=op_method(op);
            if (not res) {
                pass;
            } else if (res.get_name()=="op") {
                res.refno=op.no;
                if (! res_msg) {
                    res_msg=Message();
                }
                res_msg.append(res);
            }
            else {
                res_msg=res_msg+res;
            }
        }
        return res_msg;
#endif
    }
}

bad_type BaseEntity::external_message(bad_type msg)
{
    BaseEntity::message(msg,None/*BaseEntity::external_operation*/);
}

bad_type BaseEntity::find_operation(bad_type op_id, char * prefix="",bad_type undefined_operation=None)
{
#if 0
    if (!undefined_operation)
        undefined_operation=undefined_operation;
    }
    return get_dict_func(BaseEntity::BaseEntity::op_dict,;
                         prefix+op_id+"_operation",undefined_operation);
#endif
}

bad_type BaseEntity::setup_operation(bad_type op)
{
    return(None);
}

bad_type BaseEntity::look_operation(bad_type op)
{
    bad_type reply;
    //reply=Operation("sight",BaseEntity::as_entity(),to=op.from_);
    BaseEntity::set_refno(reply,op);
    //return reply;
}

bad_type BaseEntity::undefined_operation(bad_type op)
{
    return(None);
}

bad_type BaseEntity::call_operation(bad_type op)
{
    //operation_method=BaseEntity::find_operation(op.id);
    //return operation_method(op);
}

bad_type BaseEntity::operation(bad_type op)
{
    return call_operation(op);
}

bad_type BaseEntity::external_operation(bad_type op)
{
    return operation(op);
}

bad_type BaseEntity::apply_to_operation(method_t method, bad_type msg, BaseEntity * obj)
{
    //bad_type msg=args[0];
    if (!msg) {
        return(None);
    }
    //if (msg.get_name()=="op") {
        //apply(method,args);
    //}
    //else {
        //for (/*op in msg*/) {
            //apply(method,(op,)+args[1:]);
        //}
    //}
}

bad_type BaseEntity::set_refno_op(bad_type op, bad_type ref_op)
{
    //op.refno=ref_op.no;
    //if (op.refno<0 and op.no>0) {
        //op.no=-op.no;
    //}
}

bad_type BaseEntity::set_refno(bad_type msg, bad_type ref_msg)
{
    //apply_to_operation(BaseEntity::set_refno_op,msg,ref_msg);
}

bad_type BaseEntity::set_debug_op(bad_type op)
{
    //op.no=-op.no;
}

bad_type BaseEntity::set_debug(const char * msg)
{
    //BaseEntity::apply_to_operation(BaseEntity::set_debug_op,msg);
}

bad_type BaseEntity::debug_op(bad_type op, const char * string_message)
{
    //if (op.no<0 or op.refno<0) {
        //log.debug(1,`self`+".debug: "+string_message,op);
    //}
}

bad_type BaseEntity::debug(bad_type msg, const char * string_message)
{
    //BaseEntity::apply_to_operation(BaseEntity::debug_op,msg,string_message);
}

bad_type BaseEntity::error(bad_type op, const char * string)
{
    //err=Entity(message=string);
    //return Operation("error",err,op,refno=op.no);
}

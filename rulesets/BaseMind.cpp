// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#include <Atlas/Message/Object.h>
#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/Entity/RootEntity.h>
#include <Atlas/Objects/Operation/Login.h>
#include <Atlas/Objects/Operation/Sight.h>
#include <Atlas/Objects/Operation/Sound.h>
#include <Atlas/Objects/Operation/Combine.h>
#include <Atlas/Objects/Operation/Create.h>
#include <Atlas/Objects/Operation/Delete.h>
#include <Atlas/Objects/Operation/Divide.h>
#include <Atlas/Objects/Operation/Move.h>
#include <Atlas/Objects/Operation/Set.h>
#include <Atlas/Objects/Operation/Touch.h>
#include <Atlas/Objects/Operation/Appearance.h>
#include <Atlas/Objects/Operation/Disappearance.h>

#include <common/Chop.h>
#include <common/Cut.h>
#include <common/Eat.h>
#include <common/Fire.h>

#include <common/utility.h>
#include <common/debug.h>

#include "BaseMind.h"
#include "MemMap_methods.h"

//static const bool debug_flag = false;

int BaseMind::script_Operation(const string & op_type, const RootOperation & op,
                        oplist & ret_list, RootOperation * sub_op)
{
    if (script_object != NULL) {
        debug( cout << "Got script object for " << fullid << endl << flush;);
        string op_name = op_type+"_operation";
        // Construct apropriate python object thingies from op
        if (!PyObject_HasAttrString(script_object, (char *)(op_name.c_str()))) {
            debug( cout << "No method to be found for " << fullid
                 << "." << op_name << endl << flush;);
            return(0);
        }
        RootOperationObject * py_op = newAtlasRootOperation(NULL);
        py_op->operation = new RootOperation(op);
        py_op->own = 0;
        py_op->from = map.get_add(op.GetFrom());
        py_op->to = map.get_add(op.GetTo());
        PyObject * ret;
        if (sub_op == NULL) {
            ret = PyObject_CallMethod(script_object, (char *)(op_name.c_str()),
                                             "(O)", py_op);
        } else {
            RootOperationObject * py_sub_op = newAtlasRootOperation(NULL);
            py_sub_op->operation = sub_op;
            py_sub_op->own = 0;
            py_sub_op->from = map.get_add(sub_op->GetFrom());
            py_sub_op->to = map.get_add(sub_op->GetTo());
            ret = PyObject_CallMethod(script_object, (char *)(op_name.c_str()),
                                             "(OO)", py_op, py_sub_op);
            Py_DECREF(py_sub_op);
        }
        delete py_op->operation;
        Py_DECREF(py_op);
        if (ret != NULL) {
            debug( cout << "Called python method " << op_name
                                << " for object " << fullid << endl << flush;);
            if (PyOperation_Check(ret)) {
                RootOperationObject * op = (RootOperationObject*)ret;
                if (op->operation != NULL) {
                    ret_list.push_back(op->operation);
                    op->own = 0;
                } else {
                    debug( cout << "Method returned invalid operation"
                         << endl << flush;);
                }
            } else if (PyOplist_Check(ret)) {
                OplistObject * op = (OplistObject*)ret;
                if (op->ops != NULL) {
                    ret_list = *op->ops;
                } else {
                    debug( cout << "Method returned invalid oplist"
                         << endl << flush;);
                }
            } else {
                debug( cout << "Method returned invalid object" << endl << flush;);
            }
            
            Py_DECREF(ret);
            return(1);
        } else {
            if (PyErr_Occurred() == NULL) {
                debug( cout << "No method to be found for " << fullid << endl << flush;);
            } else {
                cerr << "Reporting python error for " << fullid << endl << flush;
                PyErr_Print();
            }
        }
    } else {
        debug( cout << "No script object asociated" << endl << flush;);
    }
    return(0);
}

BaseMind::BaseMind(string & id, string & body_name)
{
    //map=MemMap();
    fullid = id;
    name = body_name;
    map.add_object(this);
    //BaseMind::time=WorldTime();
    //if (const.debug_level>=1) {
        //BaseMind::log_fp=open("mind_"+BaseMind::id+".log","w");
    //}
    //else {
        //BaseMind::log_fp=NULL;
    //}
}

oplist BaseMind::Sight_Operation(const Sight & op, Login & sub_op)
{
    oplist res;
    if (script_Operation("sight_login", op, res, &sub_op) != 0) {
        return(res);
    }
    return(res);
}

oplist BaseMind::Sight_Operation(const Sight & op, Chop & sub_op)
{
    oplist res;
    script_Operation("sight_chop", op, res, &sub_op);
    return(res);
}

oplist BaseMind::Sight_Operation(const Sight & op, Create & sub_op)
{
    oplist res;
    if (script_Operation("sight_create", op, res, &sub_op) != 0) {
        return(res);
    }
    const Object::ListType & args = sub_op.GetArgs();
    if (args.size() == 0) {
        debug( cout << " no args!" << endl << flush;);
        return(res);
    }
    Object obj = args.front();
    Root * arg = utility::Object_asRoot(obj);
    if (arg->GetObjtype() != "op") {
        map.add(arg->AsObject());
    }
    delete arg;
    return(res);
}

oplist BaseMind::Sight_Operation(const Sight & op, Cut & sub_op)
{
    oplist res;
    script_Operation("sight_cut", op, res, &sub_op);
    return(res);
}

oplist BaseMind::Sight_Operation(const Sight & op, Delete & sub_op)
{
    debug( cout << "Sight Delete operation" << endl << flush;);
    oplist res;
    if (script_Operation("sight_delete", op, res, &sub_op) != 0) {
        return(res);
    }
    const Object::ListType & args = sub_op.GetArgs();
    if (args.size() == 0) {
        debug( cout << " no args!" << endl << flush;);
        return(res);
    }
    Object obj = args.front();
    if (obj.IsString()) {
        map._delete(obj.AsString());
    } else {
        Root * arg = utility::Object_asRoot(obj);
        if (arg->GetObjtype() != "op") {
            map._delete(arg->GetId());
        }
        delete arg;
    }
    return(res);
}

oplist BaseMind::Sight_Operation(const Sight & op, Eat & sub_op)
{
    oplist res;
    script_Operation("sight_eat", op, res, &sub_op);
    return(res);
}

oplist BaseMind::Sight_Operation(const Sight & op, Fire & sub_op)
{
    oplist res;
    script_Operation("sight_fire", op, res, &sub_op);
    return(res);
}

oplist BaseMind::Sight_Operation(const Sight & op, Move & sub_op)
{
    debug( cout << "BaseMind::Sight_Operation(Sight, Move)" << endl << flush;);
    oplist res;
    if (script_Operation("sight_move", op, res, &sub_op) != 0) {
        return(res);
    }
    const Object::ListType & args = sub_op.GetArgs();
    if (args.size() == 0) {
        debug( cout << " no args!" << endl << flush;);
        return(res);
    }
    const Object & obj = args.front();
    Root * arg = utility::Object_asRoot(obj);
    if (arg->GetObjtype() != "op") {
        map.update(obj);
    }
    delete arg;
    return(res);
}

oplist BaseMind::Sight_Operation(const Sight & op, Set & sub_op)
{
    oplist res;
    if (script_Operation("sight_set", op, res, &sub_op) != 0) {
        return(res);
    }
    const Object::ListType & args = sub_op.GetArgs();
    if (args.size() == 0) {
        debug( cout << " no args!" << endl << flush;);
        return(res);
    }
    const Object & obj = args.front();
    Root * arg = utility::Object_asRoot(obj);
    if (arg->GetObjtype() != "op") {
        map.update(obj);
    }
    delete arg;
    return(res);
}

oplist BaseMind::Sight_Operation(const Sight & op, Touch & sub_op)
{
    oplist res;
    script_Operation("sight_touch", op, res, &sub_op);
    return(res);
}

oplist BaseMind::Sight_Operation(const Sight & op, RootOperation & sub_op)
{
    debug( cout << "BaseMind::Sight_Operation(Sight, RootOperation)" << endl << flush;);
    oplist res;
    script_Operation("sight_undefined", op, res, &sub_op);
    return(res);
}

oplist BaseMind::Sound_Operation(const Sound & op, Talk & sub_op)
{
    debug( cout << "BaseMind::Sound_Operation(Sound, Talk)" << endl << flush;);
    oplist res;
    script_Operation("sound_talk", op, res, &sub_op);
    return(res);
}

oplist BaseMind::Sound_Operation(const Sound & op, RootOperation & sub_op)
{
    debug( cout << "BaseMind::Sound_Operation(Sound, RootOperation)" << endl << flush;);
    oplist res;
    script_Operation("sound_undefined", op, res, &sub_op);
    return(res);
}

#if 0
oplist BaseMind::call_sound_operation(const Sound & op, RootOperation & sub_op)
{
    map.get_add(sub_op.GetFrom());
    op_no_t op_no = op_enumerate(&sub_op);
    SUB_OP_SWITCH(op, op_no, Sound_, sub_op)
}
#endif

oplist BaseMind::Operation(const Sound & op)
{
    // Deliver argument to Sound_ things
    oplist res;
    if (script_Operation("sound", op, res) != 0) {
        return(res);
    }
    const Object::ListType & args = op.GetArgs();
    if (args.size() == 0) {
        debug( cout << " no args!" << endl << flush;);
        return(res);
    }
    Object obj = args.front();
    Root * op2 = utility::Object_asRoot(obj);
    if (op2->GetObjtype() == "op") {
        debug( cout << " args is an op!" << endl << flush;);
        res = call_sound_operation(op, *(RootOperation *)op2);
    }
    delete op2;
    return(res);
}

#if 0
oplist BaseMind::call_sight_operation(const Sight & op, RootOperation & sub_op)
{
    map.get_add(sub_op.GetFrom());
    op_no_t op_no = op_enumerate(&sub_op);
    SUB_OP_SWITCH(op, op_no, Sight_, sub_op)
}
#endif

oplist BaseMind::Operation(const Sight & op)
{
    debug( cout << "BaseMind::Operation(Sight)" << endl << flush;);
    // Deliver argument to Sight_ things
    oplist(res);
    if (script_Operation("sight", op, res) != 0) {
        debug( cout << " its in the script" << endl << flush;);
        return(res);
    }
    const Object::ListType & args = op.GetArgs();
    if (args.size() == 0) {
        debug( cout << " no args!" << endl << flush;);
        return(res);
    }
    Object obj = args.front();
    Root * op2 = utility::Object_asRoot(obj);
    if (op2->GetObjtype() == "op") {
        debug( cout << " args is an op!" << endl << flush;);
        res = call_sight_operation(op, *(RootOperation *)op2);
        //string & op2type = op2->GetParents().front().AsString();
        //string subop = "sight_" + op2type;
        //script_Operation(subop, op, res, (RootOperation *)op2);
    } else /* if (op2->GetObjtype() == "object") */ {
        debug( cout << " arg is an entity!" << endl << flush;);
        map.add(obj);
    }
    delete op2;
    return(res);
}

oplist BaseMind::Operation(const Appearance & op)
{
    oplist(res);
    script_Operation("appearance", op, res);
    Object::ListType args = op.GetArgs();
    Object::ListType::iterator I;
    for(I = args.begin(); I != args.end(); I++) {
        map.get_add(I->AsMap()["id"].AsString());
    }
    return res;
}

oplist BaseMind::Operation(const Disappearance & op)
{
    oplist(res);
    script_Operation("disappearance", op, res);
    // Not quite sure what to do to the map here, but should do something.
    return res;
}

#if 0
RootOperation * BaseMind::get_op_name_and_sub(RootOperation & op, string & name)
{
    event_name = op.id;
    sub_op = op;
    while (len(sub_op) and sub_op[0].get_name()=="op") {
        sub_op = sub_op[0];
        event_name = event_name + "_" + sub_op.id;
    }
    return event_name, sub_op;
}

int BaseMind::call_triggers(RootOperation & op)
{
    return(0);
}
#endif

//oplist BaseMind::message(const RootOperation & msg)
//{
    //return operation(msg);
//}

oplist BaseMind::operation(const RootOperation & op)
{
    // This might end up being quite tricky to do

    // In the python the following happens here:
    //   Find out if the op refers to any ids we don't know about.
    //   If so create look operations to those ids
    //   Set the minds time and date 
    oplist res;
    map.get_add(op.GetFrom());
    RootOperation * look;
    while ((look = map.look_id()) != NULL) {
        res.push_back(look);
    }
    oplist res2 = call_operation(op);
    for(oplist::const_iterator I = res2.begin(); I != res2.end(); I++) {
        res.push_back(*I);
    }
    //res = call_triggers(op);
    return(res);
}

#if 0
oplist BaseMind::operation(const RootOperation & op)
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

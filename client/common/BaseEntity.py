#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 1999 Aloril (See the file COPYING for details).

import string
from atlas import *
from common.misc import set_kw
from common import log
from world.physics.Vector3D import Vector3D

#If code has not yet been written: use this to notify user
class NoCode(Exception): pass

def get_dict_func(self, dict, func_str, func_undefined):
    """get method by name from dictionary
       if that fails, get it by looking for it by name from instance
       if it succeeds, update dictionary and return it
       if that fails too, update dictionary to point to default handler and
       return default handler
       """
    try:
        return dict[func_str]
    except KeyError:
        try:
            func=dict[func_str]=getattr(self,func_str)
        except AttributeError:
            func=dict[func_str]=func_undefined
        return func

class BaseEntity:
    "base entity for IG and OOG things"
    ########## Init and cleanup methods
    def __init__(self, **kw):
        self.attributes=[]
        set_kw(self,kw,"id","")
        set_kw(self,kw,"type",[self.get_type()])
        if kw.get("location",1)==None:
            del kw["location"]
        set_kw(self,kw,"location",None)
        set_kw(self,kw,"contains",[])
        set_kw(self,kw,"instance",[])
        set_kw(self,kw,"stamp",0.0)
        if self.type==[]: self.type=[self.get_type()]
        self.op_dict={}
        self.deleted=0
    def super(self):
        """our super class: similar to Java counterpart"""
        return self.__class__.__bases__[0]
    def base_init(self, kw):
        """call base __init__ with our actual arguments:
           usage: first call self.base_init(kw) in __init__ method
                  and then do additional initialization
           example: Thing.py
           result: all base class __init__ methods will be called from
                   most basic parent class to immediate parent class"""
        try:
            class_=self.base
        except AttributeError:
            class_=self.__class__
        base=class_.__bases__
        if base:
            self.base=base[0]
            apply(self.base.__init__,(self,),kw)
        else:
            del self.base
    def destroy(self):
        """remove all attributes (to avoid circular references)"""
        #CHEAT!: add sending of disappear operations
        if self.deleted: return
        for obj in self.contains:
            if not obj.deleted:
                obj.location.parent=self.location.parent
        if self.location and self in self.location.parent.contains:
            self.location.parent.contains.remove(self)
        self.__dict__.clear()
##         for name,value in self.__dict__.items():
##             del self.__dict__[name]
##             if hasattr(value,"destroy"):
##                 value.destroy()
    ########## Info and conversion methods
    def __repr__(self):
        """farmer('joe_123')"""
        if hasattr(self,"type") and len(self.type)>=1:
            return self.type[0]+"("+`self.id`+")"
        return self.__class__.__name__+"('')"
    def get_type(self): return string.lower(self.__class__.__name__)
##     def get_atlas_id(self, *args): 
##         """override this to handle sub ids"""
##         return self.id
    def get_xyz(self):
        """get location: if relative to parent, 
           add our location to actual parent location"""
        l=self.location
        if not l: return Vector3D(0.0,0.0,0.0)
        if l.parent:
            return l.coordinates+l.parent.get_xyz()
        else:
            return l.coordinates
    def as_entity(self):
        """convert class instance to Atlas Entity instance"""
        e=Entity(self.id)
        for a in self.attributes:
            if a=="location":
                loc=self.location
                if loc:
                    e.location=loc.copy()
            else:
                setattr(e,a,getattr(self,a))
        return e
    ########## Message and operation routing/handling
    def message(self, msg, op_method=None):
        """input: None, Operation or Message:
           call given method for all Operations
           and return combined results"""
        if not op_method: op_method=self.operation
        if not msg: return None
        if msg.get_name()=="op":
            return op_method(msg)
        else:
            res_msg=None
            for op in msg:
                res=op_method(op)
                if not res: pass
                elif res.get_name()=="op":
                    res.refno=op.no
                    if not res_msg: res_msg=Message()
                    res_msg.append(res)
                else:
                    res_msg=res_msg+res
            return res_msg
    def external_message(self, msg):
        """messages from Atlas connection"""
        self.message(msg,self.external_operation)
    def find_operation(self, op_id, prefix="",undefined_operation=None):
        """find right operation to invoke"""
        if not undefined_operation: undefined_operation=self.undefined_operation
        return get_dict_func(self,self.op_dict,
                             prefix+op_id+"_operation",undefined_operation)
    def setup_operation(self, op):
        """called once by world after object has been made"""
        pass
    def look_operation(self, op):
        """return info about us"""
        reply=Operation("sight",self.as_entity(),to=op.from_)
        self.set_refno(reply,op)
        return reply
    def undefined_operation(self, op):
        """this operation is used when no other matching operation is found"""
        pass
    def call_operation(self, op):
        operation_method=self.find_operation(op.id)
        return operation_method(op)
    def operation(self, op):
        return self.call_operation(op)
    def external_operation(self, op):
        return self.operation(op)
    ########## Methods to apply to Atlas Operations
    def apply_to_operation(self, method, *args):
        """apply method to all operations with possible extra arguments
           args[0:1]==None or Operation or Message
           args[1:]==rest arguments used unchanged"""
        msg=args[0]
        if not msg: return
        if msg.get_name()=="op":
            apply(method,args)
        else:
            for op in msg:
                apply(method,(op,)+args[1:])
    def set_refno_op(self, op, ref_op):
        op.refno=ref_op.no
        if op.refno<0 and op.no>0:
            op.no=-op.no
    def set_refno(self, msg, ref_msg):
        self.apply_to_operation(self.set_refno_op,msg,ref_msg)
    def set_debug_op(self, op):
        op.no=-op.no
    def set_debug(self, msg):
        self.apply_to_operation(self.set_debug_op,msg)
    def debug_op(self, op, string_message):
        """print debug message"""
        if op.no<0 or op.refno<0:
            log.debug(1,`self`+".debug: "+string_message,op)
    def debug(self, msg, string_message):
        self.apply_to_operation(self.debug_op,msg,string_message)
    ########## Error handling
    def error(self, op, string):
        """return Atlas error message: string is custom message string"""
        err=Entity(message=string)
        return Operation("error",err,op,refno=op.no)

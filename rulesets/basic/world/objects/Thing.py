#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 1999 Al Riddoch (See the file COPYING for details).

from atlas import *
from types import *

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

class Thing:
    def __init__(self, cppthing, **kw):
        #print "Thing.__init__"
        self.cinit(cppthing)
        self.op_dict={}
    def base_init(self, cppthing, kw):
        self.cinit(cppthing)
        try:
            class_=self.base
        except AttributeError:
            class_=self.__class__
        base=class_.__bases__
        if base:
            self.base=base[0]
            apply(self.base.__init__,(self,cppthing,),kw)
        else:
            del self.base
    def cinit(self, cppthing):
        self.__dict__['cppthing'] = cppthing
        #print "Stored reference to C++ object"
        #print cppthing
    def __getattr__(self, name):
        value = getattr(self.cppthing, name)
        #print "__getattr__",name,value
        return value
    def __setattr__(self, name, value):
        #print "__setattr__",name,value
        return setattr(self.cppthing, name, value)
    def find_op_method(self, op_id, prefix="",undefined_op_method=None):
        """find right operation to invoke"""
        if not undefined_op_method: undefined_op_method=self.undefined_op_method
        return get_dict_func(self,self.op_dict,
                             prefix+op_id+"_operation",undefined_op_method)
    def undefined_op_method(self, op):
        """this operation is used when no other matching operation is found"""
        pass
    def get_op_name_and_sub(self, op):
        event_name = op.id
        sub_op = op
        while len(sub_op) and sub_op[0].get_name()=="op":
            sub_op = sub_op[0]
            event_name = event_name + "_" + sub_op.id
        return event_name, sub_op


#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 1999 Aloril (See the file COPYING for details).

from atlas import *
from world.objects.Thing import Thing
from mind.MemMap import MemMap
from common import log,const
from server.WorldTime import WorldTime

class BaseMind(Thing):
    """base class for all NPCs"""
    ########## Initialization
    def __init__(self, **kw):
        self.base_init(kw)
        self.map=MemMap()
        self.map.add_object(self)
        self.time=WorldTime()
        if const.debug_level>=1:
            self.log_fp=open("mind_"+self.id+".log","w")
        else:
            self.log_fp=None
    ########## Sight operations
    def sight_create_operation(self, original_op, op):
        """add to our local map"""
        self.map.add(op[0])
        log.debug(3,str(op[0])+"\nsight_create_operation: "+str(self.map.things))
    def sight_delete_operation(self, original_op, op):
        """remove it from our local map"""
        id=op[0]
        if type(id)!=StringType: id=id.id
        self.map.delete(id)
        log.debug(3,"sight_delete_operation: "+str(id))
    def sight_set_operation(self, original_op, op):
        """change object in our local map"""
        self.map.update(op[0])
        log.debug(3,"sight_set_operation: "+str(self.map.get(op[0].id)))
    def sight_move_operation(self, original_op, op):
        """change position in out local map"""
        self.map.update(op[0])
        log.debug(3,"sight_move_operation: "+str(op[0].location))
    def sight_undefined_operation(self, original_op, op):
        """do nothing"""
        #CHEAT!: any way to handle these?
        pass
    def sight_operation(self, op):
        """find right method for our sighting"""
        op2=op[0]
        if op2.get_name()=="ent":
            self.map.add(op2)
            return None
        operation_method=self.find_operation(op2.id,"sight_",
                                             self.sight_undefined_operation)
        log.debug(3,"sight: "+str(operation_method))
        return operation_method(op,op2)
    ########## Sound operations
    def sound_undefined_operation(self, original_op, op):
        #CHEAT!: any way to handle these?
        pass
    def sound_operation(self, op):
        op2=op[0]
        operation_method=self.find_operation(op2.id,"sound_",
                                             self.sound_undefined_operation)
        log.debug(3,"sound: "+str(operation_method))
        return operation_method(op,op2)
    ########## Other operations
    def get_op_name_and_sub(self, op):
        event_name = op.id
        sub_op = op
        while len(sub_op) and sub_op[0].get_name()=="op":
            sub_op = sub_op[0]
            event_name = event_name + "_" + sub_op.id
        return event_name, sub_op
    def call_triggers(self, op):
        pass
    def operation(self, op):
        #for screenshots: use to 'lock' screen when certain cituation occurs
        if hasattr(self,"lock"): return
        if self.log_fp:
            self.log_fp.write("receiving:\n")
            self.log_fp.write(str(op)+"\n")
        reply=Message()
        res=op.atlas2internal(self.map.things)
        if res:
            log.debug(3,str(self.id)+" : new id: "+str(res))
            for (err_op,attr,id) in res:
                obj=self.map.add_id(id)
                if type(err_op)==InstanceType:
                    setattr(err_op,attr,obj)
                else:
                    err_op[attr]=obj
                log.debug(3,str(obj))
        while 1:
            look=self.map.look_id()
            if not look: break
            look.from_=self
            reply.append(look)
        if hasattr(op.time,"dateTime"):
            self.time=op.time.dateTime
        reply=reply+self.call_operation(op)
        reply=reply+self.call_triggers(op)
        op.internal2atlas()
        #if reply: reply.internal2atlas()
        if self.log_fp:
            self.log_fp.write("sending:\n")
            self.log_fp.write(str(reply)+"\n")
        return reply

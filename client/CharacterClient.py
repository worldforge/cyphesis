#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 1999 Aloril (See the file COPYING for details).

from atlas import *
from mind.BaseMind import BaseMind
from server import dictlist

class CharacterClient(BaseMind):
    def __init__(self, **kw):
        self.base_init(kw)
        self.event_triggers = {}
    def sight_imaginary_operation(self, op): pass
    def sight_extinguish_operation(self, op): pass
    def telepathy_operation(self, op): 
        goal_info=op[0]
        char=goal_info.from_
        char.goal=goal_info[0].description
        print char,char.goal
    def sound_talk_operation(self, original_op, op):
        talk_entity=op[0]
        #print "Talk from:",op.from_
        #print talk_entity
    def add_trigger(self, event_name, func):
        dictlist.add_value(self.event_triggers,event_name, func)
    def call_triggers(self, op):
        event_name, sub_op = self.get_op_name_and_sub(op)
        reply = Message()
        for func in self.event_triggers.get(event_name,[]):
            reply = reply + func(op, sub_op)
        return reply
    ########## Communication
    def set_from_op(self, op):
        op.from_=self
    def set_from(self, msg):
        self.apply_to_operation(self.set_from_op,msg)
    def send(self, op):
        self.set_from(op)
        #self.set_debug(op)
        op.internal2atlas()
        self.connection.send(op)
        op.atlas2internal(self.map.things)
    def find_refno_op(self, op, refno):
        if refno==op.refno:
            self.op_found=op
    def find_refno(self, msg, refno):
        self.op_found=None
        self.apply_to_operation(self.find_refno_op,msg,refno)
    def send_and_wait_reply(self, op):
        self.send(op)
        no=op.no
        if not no: return None
        while 1:
            input=self.connection.pop()
            if input:
                output=self.message(input)
                if output: self.send(output)
                self.find_refno(input,no)
                if self.op_found: return self.op_found

#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 1999 Aloril (See the file COPYING for details).

from atlas import *
from transport import ConnectionError
from common.misc import set_kw
from common import log,const

from world.objects.Character import Character

class Creator(Character):
    "This is generic body class for mapeditor, GMs and similar things"
    ########## Init and cleanup methods
    def __init__(self, **kw):
        self.base_init(kw)
        self.omnipresent=1
    ########## All kind of operations
    #-----------body operations (world2body): X_operation
    #see Thing.py for basic operations
    ########## Message routing
    def send_mind(self, msg):
        """send Message to external observer:
           1) convert it to Atlas format (object->string id)
           2) send it to external mind script/player client program 
              if connection is active
              if not active set result to None"""
        res=None
        if msg: 
            msg.internal2atlas()
            try:
                em=self.external_mind
            except AttributeError:
                em=None
            if em and em.connection:
                try:
                    res=em.message(msg)
                except ConnectionError:
                    pass
            conversion_result=msg.atlas2internal(self.world.server.id_dict)
            #should not happen: bug if happens
            if conversion_result:
                print "Potentially fatal result!"
                #raise KeyError,conversion_result
            if res: conversion_result=res.atlas2internal(self.world.server.id_dict)
            if conversion_result:
                print "Potentially fatal result 2!"
                #raise KeyError,conversion_result
        return res
    def operation(self, op):
        """operation from world:
           send everything to extern client"""
        if op.id=="look":
            return self.look_operation(op)
        if op.id=="setup":
            self.world.look_operation(Operation("look", from_=self))
        return self.send_mind(op)
    def external_operation(self, op):
        """operation from external connection:
           if it's meant to ourself: do it and send result to world,
           otherwise sent unchanged to world"""
        if op.to==self or not op.to:
            res=self.call_operation(op)
            self.set_refno(res,op)
            self.send_world(res)
        else:
            #CHEAT!
            op.from_ = "cheat" #make it appear like it came from character itself
            self.send_world(op)


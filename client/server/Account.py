#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 1999 Aloril (See the file COPYING for details).
import string
from whrandom import randint

from atlas import *
from world.physics.Vector3D import Vector3D

from OOG_Thing import OOG_Thing
from common.misc import set_kw
from common.lookup import get_thing_class
from mind.ExternalMind import ExternalMind
from common import log

class Account(OOG_Thing):
    """accounts"""
    def __init__(self, **kw):
        self.base_init(kw)
        set_kw(self,kw,"password","")
        set_kw(self,kw,"characters",[])
        self.characters_dict={}
    def logout_operation(self, op):
        """logout: disconnect player: will raise ConnectionError"""
        print "Account logout:", self
        self.connection.disconnect()
    def create_operation(self, op):
        """character creation by player"""
        ent=op[0]
        if len(ent.type)!=1:
            return self.error(op,"Type field should contain exactly one type")
        error = self.character_error(op, ent)
        if error: return error
#        type=string.capitalize(type)
        #get given character type class or use default Character class
        type_class=get_thing_class(ent.type[0],"character")
        obj=self.add_character(type_class,ent)
        log.inform("Player "+self.id+" adds character "+`obj`,op)
        return Operation("info", obj.as_entity())
    def add_character(self, character_class, ent):
        """add character to world and put it to random location
           set various attributes
           add character to various dictionaries and lists"""
        char=self.world.add_object(character_class, ent)
        if not char.location:
            char.location=Location(parent=self.world,
                                   coordinates=\
                                   Vector3D(randint(0,9),randint(0,4),0.0))
        char.player=self
        char.external_mind=ExternalMind(id=char.id, body=char,
                                        connection=self.connection)
        self.characters_dict[char.id]=char
        self.characters.append(char.id)
        self.connection.add_object(char)
        self.world.message(Operation("sight",
                                     Operation("create",char.as_entity())),
                           char)
        return char

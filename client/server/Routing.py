#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 1999 Aloril (See the file COPYING for details).

from atlas import *
from OOG_Thing import OOG_Thing

class Routing(OOG_Thing):
    """base class for all things that route atlas messges"""
    ########## Init
    def __init__(self, **kw):
        self.base_init(kw)
        self.objects={}
    ########## Operations
    def check_operation(self, op):
        """check for operation correctness"""
        #CHEAT!:check for correctness
        if op.id=="check":
            return self.error(op,"Check operation not supported")
    ########## Object handling
    def add_object(self, obj, ent=None):
        """add object to list
           if atlas Entity given:
              obj is class object
              create instance of that class
           add object to dictionary"""
        if ent:
            obj=object_from_entity(obj,ent)
        self.objects[obj.id]=obj
        return obj
    def del_object(self, obj):
        """remove object from dictionary"""
        del self.objects[obj.id]
    def get_object(self, id):
        """find object: return None if not found"""
        return self.objects.get(id)
    def find_object(self, id):
        """get object: raise KeyError if it doesn't exist"""
        return self.objects[id]

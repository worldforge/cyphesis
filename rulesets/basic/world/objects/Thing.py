#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 1999 Al Riddoch (See the file COPYING for details).

from atlas import *
from misc import set_kw

class Thing:
    def __init__(self, cppthing, **kw):
        print "Thing.__init__"
	self.cinit(cppthing)
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
        print "initialising with python Thing constructor"
        self.__dict__['cppthing'] = cppthing
        print "Stored reference to C++ object"
        print cppthing
    def __getattr__(self, name):
        print "__getattr__",name
        return getattr(self.cppthing, name)
    def __setattr__(self, name, value):
        print "__setattr__",name
        return setattr(self.cppthing, name, value)

#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 1999-2005 Al Riddoch (See the file COPYING for details).

class Thing(object):
    """This is the interface class for all entity and mind scripts. It
       provides the interface to the object in the server core."""
    def __init__(self, cppthing):
        #print "Thing.__init__"
        self.cinit(cppthing)
    def cinit(self, cppthing):
        self.__dict__['cppthing'] = cppthing
    def __getattr__(self, name):
        value = getattr(self.cppthing, name)
        #print "__getattr__",name,value
        return value
    def __setattr__(self, name, value):
        #print "__setattr__",name,value
        return setattr(self.cppthing, name, value)

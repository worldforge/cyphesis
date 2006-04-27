#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 2005 Al Riddoch (See the file COPYING for details).

from atlas import *
from physics import *
from Quaternion import Quaternion
from Vector3D import Vector3D

try:
  from random import *
except ImportError:
  from whrandom import *

from world.objects.Thing import Thing

class Logging(Thing):
    """ A proof of concept task for logging."""
    def cut_operation(self, op):
        """ Op handler for cut op which activates this task """
        print "Logging.cut"

        if len(op) < 1:
            print "No target"

        # FIXME Use weak references, once we have them
        self.target = op[0].id
        self.tool = op.to

    def tick_operation(self, op):
        """ Op handler for regular tick op """
        print "Logging.tick"
        res=Message()

        target=self.character.world.get_object(self.target)
        if not target:
            print "Target is no more"
            self.irrelevant()
            return

        if target.status > 0.5:
            set=Operation("set", Entity(self.target, status=target.status-0.1), to=self.target)
            res.append(set)
            print "CHOP",target.status
        else:
            normal=Vector3D(0,0,1)
            print "LOC.ori ", target.location.orientation
            if target.location.orientation.valid():
                normal.rotate(target.location.orientation)
            print "Normal ", normal, normal.dot(Vector3D(0,0,1))
            if normal.dot(Vector3D(0,0,1)) > 0.8:
                print "Fall down"
                chop=Operation("cut", Entity(self.target), to=self.tool)
                res.append(chop)
            elif target.status > 0.2:
                set=Operation("set", Entity(self.target, status=target.status-0.1), to=self.target)
                res.append(set)
                print "TRIM",target.status
            else:
                chop=Operation("cut", Entity(self.target), to=self.tool)
                res.append(chop)
        
        tick=Operation("tick", Entity(name="task",serialno=self.new_tick()), to=self.character.id)
        tick.setFutureSeconds(1.75)
        res.append(tick)

        return res

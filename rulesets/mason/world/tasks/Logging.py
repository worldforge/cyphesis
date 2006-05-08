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
        # print "Logging.cut"

        if len(op) < 1:
            sys.stderr.write("Logging task has no target in cut op")

        # FIXME Use weak references, once we have them
        self.target = op[0].id
        self.tool = op.to

    def tick_operation(self, op):
        """ Op handler for regular tick op """
        # print "Logging.tick"

        target=self.character.world.get_object(self.target)
        if not target:
            # print "Target is no more"
            self.irrelevant()
            return

        current_status = target.status

        if square_distance(self.character.location, target.location) > target.location.bbox.square_bounding_radius():
            self.progress = 1 - current_status
            self.rate = 0
            return self.next_tick(1.75)

        res=Message()
        if current_status > 0.5:
            set=Operation("set", Entity(self.target, status=current_status-0.1), to=self.target)
            res.append(set)
            # print "CHOP",current_status
        else:
            normal=Vector3D(0,0,1)
            # print "LOC.ori ", target.location.orientation
            if target.location.orientation.valid():
                normal.rotate(target.location.orientation)
            # print "Normal ", normal, normal.dot(Vector3D(0,0,1))
            if normal.dot(Vector3D(0,0,1)) > 0.8:
                # print "Fall down"
                chop=Operation("cut", Entity(self.target), to=self.tool)
                res.append(chop)
            elif current_status > 0.2:
                set=Operation("set", Entity(self.target, status=current_status-0.1), to=self.target)
                res.append(set)
                # print "TRIM",current_status
            else:
                chop=Operation("cut", Entity(self.target), to=self.tool)
                res.append(chop)
        self.progress = 1 - current_status
        self.rate = 0.1 / 1.75
        
        res.append(self.next_tick(1.75))

        return res

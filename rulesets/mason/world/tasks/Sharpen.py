#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 2006 Al Riddoch (See the file COPYING for details).

from atlas import *
from physics import *
from Quaternion import Quaternion
from Vector3D import Vector3D

try:
  from random import *
except ImportError:
  from whrandom import *

from world.objects.Thing import Thing

class Sharpen(Thing):
    """ A proof of concept task for sharpening a log into a stake."""
    def cut_operation(self, op):
        """ Op handler for cut op which activates this task """
        # print "Sharpen.cut"

        if len(op) < 1:
            sys.stderr.write("Sharpen task has no target in cut op")

        # FIXME Use weak references, once we have them
        self.target = op[0].id
        self.tool = op.to

    def tick_operation(self, op):
        """ Op handler for regular tick op """
        # print "Sharpen.tick"
        res=Message()

        target=self.character.world.get_object(self.target)
        if not target:
            # print "Target is no more"
            self.irrelevant()
            return

        new_status = target.status - 0.1

        if square_distance(self.character.location, target.location) > target.location.bbox.square_bounding_radius():
            self.progress = 1 - new_status
            self.rate = 0
            return self.next_tick(1.75)

        set=Operation("set", Entity(self.target, status=new_status), to=target)
        res.append(set)
        if new_status < 0:
            new_loc = target.location.copy()
            new_loc.bbox = target.location.bbox
            new_loc.orientation = target.location.orientation
            create=Operation("create", Entity(name='stake',type='stake',location=new_loc), to=target)
            res.append(create)
        self.progress = 1 - new_status
        self.rate = 0.1 / 1.75
        
        res.append(self.next_tick(1.75))

        return res

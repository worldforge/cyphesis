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

from cyphesis.Thing import Thing

class Trailblaze(Thing):
    """ A proof of concept task for making new paths and roads."""
    def strike_operation(self, op):
        """ Op handler for strike op which activates this task """
        # print "Trailblaze.strike"

        if len(op) < 1:
            sys.stderr.write("Trailblaze task has no target in strike op")

        # FIXME Use weak references, once we have them
        self.target = op[0].id
        self.tool = op.to

        self.stuff = 'stuff'
        self.things = 'things'
        self.points = []

    def tick_operation(self, op):
        """ Op handler for regular tick op """
        # print "Trailblaze.tick"
        res=Message()

        target=self.character.world.get_object(self.target)
        if not target:
            # print "Target is no more"
            self.irrelevant()
            return

        if not self.points:
            print 'Iniit'
            self.rate = 0
            self.progress = 0
            if 'world' in target.type:
                print 'World'
                new_loc = Location(target, self.character.location.coordinates)
                create = Operation("create", Entity(name='pile', type='pile', location = new_loc), to=target)
                res.append(create)
                self.points.append(self.character.location.coordinates)
            elif 'pile' in target.type:
                print 'Pile'
            else:
                print 'WTF!', target.type
                self.irrelevant()
                return
        else:
            if not self.character.location.velocity.is_valid() or \
               self.character.location.velocity.square_mag() < 1:
                if self.rate:
                    print 'terminated'
                    print self.points
                    # Finish up, and create the path
                    self.irrelevant()
                    return
                else:
                    print 'Progress'
                    self.progress = 0
                    self.rate = 1 / 1.75
            else:
                self.progress = 0
                self.rate = 0
                print 'Drawing'
                self.points.append(self.character.location.coordinates)
        res.append(self.next_tick(1.75))
        return res

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

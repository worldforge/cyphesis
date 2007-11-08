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
                    self._create_path(target, res)
                    self.irrelevant()
                    return res
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

    def _create_path(self, target, res):
        line = []
        # The left side of the path
        area = []
        # The right side of the path
        area_tail = []
        count = len(self.points)

        for i in range(count):
            point = self.points[i]
            line.append([point.x, point,y, point.z])
            if i == 0:
                # The first point on the path. Make it the start of both sides
                area.append([point.x, point,y])
                area_tail.append([point.x, point,y])
                print 'Begin'
                continue
            if i == count - 1:
                # The end point of the path. Make it the end of left side.
                area.append([point.x, point,y])
                print 'End'
                continue
            # vector from previous
            vfp = (point - self.points[i - 1]).unit_vector()
            # vector to next
            vtn = (self.points[i + 1] - point).unit_vector()

            area.append([- vfp.y - vtn.y, vfp.x + vtn.x])
            area_tail.end([vfp.y + vtn.y, - vfp.x - vtn.x])

            print 'Including this one'

        # Reverse the right side of the path
        area_tail.reverse()
        # and append it to the left side to make an area boundary
        area += area_tail()

        create = Operation('create', Entity(name='path', type='path', location=new_loc, area=area, line=line), to=target)
        res.append(create)
        return

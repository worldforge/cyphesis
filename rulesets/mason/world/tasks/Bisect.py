#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 2006 Al Riddoch (See the file COPYING for details)
#Copyright (C) 2011 Jekin Trivedi <jekintrivedi@gmail.com> (See the file COPYING for details).

from atlas import *
from physics import *
from physics import Quaternion
from physics import Vector3D

import server

class Bisect(server.Task):
    """ A task for cutting a section of material in two."""
    def bicut_operation(self, op):
        """ Op handler for cut op which activates this task """
        # print "Bisect.bicut"

        if len(op) < 1:
            sys.stderr.write("Bisect task has no target in cut op")

        # FIXME Use weak references, once we have them
        self.target = op[0].id
        self.tool = op.to


    def tick_operation(self, op):
        """ Op handler for regular tick op """
        # print "Bisect.tick"

        target=server.world.get_object(self.target)
        if not target:
            print "Target is no more"
            self.irrelevant()
            return

        if square_distance(self.character.location, target.location) > (target.location.bbox.square_bounding_radius() + 1):
            self.rate = 0
            print "Too far away"
            return self.next_tick(0.75)
        else:
            self.progress += 1

        self.rate = 0.5 / 0.75

        if self.progress < 1:
            print "Not done yet"
            return self.next_tick(0.75)

        cut_plane = 2 # z

        length = target.location.bbox.far_point[cut_plane] - target.location.bbox.near_point[cut_plane]
        mid = length/2

        res=Oplist()
        # A new BBox which changes the size of original entity
        new_bbox = [target.location.bbox.near_point.x,
                    target.location.bbox.near_point.y,
                    target.location.bbox.near_point.z,
                    target.location.bbox.far_point.x ,
                    target.location.bbox.far_point.y,
                    target.location.bbox.far_point.z]

        new_bbox[cut_plane + 3] -= mid

        print "mid ", mid

        print "mod",target.location.coordinates, new_bbox

        set=Operation("set", Entity(target.id, bbox=new_bbox), to=target)
        res.append(set)

        slice_loc = target.location.copy()

        pos_offset = Vector3D(0, 0, 0)
        pos_offset[cut_plane] = mid
        if target.location.orientation.is_valid():
            pos_offset.rotate(target.location.orientation)

        slice_loc.coordinates = target.location.coordinates + pos_offset

        slice_bbox = [target.location.bbox.near_point.x,
                      target.location.bbox.near_point.y,
                      target.location.bbox.near_point.z,
                      target.location.bbox.far_point.x,
                      target.location.bbox.far_point.y,
                      target.location.bbox.far_point.z]

        slice_bbox[cut_plane] = 0.0
        slice_bbox[cut_plane + 3] = mid

        print "new",slice_loc.coordinates, slice_bbox

        slice_loc.orientation = target.location.orientation

        typ = str(target.type[0])
        # Some entity do not have name attribute defined. If name not present assign the same name as type.
        if hasattr ( target, 'name' ) : 
            nam = str(target.name)
        else : 
            nam = typ
        
        # create a new fragment of the remaining dimensions 
        create=Operation("create", Entity(name=nam, type=typ, location=slice_loc, bbox=slice_bbox), to=target)
        res.append(create)

        self.progress = 1
        self.irrelevant()

        return res

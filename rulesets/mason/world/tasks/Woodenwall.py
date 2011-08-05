#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 2011 Jekin Trivedi <jekintrivedi@gmail.com> (See the file COPYING for details).

from atlas import *
from physics import *
from physics import Quaternion
from physics import Point3D
from physics import Vector3D

import server

class Woodenwall(server.Task):
    """A task for creating a Woodenwall by consuming 2 lumber boards and a rope"""
    
    materials = ["lumber" , "wood" ]
    def walls_operation(self, op):
        """ Op handler for walls op which activates this task """

        if len(op) < 1:
            sys.stderr.write("Woodenwall task has no target in walls op")

        # FIXME Use weak references, once we have them
        self.target = op[0].id
        self.tool = op.to

        self.pos = Point3D(op[0].pos)
        
	print self.target , self.tool , self.pos
	print "start"

    def tick_operation(self, op):

        """ Op handler for regular tick op """
        target=server.world.get_object(self.target)
        if not target:
            # print "Target is no more"
            self.irrelevant()
            return

        self.rate = 0.5 / 0.75
        self.progress += 1
	print "2"

        if not target:
            print "Target is no more"
            self.irrelevant()
            return

        res=Oplist()

        if self.progress < 1:
            # print "Not done yet"
            return self.next_tick(0.75)

        self.progress = 0
        count = 0

        while count < 2 :
            raw_lumber = []
	    print "1"

            for item in self.character.contains:
                if item.type[0] in self.materials:
		    print item.id
                    raw_lumber.append(item.id)
	    	    print count
                    self.character.contains.remove(item)
                    break
            else:
                print "No lumber in inventory"
                self.irrelevant()
                return

  	    print "2"
            chunk_loc = target.location.copy()
            chunk_loc.coordinates = target.location.coordinates
            chunk_loc.orientation = target.location.orientation
            set = Operation("set", Entity(raw_lumber.pop(), status = -1), to = target)
            res.append(set)
            count = count + 1

        create=Operation("create", Entity(name = "wall", type = "wall", location = chunk_loc), to = target)
        res.append(create)
        self.progress = 1
        self.irrelevant()
	print "end"
        return res

#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 2012 Anthony Pesce <timetopat@gmail.com> (See the file COPYING for details).

from atlas import *
from physics import *
from physics import Quaternion
from physics import Point3D
from physics import Vector3D

import server

class Pioneeringconstruction(server.Task):
    """A task for creating a Wooden structures such as A Frames with lumber and rope but for now a hammer""" 

    materials = "lumber"
    def constructions_operation(self, op):
        """ Op handler for Pioneeringconstruction op which activates this task """
        
        if len(op) < 1:
            sys.stderr.write("Pioneeringconstruction  task has no target in op")

        self.target = server.world.get_object_ref(op[0].id)
        self.tool = op.to

        self.pos = Point3D(op[0].pos)
        
    def tick_operation(self, op):

        """ Op handler for regular tick op """
        target=self.target()
        if not target:
            # print "Target is no more"
            self.irrelevant()
            return

        self.rate = 0.5 / 0.75
        self.progress += 1

        if not target:
            print "Target is no more"
            self.irrelevant()
            return

        

        if self.progress < 1:
            # print "Not done yet"
            return self.next_tick(0.75)

        self.progress = 0
        lcount = 0
        count = 0
        raw_materials = []

        # Make sure only 1 part of each attribute is being consumed as per the recipe. 
        for item in self.character.contains:
            if item.type[0] == str(self.materials):
                    raw_materials.append(item)
                    lcount = lcount + 1
            if lcount == 3 :
                break
        else:
            print "No materials in inventory"
            self.irrelevant()
            return

        chunk_loc = Location(self.character.location.parent)
        chunk_loc.velocity = Vector3D()
        chunk_loc.coordinates = self.pos
        chunk_loc.orientation=Quaternion([0,0,0,0])
        count = lcount 
        res=Oplist()
        
        #loops through raw_materials and attempts to place 3 lumber in inventory infront of user
        while (count > 0) : 
            tar = raw_materials.pop()
            move=Operation("move", Entity(tar.id,location=chunk_loc), to=self.target())
            print " ID "+tar.id+"!"
            res.append(move)
            count = count - 1
            
        
        self.progress = 1
        self.irrelevant()
        return res

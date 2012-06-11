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

        res=Oplist()

        if self.progress < 1:
            # print "Not done yet"
            return self.next_tick(0.75)

        self.progress = 0
        lcount = 0
        count = 0
        # List which stores the to be consumed entity  
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
        chunk_loc.orientation=Quaternion([1,0.5,0,1])
        count = lcount 

        # consume the materials stores in the list raw_materials
        while (count > 0) : 
            tar = raw_materials.pop()
            set = Operation("set", Entity(tar.id, status = -1), to = tar)
            res.append(set)
            count = count - 1
        #right part of a frame
        offset=Vector3D(0.5,0,0.5)
        chunk_loc.orientation=Quaternion([1,0.5,0,1])
        create1=Operation("create", Entity(name = "lumber1", type = "lumber", location = chunk_loc), to = target)      
        res.append(create1)
        #left part of a frame
        chunk_loc.orientation=Quaternion([1,-0.5,0,1])
        chunk_loc.coordinates=chunk_loc.coordinates+offset
        create2=Operation("create", Entity(name = "lumber2", type = "lumber", location = chunk_loc), to = target)      
        res.append(create2)
        #bottom part of a frame
        offset1=Vector3D(-0.4,0,0.5)
        chunk_loc.orientation=Quaternion([1,2,0,2])
        chunk_loc.coordinates=chunk_loc.coordinates+offset1
        create3=Operation("create", Entity(name = "lumber3", type = "lumber", location = chunk_loc), to = target)      
        res.append(create3)
        
        
        self.progress = 1
        self.irrelevant()
        return res

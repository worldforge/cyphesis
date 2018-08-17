#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 2011 Jekin Trivedi <jekintrivedi@gmail.com> (See the file COPYING for details).

from atlas import *
from physics import *
from physics import Quaternion
from physics import Point3D
from physics import Vector3D

import server

class Baking(server.Task):
    """A task for making various structures using skills with pickaxe"""
    
    materials = ["earth_wall" , "board_wall" ]
    def craft_operation(self, op):
        """ Op handler for crafting op which activates this task """



        if len(op) < 1:
            sys.stderr.write("Baking task has no target in crafting op")

        # FIXME Use weak references, once we have them
        self.target = server.world.get_object(op[0].id)
        self.tool = op.to

        self.pos = Point3D(op[0].pos)
        
    def tick_operation(self, op):

        """ Op handler for regular tick op """
        if self.target is None:
            # print "Target is no more"
            self.irrelevant()
            return

        self.rate = 0.5 / 0.75
        self.progress += 1

        res=Oplist()

        if self.progress < 1:
            # print "Not done yet"
            return self.next_tick(0.75)

        self.progress = 0
        # counter for Earthwall , board_wall & total count of entity.
        ecount = 0  
        bcount = 0
        count = 0
        # List which stores the to be consumed entity  
        raw_materials = []

        for item in self.character.contains:
            if item.type[0] == str(self.materials[0]):
                raw_materials.append(item)
                ecount = ecount + 1
            if item.type[0] == str(self.materials[1]):
                raw_materials.append(item)
                bcount = bcount + 1
        else:
            print(item, "Not suffcient material in inventory")

        count = ecount + bcount 

        chunk_loc = self.target().location.copy()
        chunk_loc.position = self.target().location.position
        chunk_loc.orientation = self.target().location.orientation

        # Select which structure to produce depending on the recipe present in inventory
        if ecount == 1 :
            if bcount == 1 :
                create=Operation("create", Entity(name = "castle_wall_run", type = "castle_wall_run", location = chunk_loc), to = self.target())
                res.append(create)

        if ecount == 2 :
            if bcount == 4 :
                create=Operation("create", Entity(name = "castle_wall_corner", type = "castle_wall_corner", location = chunk_loc), to = self.target())
                res.append(create)

        if ecount == 3 :
            if bcount == 2 :
                create=Operation("create", Entity(name = "castle_wall_stairs", type = "castle_wall_stairs", location = chunk_loc), to = self.target())
                res.append(create)

            if bcount == 3 :
                create=Operation("create", Entity(name = "castle_wall_gate", type = "castle_wall_gate", location = chunk_loc), to = self.target())
                res.append(create)

        if ecount == 4 :
            if bcount == 3 :
                create=Operation("create", Entity(name = "castle_house_a", type = "castle_house_a", location = chunk_loc), to = self.target())
                res.append(create)

        # Consume the materials according to the recipe
        while (count > 0) : 
            tar = raw_materials.pop()
            set = Operation("set", Entity(tar.id, status = -1), to = tar)
            res.append(set)
            count = count - 1

        self.progress = 1
        self.irrelevant()
        return res

#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 2011 Jekin Trivedi <jekintrivedi@gmail.com> (See the file COPYING for details).

from atlas import *
from physics import *
from physics import Quaternion
from physics import Point3D
from physics import Vector3D

import server

class Pioneering(server.Task):
    """A task for making various structures using skills with rope"""
    
    materials = ["rope" , "wood" , "lumber" ]
    def crafting_operation(self, op):
        """ Op handler for crafting op which activates this task """

        if len(op) < 1:
            sys.stderr.write("Pioneering task has no target in crafting op")

        # FIXME Use weak references, once we have them
        self.target = server.world.get_object_ref(op[0].id)
        self.tool = op.to

        self.pos = Point3D(op[0].pos)
        
    def tick_operation(self, op):

        """ Op handler for regular tick op """
        if self.target() is None:
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
        # counter for rope , wood , lumber & total count of entity.
        rcount = 0
        wcount = 0
        lcount = 0
        count = 0
        # List which stores the to be consumed entity  
        raw_materials = []

        for item in self.character.contains:
            if item.type[0] == str(self.materials[0]):
                raw_materials.append(item)
                rcount = rcount + 1
            if item.type[0] == str(self.materials[1]):
                raw_materials.append(item)
                wcount = wcount + 1
            if item.type[0] == str(self.materials[2]):
                raw_materials.append(item)
                lcount = lcount + 1
        else:
            print item, "Not suffcient material in inventory"

        count = rcount + wcount + lcount

        chunk_loc = self.target().location.copy()
        chunk_loc.coordinates = self.target().location.coordinates
        chunk_loc.orientation = self.target().location.orientation

        # Select which structure to produce depending on the recipe present in inventory
        if rcount == 1 :
            if wcount == 1 and lcount == 0:
                sledge_ent = Entity(name = "sledge", type = "sledge",
                                    location = chunk_loc)
                create=Operation("create", sledge_ent, to = self.target())
                res.append(create)

            if wcount == 1 and lcount == 1:
                board_wall_ent = Entity(name = "board_wall", type = "board_wall",
                                        location = chunk_loc)
                create=Operation("create", board_wall_ent, to = self.target())
                res.append(create)

            if wcount == 0 and lcount == 2:
                palissade_unnit_ent = Entity(name = "palissade_unit",
                                             type = "palissade_unit", location = chunk_loc)
                create=Operation("create", palissade_unnit_ent, to = self.target())
                res.append(create)

            #if wcount == 2 and lcount == 1:
             #   create=Operation("create", Entity(name = "wooden_gate", type = "wooden_gate", location = chunk_loc), to = self.target())
              #  res.append(create)

        if rcount == 2 :
            if wcount == 1 and lcount == 0:
                fence_section_ent = Entity(name = "fence_section", type = "fence_section",
                                           location = chunk_loc)
                create=Operation("create", fence_section_ent, to = self.target())
                res.append(create)

            if wcount == 0 and lcount == 3:
                palissade_entry_ent = Entity(name = "palissade_entry", type = "palissade_entry",
                                             location = chunk_loc)
                create=Operation("create", palissade_entry_ent, to = self.target())
                res.append(create)

        if rcount == 3 :
            if wcount == 1 and lcount == 0:
                fence_gate_ent = Entity(name = "fence_gate", type = "fence_gate", location = chunk_loc)
                create=Operation("create", fence_gate_ent, to = self.target())
                res.append(create)

#            if wcount == 3 and lcount == 0:
#                create=Operation("create", Entity(name = "draw_bridge", type = "draw_bridge", location = chunk_loc), to = self.target())
 #               res.append(create)

            if wcount == 0 and lcount == 5:
                palissade_circle_ent = Entity(name = "palissade_circle",
                                              type = "palissade_circle", location = chunk_loc)
                create=Operation("create", palissade_circle_ent, to = self.target())
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

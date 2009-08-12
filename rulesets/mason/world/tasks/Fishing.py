#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 2009 Amey Parulekar (See the file COPYING for details).

from atlas import *
from physics import *
from physics import Quaternion
from physics import Point3D
from physics import Vector3D

import server

class Fishing(server.Task):
    """A task for fishing in the ocean"""
    
    baitlist = ["annelid", "larva", "maggot"]
    def sow_operation(self, op):
        """ Op handler for sow op which activates this task """

        if len(op) < 1:
            sys.stderr.write("Fish task has no target in sow op")

        # FIXME Use weak references, once we have them
        self.target = op[0].id
        self.tool = op.to

        self.pos = Point3D(op[0].pos)
        
        target=server.world.get_object(self.target)

        #self.character.contains is a list of entities inside the player's inventory
        
        bait = 0
        
        for item in self.character.contains:
            if item.type[0] in self.baitlist:
                bait = item
                #self.character.contains.remove(item)
                break
        else:
            print "No bait in inventory"
            return

        if not target:
            print "Target is no more"
            return
        
        if "ocean" not in target.type:
            print "Can fish only in the ocean"
            return

        self.progress = 0.5

        res=Oplist()
        
        float_loc = Location(self.character.location.parent)
        #This is <server.Entity object at 0xb161b90>
        
        float_loc.coordinates = self.pos

        bait_vector = Vector3D(0, 0, -0.5)
        bait_loc = float_loc.copy()
        bait_loc.coordinates = bait_loc.coordinates + bait_vector
        print float_loc.coordinates
        print bait_loc.coordinates
        
        res = Operation("create", Entity(name = "float", parents = ["float"], location = float_loc), to = target)
        res = res + Operation("move", Entity(bait.id, location = bait_loc), to = bait)
        res = res + Operation("create", Entity(name = "hook", parents = ["hook"], location = bait_loc), to = bait)
        return res

    def tick_operation(self, op):
        """ Op handler for regular tick op """
        hook = 0
        res=Oplist()
        try:
            for item in bait.contains:
                if item.type[0] == "hook":
                    hook = item

            while hook.location.parent.id == bait.id:
                old_rate = self.rate

                self.rate = 0.1 / 17.5
                self.progress += 0.1

                if old_rate < 0.01:
                    self.progress = 0
                else:
                    self.progress += 0.01
            #a fish has eaten the bait
            self.progress = 1
            res.append(self.next_tick(1.75))
            return res
        except NameError:
            res.append(self.next_tick(1.75))
            return res

#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 2009 Amey Parulekar (See the file COPYING for details).

from atlas import *
from physics import *
from physics import Quaternion
from physics import Point3D
from physics import Vector3D

import server
import weakref

class Fishing(server.Task):
    """A task for fishing in the ocean"""
    
    baitlist = ["annelid", "larva", "maggot"]
    def sow_operation(self, op):
        """ Op handler for sow op which activates this task """

        if len(op) < 1:
            sys.stderr.write("Fish task has no target in sow op")

        # FIXME Use weak references, once we have them
        self.target = server.world.get_object_ref(op[0].id)
        self.tool = op.to

        self.pos = Point3D(op[0].pos)
        
        #self.character.contains is a list of entities inside the player's inventory
        
        bait = 0
        
        for item in self.character.contains:
            if item.type[0] in self.baitlist:
                bait = item
                #self.character.contains.remove(item)
                break
        else:
            print "No bait in inventory"
            self.irrelevant()
            return

        if "ocean" not in self.target().type:
            print "Can fish only in the ocean"
            self.irrelevant()
            return

        self.bait = weakref.ref(bait)
        self.progress = 0.5

        res=Oplist()
        
        float_loc = Location(self.character.location.parent)
        #This is <server.Entity object at 0xb161b90>
        
        float_loc.coordinates = self.pos

        bait_vector = Vector3D(0, 0, -0.5)
        bait_loc = float_loc.copy()
        bait_loc.coordinates = bait_loc.coordinates + bait_vector

        # local ents to help the next block of lines maintain a nice standard
        bobber_ent = Entity(name = "bobber", parents = ["bobber"], location = float_loc)
        bait_ent = Entity(bait.id, location = bait_loc)
        hook_ent = Entity(parents = ["hook"], location = Location(bait, Point3D(0,0,0)))

        res = Operation("create", bobber_ent, to = self.target())
        res = res + Operation("move", bait_ent, to = bait)
        res = res + Operation("create", hook_ent, to = bait)
        return res

    def tick_operation(self, op):
        """ Op handler for regular tick op """
        res=Oplist()
        hook = 0
        if not self.bait() is None:
            if not hasattr(self, 'hook'):
                for item in self.bait().contains:
                    if item.type[0] == "hook":
                        self.hook = weakref.ref(item)
            if not hasattr(self, 'hook'):
                #something has gone wrong, there is bait, but no hook inside it
                self.irrelevant()
                return
            old_rate = self.rate

            self.rate = 0.1 / 17.5
            self.progress += 0.1

            if old_rate < 0.01:
                self.progress = 0
            else:
                self.progress += 0.1
            res.append(self.next_tick(0.75))
        else:
        #a fish has eaten the bait
            if self.hook() is None:
                self.irrelevant()
                return
            fish = self.hook().location.parent
            #TODO: add check to ensure that the fish's parent isn't world or something like that
            fish_ent = Entity(fish.id, location = Location(self.character, Point3D(0,0,0)))

            res.append(Operation("move", fish_ent, to=fish))
            self.progress = 1
            self.irrelevant()
        return res

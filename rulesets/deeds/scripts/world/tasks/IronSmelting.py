# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 2011 Peter <elminister@interia.pl> (See the file COPYING for details).


from atlas import *
from physics import *
from physics import Quaternion
from physics import Point3D
from physics import Vector3D

import server


class IronSmelting(server.Task):
    """A task for making iron in bloomery"""

    def craft_operation(self, op):
        """ Op handler for crafting op which activates this task """

        if len(op) < 1:
            sys.stderr.write("IronSmelting task has no target in crafting op")

        # FIXME Use weak references, once we have them
        self.target = server.world.get_object(op[0].id)
        self.tool = op.to

    def tick_operation(self, op):

        """ Op handler for regular tick op """
        if self.target() is None:
            # print "Target is no more"
            self.irrelevant()
            return

        self.rate = 0.1 / 0.75
        self.progress += 0.1
        bloomery_location = self.target().location.copy()

        res = Oplist()

        if square_distance(self.character.location, self.target().location) > self.target().location.bbox.square_bounding_radius() + 0.1:
            self.rate = 0
            self.progress -= 0.1
            # no progress

        if self.progress < 1:
            # print "Not done yet"
            return self.next_tick(0.75)

        # Destroy bloomery, bloomeries were for single use only!
        set = Operation("set", Entity(self.target().id, status=-1), to=self.target())
        res.append(set)

        # A bit naive iron ingot can not be made without anvil & hammer but..

        # Create iron stab in place of bloomery
        print("Trying to create iron ingot")
        create = Operation("create", Entity(name="iron_ingot", type="iron_ingot"), to=self.character)
        res.append(create)

        self.progress = 1
        self.irrelevant()

        return res

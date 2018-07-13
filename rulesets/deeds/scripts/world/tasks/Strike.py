#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 2018 Erik Ogenvik (See the file COPYING for details).

from atlas import *
from physics import *
from physics import Quaternion
from physics import Vector3D

from random import *

import server

class Strike(server.Task):
    def attack_operation(self, op):

        attacker = server.world.get_object(op.from_)
        if not attacker:
            self.irrelevant()
            print("No attacker")
            return

        #First get the tool used, and make sure it's a wielded
        tool = server.world.get_object(op.to)
        if not tool:
            self.irrelevant()
            print("No tool")
            return

        target = server.world.get_object(op[0].id)

        if not target:
            self.irrelevant()
            print("No target")
            return

        if not target.is_reachable_for_other_entity(attacker):
            self.irrelevant()
            print("Too far away")
            return

        print("Striking!")
        #TODO: add logic for striking


    def tick_operation(self, op):
        print("Tick!")
        self.irrelevant()
        pass

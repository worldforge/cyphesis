# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 2020 Erik Ogenvik (See the file COPYING for details).
import math
import random

import physics
import server
from atlas import Operation, Entity

from world.traits import Spawner


def createCharacter(spawn_entity, entity_def):
    pass


class CharacterCreator(server.Thing):

    def spawn_operation(self, op):

        arg = op[0]
        if not arg:
            return server.OPERATION_BLOCKED

        spawn_prop = self.props.get_prop_map("spawn")
        if spawn_prop:
            properties = spawn_prop["properties"]
            for entry in properties:
                name = entry["name"]
                providedValue = arg[name]
                # Check if it's one of the options
                if "options" in entry:
                    optionsList = entry["options"]

        ent = Entity()

        pos = Spawner.get_spawn_pos(self)
        if pos:
            create_op = Operation("create", arg)

            # Randomize orientation
            rotation = random.random() * math.pi * 2
            orientation = physics.Quaternion(physics.Vector3D(0, 1, 0), rotation)
            entity_map = {"pos": pos,
                          "orientation": orientation.as_list(),
                          "parent": "settler",
                          "__account": arg["__account"],
                          "loc": self.location.parent.id}

            return server.OPERATION_BLOCKED, Operation("create", entity_map, to=self.id)

        return server.OPERATION_BLOCKED

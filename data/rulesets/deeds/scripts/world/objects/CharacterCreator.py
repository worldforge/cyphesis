# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 2020 Erik Ogenvik (See the file COPYING for details).
import math
import random

import physics
import server
from atlas import Operation, Entity

from world.traits import Spawner


class CharacterCreator(server.Thing):

    def create_operation(self, op):

        arg = op[0]
        if not arg:
            print("No argument in Create op.")
            return server.OPERATION_BLOCKED

        spawn_prop = self.get_prop_map("__spawn")
        if spawn_prop:
            ent = Entity()
            properties = spawn_prop["properties"]
            for entry in properties:
                name = entry["name"]
                provided_value = arg[name]
                if provided_value is None:
                    print("Lacking required value '{}'.".format(name))
                    # If the required value isn't present in the ones provided, abort
                    return server.OPERATION_BLOCKED
                # Check if it's one of the options
                if "options" in entry:
                    options_list = entry["options"]
                    if provided_value in options_list:
                        ent[name] = provided_value
                    else:
                        print("Supplied value {} = {} does not exist in allowed values in '__spawn' property. Someone trying to hack?".format(name, provided_value))
                else:
                    ent[name] = provided_value

            spawn_properties = self.get_prop_map("__spawn_properties")
            if spawn_properties is not None:
                # There's a "__spawn_properties" property declared; use its properties directly
                for key, value in spawn_properties.items():
                    ent[key] = value

            # Make sure that we prepend a script for player control for the new entity.
            if "__scripts!prepend" in ent:
                ent["__scripts!prepend"] = ent["__scripts!prepend"] + [{"language": "python", "name": "world.traits.PlayerControlled.PlayerControlled"}]
            else:
                ent["__scripts!prepend"] = [{"language": "python", "name": "world.traits.PlayerControlled.PlayerControlled"}]

            # Any entity created as a character should have it's "mind" property disabled; i.e. we don't want AI to control this character.
            ent["mind"] = None

            pos = Spawner.get_spawn_pos(self)
            if pos:
                # Randomize orientation
                rotation = random.random() * math.pi * 2
                orientation = physics.Quaternion(physics.Vector3D(0, 1, 0), rotation)
                ent["loc"] = self.parent.id
                ent["pos"] = pos
                ent["orientation"] = orientation.as_list()
                ent["__account"] = arg["__account"]

                print("creating new character")
                return server.OPERATION_BLOCKED, Operation("create", ent, to="0")
        else:
            print("CharacterCreator script attached to entity without '__spawn' property.")
            return server.OPERATION_BLOCKED

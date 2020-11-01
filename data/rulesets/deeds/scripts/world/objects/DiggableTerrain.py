# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 2019 Erik Ogenvik (See the file COPYING for details).

import random

import server
from atlas import Operation, Entity
from rules import Location


class DiggableTerrain(server.Thing):
    """
    Applied on terrain which can be dug.
    """

    materials = {'sand': 'sand', 'grass': 'earth'}

    def dig_operation(self, op):

        arg = op[0]
        if not arg:
            return server.OPERATION_IGNORED

        if not arg.pos:
            print('No pos supplied')
            return server.OPERATION_IGNORED

        terrain_prop = self.props.terrain
        if not terrain_prop:
            print('No terrain prop on diggable terrain entity')
            actor = server.world.get_entity(op.from_)
            return server.OPERATION_BLOCKED, actor and actor.client_error(op, "Cannot dig here.")

        surface = self.props.terrain.get_surface_name(arg.pos[0], arg.pos[2])
        if surface not in DiggableTerrain.materials:
            actor = server.world.get_entity(op.from_)
            print("The surface couldn't be dug here. Material {}.".format(surface))
            return server.OPERATION_BLOCKED, actor and actor.client_error(op, "The surface couldn't be dug here.")

        material = DiggableTerrain.materials[surface]

        chunk_loc = Location(self, arg.pos)

        print("Creating pile of {} at {}".format(material, chunk_loc))
        new_entity = Entity(name="Pile of {}".format(material),
                            parent="pile",
                            material=material,
                            location=chunk_loc)
        if material == 'earth':
            new_entity._worms = random.randint(0, 3)
        create_op = Operation("create", new_entity, to=self.id)

        return server.OPERATION_BLOCKED, create_op

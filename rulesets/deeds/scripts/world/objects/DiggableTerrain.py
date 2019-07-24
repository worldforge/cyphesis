# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 2006 Al Riddoch (See the file COPYING for details).

from atlas import Operation, Entity, Oplist
from rules import Location
import random
import server


class DiggableTerrain(server.Thing):
    """
    Applied on terrain which can be digged.
    """

    materials = {1: 'sand', 2: 'earth'}

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
            return server.OPERATION_IGNORED

        surface = self.props.terrain.get_surface(arg.pos[0], arg.pos[2])
        if surface not in DiggableTerrain.materials:
            print("The surface couldn't be digged here.")
            return server.OPERATION_IGNORED

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

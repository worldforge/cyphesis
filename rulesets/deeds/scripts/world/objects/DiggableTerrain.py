# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 2006 Al Riddoch (See the file COPYING for details).

from atlas import Operation, Entity, Oplist
from rules import Location

import server


class DiggableTerrain(server.Thing):
    """
    Applied on terrain which can be digged.
    """

    materials = {1: 'sand', 2: 'earth', 3: 'silt'}

    def dig_operation(self, op):

        arg = op[0]
        if not arg:
            return

        if not arg.pos:
            print('No pos supplied')
            return

        surface = self.props.terrain.get_surface(arg.pos[0], arg.pos[2])
        if surface not in DiggableTerrain.materials:
            print("The surface couldn't be digged here.")
            return
        material = DiggableTerrain.materials[surface]

        chunk_loc = Location(self, arg.pos)

        print("Creating pile of {} at {}".format(material, chunk_loc))
        create_op = Operation("create",
                              Entity(name="Pile of {}".format(material),
                                     parent="pile",
                                     material=material,
                                     location=chunk_loc), to=self.id)

        return create_op

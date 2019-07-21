# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 2006 Al Riddoch (See the file COPYING for details).

from atlas import Operation, Entity, Oplist
from physics import Quaternion
from physics import Point3D
from physics import Vector3D
from rules import Location

import server


class DiggableTerrain(server.Thing):
    """
    Applied on terrain which can be digged.
    """

    materials = {1: 'sand', 2: 'earth', 3: 'silt'}

    def dig_operation(self, op):

        surface = self.target().props.terrain.get_surface(self.pos)
        if surface not in Dig.materials:
            print("The surface couldn't be digged here.")
            return

        arg = op[0]
        if not arg:
            return


        chunk_loc = Location(self.location.parent)
        chunk_loc.velocity = Vector3D()

        chunk_loc.pos = arg.pos

        create_op = Operation("create",
                           Entity(name=Dig.materials[surface],
                                  type="pile",
                                  material=Dig.materials[surface],
                                  location=chunk_loc), to=self.id)

        return create_op

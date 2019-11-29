# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 2006 Al Riddoch (See the file COPYING for details).

import server
from atlas import Operation, Entity, Oplist
from rules import Location


class ExcavatableTerrain(server.Thing):
    """
    Applied on terrain which can be excavated.
    """

    materials = {'rock': 'granite_a', 'snow': 'ice'}

    def excavate_operation(self, op):

        res = Oplist()
        arg = op[0]
        if not arg:
            return server.OPERATION_IGNORED

        if not arg.pos:
            print('No pos supplied')
            return server.OPERATION_IGNORED

        terrain_prop = self.props.terrain
        if not terrain_prop:
            print('No terrain prop on excavatable terrain entity')
            return server.OPERATION_IGNORED

        chunk_loc = Location(self, arg.pos)

        surface = terrain_prop.get_surface_name(arg.pos[0], arg.pos[2])
        if surface not in ExcavatableTerrain.materials:
            print("Not rock")
            return
        material = ExcavatableTerrain.materials[surface]

        mods = terrain_prop.find_mods(arg.pos[0], arg.pos[2])
        if len(mods) == 0:
            # There is no terrain mod where we are digging,
            # create a quarry

            modmap = {
                'heightoffset': -1,
                'shape': {
                    'points': [[-1.0, -1.0],
                               [-1.0, 1.0],
                               [1.0, 1.0],
                               [1.0, -1.0]],
                    'type': 'polygon'
                },
                'type': 'levelmod'
            }
            quarry_create = Operation("create",
                                      Entity(name="quarry",
                                             parent="path",
                                             location=chunk_loc,
                                             terrainmod=modmap),
                                      to=self)
            res.append(quarry_create)
        else:
            print(mods)
            for mod in mods:
                if not hasattr(mod, 'name') or mod.name != 'quarry':
                    print("%s is no good" % mod.id)
                    continue
                print("%s looks good" % mod.id)
                print(mod.terrainmod)
                mod.terrainmod.heightoffset -= 1.0
                # We have modified the attribute in place, so must send an update op to propagate
                res.append(Operation("update", to=mod.id))
                break

        create = Operation("create",
                           Entity(name=material,
                                  parent=material,
                                  location=chunk_loc), to=self)
        res.append(create)

        return res

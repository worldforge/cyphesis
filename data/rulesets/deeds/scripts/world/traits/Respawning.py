import math
import random

import physics
import server
from atlas import Operation, Entity, Oplist
from rules import Location

# Respawns whenever it's destroyed.
# This relies on the property "_respawning" being set to an aliased entity.
from world.traits import Spawner


class Respawning(server.Thing):

    def delete_operation(self, op):
        res = Oplist()

        # Restore status if it's zero.
        if self.has_prop_float("status"):
            res += Operation("set", Entity(self.id, status=1.0), to=self.id)

        # Respawn in a spawn area
        respawn_alias = self.get_prop_string("_respawning")
        if respawn_alias:
            respawn_entity = server.get_alias_entity(respawn_alias)
            if respawn_entity:
                pos = Spawner.get_spawn_pos(respawn_entity)
                if pos:
                    location = Location()
                    location.pos = pos
                    # Randomize orientation
                    rotation = random.random() * math.pi * 2
                    location.orientation = physics.Quaternion(physics.Vector3D(0, 1, 0), rotation)
                    location.parent = respawn_entity.parent
                    # Emit a sight of this entity being defeated
                    res += Operation("sight", Operation("defeated", Entity(self.id)))
                    res += Operation("move", Entity(self.id, location=location), to=self.id)
                    res += Operation("imaginary", Entity(description="You were killed and will be respawned."), to=self.id, from_=self.id)
                    return server.OPERATION_BLOCKED, res

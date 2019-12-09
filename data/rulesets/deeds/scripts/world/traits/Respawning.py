import server
from atlas import Operation, Entity, Oplist
from rules import Location


# Respawns whenever it's destroyed.
# This relies on the property "_respawning" being set to a spawn area.
class Respawning(server.Thing):

    def delete_operation(self, op):
        res = Oplist()

        # Restore status if it's zero.
        if self.has_prop_float("status"):
            res += Operation("set", Entity(self.id, status=1.0), to=self.id)

        # Respawn in a spawn area
        location = Location()
        server.move_to_spawn(self.props["_respawning"], location)
        res += Operation("move", Entity(self.id, location=location), to=self.id)
        res += Operation("imaginary", Entity(description="You were killed and will be respawned."), to=self.id, from_=self.id)
        return server.OPERATION_BLOCKED, res

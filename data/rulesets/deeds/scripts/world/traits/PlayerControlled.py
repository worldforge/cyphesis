import server
from atlas import Operation, Entity, Oplist
from rules import Location


# A player controlled entity should be moved to limbo when the user stops controlling it.
# When it's killed it's also moved to limbo and stays there for a while.
# This relies on the property "_respawning" being set to a spawn area.
class PlayerControlled(server.Thing):

    def __init__(self, cpp):
        self.tick_refno = 0

    def respawn(self):
        limbo_entity = server.get_limbo_location()
        # If we're in limbo we should respawn
        if limbo_entity and self.location.parent == limbo_entity:
            respawn_loc = self.props["__respawn"]
            if respawn_loc:
                set_op = Operation("set", Entity(self.id, __respawn=None), to=self.id)
                if hasattr(respawn_loc, "pos") and hasattr(respawn_loc, "loc"):
                    return Operation("move", Entity(self.id, pos=respawn_loc.pos, loc=respawn_loc.loc),
                                     to=self.id), set_op
                elif hasattr(respawn_loc, "spawn"):
                    # Respawn in a spawn area
                    location = Location()
                    server.move_to_spawn(respawn_loc.spawn, location)
                    return Operation("move", Entity(self.id, location=location), to=self.id), set_op

    def _minds_property_update(self):
        limbo_entity = server.get_limbo_location()
        if len(self.props._minds) == 0:
            if self.location.parent != limbo_entity:
                self.tick_refno = self.tick_refno + 1
                # No minds anymore, delay movement to limbo with some time
                return Operation("tick", Entity(name=self.__class__.__name__, type="remove"), refno=self.tick_refno,
                                 future_seconds=30, to=self.id)
        else:
            return self.respawn()

    def tick_operation(self, op):
        if len(op) > 0:
            arg = op[0]

            if arg.name == self.__class__.__name__:
                res = Oplist()
                if op.refno == self.tick_refno:
                    if hasattr(arg, "type ") and arg.type == "remove":
                        # Move entity to limbo
                        limbo_entity = server.get_limbo_location()
                        if limbo_entity and self.location.parent != limbo_entity:
                            # Store the current position in "__respawn" so we can spawn back there.
                            res += Operation("set", Entity(self.id, __respawn={"loc": self.location.parent.id,
                                                                               "pos": self.location.pos}), to=self.id)
                            res += Operation("move", Entity(self.id, loc=limbo_entity.id), to=self.id)
                    else:
                        # Only respawn if there's a mind
                        if self.props._minds and len(self.props._minds) != 0:
                            res += self.respawn()

                return server.OPERATION_BLOCKED, res

    def delete_operation(self, op):
        limbo_entity = server.get_limbo_location()
        if limbo_entity and self.location.parent != limbo_entity:
            # Move to limbo, wait a couple of seconds, and then move back to respawn place
            set_op = Operation("set", Entity(self.id, __respawn={"spawn": self.props["_respawning"]}, status=1),
                               to=self.id)
            self.tick_refno = self.tick_refno + 1
            tick_op = Operation("tick", Entity(name=self.__class__.__name__, type="respawn"), refno=self.tick_refno,
                                future_seconds=5, to=self.id)
            imaginary_op = Operation("imaginary", Entity(
                description="You were killed. You need to wait 30 seconds before you will be returned to the world."),
                                     to=self.id, from_=self.id)
            return server.OPERATION_BLOCKED, imaginary_op, Operation("move", Entity(self.id, loc=limbo_entity.id),
                                                                     to=self.id), set_op, tick_op

        return server.OPERATION_BLOCKED

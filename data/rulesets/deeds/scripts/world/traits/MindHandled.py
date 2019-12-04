import server
from atlas import Operation, Entity


# A player controlled entity should be moved to limbo when the user stops controlling it.
class IdleInLimbo(server.Thing):

    def __init__(self, cpp):
        self.tick_refno = 0

    def _minds_property_update(self):
        if len(self.props._minds) == 0:
            self.tick_refno = self.tick_refno + 1
            # No minds anymore, delay movement to limbo with some time
            return Operation("tick", Entity(name=self.__class__.__name__), refno=self.tick_refno,
                             future_seconds=30, to=self.id)
        else:
            # If we're in limbo we should respawn
            limbo_entity = server.get_limbo_location()
            if limbo_entity and self.location.parent == limbo_entity:
                respawn_loc = self.props["__respawn"]
                if respawn_loc and respawn_loc.pos and respawn_loc.loc:
                    set_op = Operation("set", Entity(self.id, __respawn=None), to=self.id)
                    return Operation("move", Entity(self.id, pos=respawn_loc.pos, loc=respawn_loc.loc), to=self.id), set_op

    def tick_operation(self, op):
        if len(op) > 0:
            arg = op[0]

            if arg.name == self.__class__.__name__:
                if op.refno != self.tick_refno:
                    return server.OPERATION_BLOCKED
                # Move entity to limbo
                limbo_entity = server.get_limbo_location()
                if limbo_entity and self.location.parent != limbo_entity:
                    # Store the current position in "__respawn" so we can spawn back there.
                    set_op = Operation("set", Entity(self.id, __respawn={"loc": self.location.parent.id, "pos": self.location.pos}), to=self.id)
                    return server.OPERATION_BLOCKED, Operation("move", Entity(self.id, loc=limbo_entity.id), to=self.id), set_op

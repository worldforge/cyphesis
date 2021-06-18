import server
from atlas import Operation, Entity, Oplist


# A player controlled entity should be moved to limbo when the user stops controlling it.
# When it's killed it's also moved to limbo and stays there for a while.
# This relies on the property "_respawning" being set to a spawn area.
class PlayerControlled(server.Thing):

    def __init__(self, cpp):
        self.tick_refno = 0

    def respawn(self):
        limbo_entity = server.get_alias_entity("limbo")
        # If we're in limbo we should respawn
        if limbo_entity and self.parent == limbo_entity:
            respawn_prop = self.props["__respawn"]
            if respawn_prop:
                set_op = Operation("set", Entity(self.id, __respawn=None), to=self.id)
                if hasattr(respawn_prop, "pos") and hasattr(respawn_prop, "loc"):
                    return Operation("move", Entity(self.id, pos=respawn_prop.pos, loc=respawn_prop.loc),
                                     to=self.id), set_op
                elif hasattr(respawn_prop, "spawn") and respawn_prop.spawn:
                    # Respawn in a spawn area
                    respawn_entity = server.get_alias_entity(respawn_prop.spawn)
                    if respawn_entity:
                        location = respawn_entity.location
                        return Operation("move", Entity(self.id, location=location), to=self.id), set_op
                    else:
                        print("Could not get any entity with alias '{}'.".format(respawn_prop.spawn))

    def _minds_property_update(self):
        if len(self.props._minds) == 0:
            limbo_entity = server.get_alias_entity("limbo")
            if self.parent != limbo_entity:
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
                # Handle the world being recreated by checking for 0
                if op.refno == self.tick_refno or self.tick_refno == 0:
                    minds_prop = self.get_prop_list("_minds")
                    if hasattr(arg, "type") and arg.type == "remove":
                        # Check that the entity hasn't gotten a new mind in the meantime
                        if minds_prop is None or len(minds_prop) == 0:
                            # Move entity to limbo
                            limbo_entity = server.get_alias_entity("limbo")
                            if limbo_entity and self.parent != limbo_entity:
                                # Store the current position in "__respawn" so we can spawn back there.
                                res += Operation("set", Entity(self.id, __respawn={"loc": self.parent.id,
                                                                                   "pos": self.location.pos}), to=self.id)
                                res += Operation("move", Entity(self.id, loc=limbo_entity.id), to=self.id)
                    else:
                        # Only respawn if there's a mind
                        if minds_prop is not None and len(minds_prop) > 0:
                            res += self.respawn()

                return server.OPERATION_BLOCKED, res

    def delete_operation(self, op):
        res = Oplist()
        limbo_entity = server.get_alias_entity("limbo")
        if not limbo_entity:
            print("Entity is set to respawn, but there's no limbo entity set in the system.")
        if self.parent != limbo_entity:
            # Emit a sight of this entity being defeated
            res += Operation("sight", Operation("defeated", Entity(self.id)))
            res += Operation("imaginary", Entity(
                description="You were killed. You need to wait 30 seconds before you will be returned to the world."),
                             to=self.id, from_=self.id)
            res += Operation("move", Entity(self.id, loc=limbo_entity.id),
                             to=self.id)

            respawning_prop = self.get_prop_string("_respawning")
            # Move to limbo, wait a couple of seconds, and then move back to respawn place
            if not respawning_prop:
                print("Entity is set to respawn, but there's no _respawning property set. Will move to limbo.")
            else:
                res += Operation("set", Entity(self.id, __respawn={"spawn": self.props["_respawning"]}, status=1),
                                 to=self.id)
            self.tick_refno = self.tick_refno + 1
            res += Operation("tick", Entity(name=self.__class__.__name__, type="respawn"), refno=self.tick_refno,
                             future_seconds=30, to=self.id)
            return server.OPERATION_BLOCKED, res

        return server.OPERATION_BLOCKED

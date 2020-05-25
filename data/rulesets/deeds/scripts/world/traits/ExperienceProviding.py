import server
from atlas import Operation, Entity


class ExperienceProviding(server.Thing):
    """Attached to entities that will gift "xp" to their attackers when killed."""

    def delete_operation(self, op):
        aggressor = self.get_prop_string("__aggressor")
        xp_provided = self.get_prop_float("xp_provided")
        if aggressor and xp_provided:
            return server.OPERATION_IGNORED, \
                   Operation("set", Entity(aggressor, {"xp!append": xp_provided}), to=aggressor, from_=aggressor), \
                   Operation("imaginary", Entity(description="You gain {} xp.".format(xp_provided)), to=aggressor, from_=aggressor)

        return server.OPERATION_IGNORED

    def hit_operation(self, op):
        arg = op[0]
        if arg:
            actor_id = arg.id
            if actor_id:
                # TODO: We need to allow for multiple aggressors (with "assists" perhaps), as well as timeouts.
                return server.OPERATION_IGNORED, Operation("set", Entity(self.id, {"__aggressor": actor_id}), to=self.id)

        return server.OPERATION_IGNORED

import server
from atlas import Operation, Entity


# Flammable entities can burn. When they receive an "consume" op of type "fire" they will lose some status
# (if they have a status prop) and nourish the flame.
class Flammable(server.Thing):

    def consume_operation(self, op):
        if len(op) > 0:
            arg = op[0]
            if arg.consume_type == "fire":
                # Determine the burn speed, i.e. how much to remove of the status for each burn
                burn_speed = 0.1
                burn_speed_prop = self.props._burn_speed
                if burn_speed_prop:
                    burn_speed = burn_speed_prop

                nourish_ent = Entity()

                nourish_ent.consume_type = arg.consume_type
                nourish_op = Operation("nourish", nourish_ent, to=op.from_)

                status_prop = self.props.status

                if status_prop:
                    return server.OPERATION_BLOCKED, nourish_op, \
                           Operation("set", Entity(self.id, {"status!subtract": burn_speed}), to=self)
                else:
                    return server.OPERATION_BLOCKED, nourish_op

        return server.OPERATION_IGNORED

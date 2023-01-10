import server
from atlas import Operation, Entity


# Used by items that should embed themselves into whatever they hit.
class Embeddable(server.Thing):

    def hit_operation(self, op):
        mode_prop = self.props.mode
        if mode_prop is not None and mode_prop == "projectile":
            # It should now be planted on the thing it hit
            if len(op.args):
                first_arg = op.args[0]
                # print("embedded in {}".format(first_arg.id))
                mode_data = {"mode": "planted", "$eid": first_arg.id}
                return server.OPERATION_HANDLED, \
                    Operation("move", Entity(self.id, mode="planted", mode_data=mode_data), to=self.id)

        return server.OPERATION_IGNORED

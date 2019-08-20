from atlas import Operation, Entity, Oplist
import server


# Used by items that should embed themselves into whatever they hit.
class Embeddable(server.Thing):

    def hit_operation(self, op):
        mode_prop = self.props.mode
        if mode_prop is not None and mode_prop == "projectile":
            # It should now be planted on the thing it hit
            if len(op.args):
                firstArg = op.args[0]
                print("embedded in {}".format(firstArg.id))
                return server.OPERATION_HANDLED, Operation("move", Entity(self.id, mode="planted", planted_on=firstArg.id), to=self.id)

        return server.OPERATION_IGNORED

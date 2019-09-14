from atlas import Operation, Entity, Oplist
import server
import rules


# Used by items that should explode themselves when hit.
class Explodable(server.Thing):

    def hit_operation(self, op):
        res = Oplist()
        arg = op[0]
        if arg:
            # Place the explosion at the point of collision.
            new_location = rules.Location(self.location.parent, arg.pos)
            res.append(Operation("create", Entity(parent="explosion", location=new_location, mode="fixed"), to=self.id))
            res.append(Operation("delete", Entity(self.id), to=self.id))
        return server.OPERATION_HANDLED, res

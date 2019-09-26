from atlas import Operation, Entity, Oplist
import server


# When attached to an entity this allows for creation of arbitrary entities.
# Be very careful with attaching this trait.
class Creator(server.Thing):

    def create_operation(self, op):
        res = Oplist()
        self.create_new_entity(op, res)
        return server.OPERATION_HANDLED, res

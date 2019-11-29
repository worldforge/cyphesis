import server
from atlas import Operation, Entity


# When status is set to <= 0 the entity is deleted
class StatusDeletable(server.Thing):

    def update_operation(self, op):
        if self.has_prop_float('status') and self.get_prop_float('status') <= 0:
            return server.OPERATION_HANDLED, Operation("delete", Entity(self.id), to=self)

        return server.OPERATION_HANDLED

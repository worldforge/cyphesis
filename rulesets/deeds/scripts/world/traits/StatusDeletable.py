from atlas import Operation, Entity, Oplist
import server


# When status is set to <= 0 the entity is deleted
class StatusDeletable(server.Thing):

    def update_operation(self, op):
        if self.props.status and self.props.status <= 0:
            return server.OPERATION_HANDLED, Operation("delete", Entity(self.id), to=self)

        return server.OPERATION_HANDLED

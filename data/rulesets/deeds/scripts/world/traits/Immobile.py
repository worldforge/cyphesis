import server

from atlas import Operation


class Immobile(server.Thing):
    """
    Applied on entities that can't be moved
    """

    def move_operation(self, op):
        # Is the move operation sent because the parent has been deleted? If so we should delete ourselves.
        if self.parent.is_destroyed:
            return server.OPERATION_BLOCKED, Operation("delete", to=self)
        else:
            return server.OPERATION_BLOCKED

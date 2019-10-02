import server


class Immobile(server.Thing):
    """
    Applied on entities that can't be moved
    """

    def move_operation(self, op):
        return server.OPERATION_BLOCKED

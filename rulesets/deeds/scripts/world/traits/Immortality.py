import server


# If the "immortal" property is set to be "true", we can't be deleted.
class Immortality(server.Thing):

    def delete_operation(self, op):
        if self.props.immortal and self.props.immortal == 1:
            return server.OPERATION_BLOCKED

        return server.OPERATION_IGNORED

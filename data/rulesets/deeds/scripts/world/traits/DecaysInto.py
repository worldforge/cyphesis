from atlas import Operation, Entity, Oplist
import server


# When the entity is deleted it's replaced by one or many other entities.
class DecaysInto(server.Thing):

    def delete_operation(self, op):
        decays_prop = self.props["__decays"]
        if not decays_prop:
            print("DecaysInto placed on entity without '__decays' prop")
            return server.OPERATION_IGNORED
        op_list = Oplist()
        # Only spawn if there's a parent
        if self.location.parent:
            for decay in decays_prop:
                op_list += Operation("create", Entity(parent=decay,
                                                      location=self.location.copy()), to=self.location.parent)
        return server.OPERATION_IGNORED, op_list

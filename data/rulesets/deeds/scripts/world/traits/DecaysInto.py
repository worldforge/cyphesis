import server
from atlas import Operation, Entity, Oplist
from rules import extract_location


# When the entity is deleted it's replaced by one or many other entities.
class DecaysInto(server.Thing):

    def delete_operation(self, op):
        decays_prop = self.props["__decays"]
        if not decays_prop:
            print("DecaysInto placed on entity without '__decays' prop")
            return server.OPERATION_IGNORED
        op_list = Oplist()
        # Only spawn if there's a parent
        if self.parent:
            for decay in decays_prop:
                op_list += Operation("create", extract_location(self, Entity(parent=decay)), to=self.parent)
        return server.OPERATION_IGNORED, op_list

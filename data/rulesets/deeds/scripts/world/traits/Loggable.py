import server
from atlas import Operation, Entity


# Turn the entity into lumber when it's destroyed.
class Loggable(server.Thing):

    def delete_operation(self, op):
        new_ent = Entity(parent="lumber",
                         mass=self.props.mass,
                         location=self.location.copy(),
                         mode="free")

        # Copy scale if there is one
        if self.props.scale:
            new_ent.scale = self.props.scale
        create = Operation("create",
                           new_ent,
                           to=self.parent)
        # Send through parent since this entity will be destroyed once the op is handled.
        self.parent.send_world(create)

        return server.OPERATION_HANDLED

from atlas import Operation, Entity, Oplist
import server


# When scale is set to <= 0 the entity is deleted
class ScaleDeletable(server.Thing):

    def update_operation(self, op):
        scale_prop = self.props.scale
        if scale_prop and (scale_prop[0] <= 0 or scale_prop[1] <= 0 or scale_prop[2] <= 0):
            return server.OPERATION_HANDLED, Operation("delete", Entity(self.id), to=self)

        return server.OPERATION_HANDLED

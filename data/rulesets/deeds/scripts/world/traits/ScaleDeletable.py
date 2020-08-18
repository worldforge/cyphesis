import server
from atlas import Operation, Entity


# When scale is set to <= 0 the entity is deleted
class ScaleDeletable(server.Thing):

    def scale_property_update(self):
        scale_prop = self.props.scale
        if scale_prop and (scale_prop[0] <= 0 or scale_prop[1] <= 0 or scale_prop[2] <= 0):
            return Operation("delete", Entity(self.id), to=self)

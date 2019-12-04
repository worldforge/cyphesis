import server
from atlas import Operation, Entity


# When status is set to <= 0 the entity is deleted
class StatusDeletable(server.Thing):

    def status_property_update(self):
        if self.get_prop_float('status') <= 0:
            return Operation("delete", Entity(self.id), to=self)

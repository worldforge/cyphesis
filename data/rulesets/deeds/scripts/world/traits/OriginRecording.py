import server
from atlas import Operation, Entity


class OriginRecording(server.Thing):
    """Records an "_origin" property when created. This property can then be used by mind code to make sure that
    NPCs are moving in place."""

    def __init__(self, cpp):
        # At init time not all properties have been applied yet, so we need to send ourselves a "setup"
        # op which will be handled immediately, but with a guarantee that all props are installed.
        self.send_world(Operation("setup", to=self.id))

    def setup_operation(self, op):
        origin = self.get_prop_map("_origin")
        if not origin:
            return Operation('set', Entity(id=self.id, _origin={"$eid": self.parent.id, "pos": self.location.pos}), to=self.id)

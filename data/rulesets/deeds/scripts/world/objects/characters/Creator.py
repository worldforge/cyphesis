import server
from atlas import Operation, Entity


class Creator(server.Thing):
    """
    When there are no more minds controlling the creator entity it should automatically be destroyed.
    """

    def _minds_property_update(self):
        minds = self.get_prop_list("_minds")
        if minds is not None and len(minds) == 0:
            return Operation("delete", Entity(self.id), to=self)

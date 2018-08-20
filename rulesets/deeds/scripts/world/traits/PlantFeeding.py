from atlas import *
import server
from world.utils.Ticks import *


# Used on plant entities that feed from the soil when planted
class PlantFeeding(server.Thing):
    tick_interval = 30

    def __init__(self, cpp):
        init_ticks(self, self.tick_interval)

    def tick_operation(self, op):
        res = Oplist()
        if verify_tick(self, op, res, self.tick_interval):
            if self.props.mode and self.props.mode == 'planted' and self.location.parent:
                # If we're planted we should send an Eat op to our parent.
                # A 'passive' eat op should be ignored by most entities except those with soil.
                # (So a character won't get eaten if a plant is in it's inventory (which in normal cases would mean it's not "planted" though))
                res += Operation("eat", Entity(eat_type='passive'), to=self.location.parent)

            return (server.OPERATION_BLOCKED, res)
        return server.OPERATION_IGNORED

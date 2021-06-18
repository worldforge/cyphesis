import server
from atlas import Operation, Entity, Oplist

from world.utils import Ticks


# Used on plant entities that feed from the soil when planted


class PlantFeeding(server.Thing):
    # Feed each 15 minutes
    tick_interval = 900

    def __init__(self, cpp):
        Ticks.init_ticks(self, self.tick_interval)

    def tick_operation(self, op):
        res = Oplist()
        if Ticks.verify_tick(self, op, res, self.tick_interval):
            if self.props.mode and self.props.mode == 'planted' and self.parent and self.props.mass:
                # If we're planted we should send an Consume op to our parent.
                # A 'soil' consume op should be ignored by most entities except those with soil.
                # (So a character won't get eaten if a plant is in it's inventory
                # (which in normal cases would mean it's not "planted" though))
                # Try to double mass each day
                mass = (self.props.mass ** 0.5) / ((24 * 60 * 60) / PlantFeeding.tick_interval)
                res += Operation("consume",
                                 Entity(consume_type='soil', pos=self.location.pos, mass=mass), to=self.parent)

            return server.OPERATION_BLOCKED, res
        return server.OPERATION_IGNORED

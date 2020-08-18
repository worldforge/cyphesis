import server
from atlas import Operation, Entity, Oplist

from world.utils.Ticks import *


class Replenishing(server.Thing):
    """
    Will automatically replenish itself at an interval.
    The property "__replenish_interval" determines the check interval.
    The property "__replenish_property" determines the name of the property which contains the current amount.
    The property "__replenish_max" determines the max value which the property defined in "__replenish_property" can contain.
    """
    tick_interval = 300  # Default to five minutes

    def __init__(self, cpp):
        self.send_world(Operation("setup", to=self.id))

    def setup_operation(self, op):
        interval = self.get_prop_int("__replenish_interval", self.tick_interval)
        init_ticks(self, interval, interval * 0.2)

    def tick_operation(self, op):
        res = Oplist()
        interval = self.get_prop_int("__replenish_interval", self.tick_interval)
        if verify_tick(self, op, res, interval, interval * 0.2):
            property_name = self.get_prop_string("__replenish_property")
            if property_name:
                max_amount = self.get_prop_int("__replenish_max", 0)
                current_amount = self.get_prop_int(property_name, 0)
                if current_amount < max_amount:
                    res += Operation("set", Entity(self.id, {property_name + "!append": 1}), to=self)
            return server.OPERATION_BLOCKED, res
        return server.OPERATION_IGNORED

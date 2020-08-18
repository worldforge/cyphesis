import server
from atlas import Operation, Entity, Oplist

from world.utils.Ticks import *


# Metabolizing entities that consume nutrients to improve their status
class Metabolizing(server.Thing):
    tick_interval = 30

    def __init__(self, cpp):
        init_ticks(self, self.tick_interval)

    def tick_operation(self, op):
        res = Oplist()
        if verify_tick(self, op, res, self.tick_interval):

            # The simple case is that nutrient will only be consumed to fill up status
            if self.props.status:
                status = self.props.status
                nutrients = self.get_prop_float("_nutrients", 0.0)
                # If no nutrient, we're starving and should decrease status, but only if "starveable" is set.
                if nutrients <= 0:
                    if self.props.starveable == 1:
                        res += Operation("set", Entity({"status!subtract": 0.01}), to=self)
                else:
                    # Else we'll see if we can increase status by consuming nutrient
                    if status < 1.0:
                        # We need to know the mass to know the mass-to-status ratio
                        if self.props.mass:
                            set_ent = Entity()
                            # Consuming 5% of the total mass as nutrient will increase status from 0 to 1.
                            mass_to_status_ratio = 0.05
                            # Convert that into actual mass
                            mass_for_full_status = self.props.mass * mass_to_status_ratio
                            # Then we define that we'll only increase status by a certain step each tick
                            status_increase_per_tick = 0.1
                            # Which gives us the total mass we can consume this tick
                            nutrient_consumed = min(status_increase_per_tick * mass_for_full_status, nutrients)
                            set_ent["_nutrients!subtract"] = nutrient_consumed
                            set_ent["status!append"] = (nutrient_consumed / mass_for_full_status)
                            res += Operation("set", set_ent, to=self)

            return server.OPERATION_BLOCKED, res
        return server.OPERATION_IGNORED

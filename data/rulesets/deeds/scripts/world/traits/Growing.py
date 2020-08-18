import server
from atlas import Operation, Entity, Oplist

from world.utils.Ticks import *


# Growing entities that consume nutrients to increase in size.
class Growing(server.Thing):
    # Grow each 15 minutes
    tick_interval = 900

    def __init__(self, cpp):
        # Use a large jitter so not all entities grow at the same time, as they are
        # initialized at the same time when the world is created.
        init_ticks(self, self.tick_interval, self.tick_interval)

    def tick_operation(self, op):
        res = Oplist()
        if verify_tick(self, op, res, self.tick_interval):
            # Default to 1.0 for max scale, unless something is set.
            max_scale = 1.0
            if self.props.maxscale:
                max_scale = self.props.maxscale
            nutrients = self.get_prop_float("_nutrients", 0)
            density = self.get_prop_float("density")
            mass = self.get_prop_float("mass", 0)
            if density and self.props.bbox and nutrients > 0:
                # Use half of the nutrients to grow
                new_mass = mass + (nutrients * 0.5)
                bbox_unscaled = self.props.bbox
                volume_vector = bbox_unscaled.high_corner - bbox_unscaled.low_corner
                volume_unscaled = volume_vector.x * volume_vector.y * volume_vector.z
                volume_new = new_mass / density
                new_scale = min(pow(volume_new / volume_unscaled, 0.33333), max_scale)

                if not self.props.scale or new_scale != self.props.scale:
                    set_ent = Entity(scale=[new_scale])
                    # check how much nutrient really was used
                    final_new_mass = new_scale * volume_new * density
                    set_ent["_nutrients!subtract"] = final_new_mass - mass

                    res += Operation("set", set_ent, to=self)

            return server.OPERATION_BLOCKED, res
        return server.OPERATION_IGNORED

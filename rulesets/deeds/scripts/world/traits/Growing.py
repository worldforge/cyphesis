from atlas import *
import server
from world.utils.Ticks import *


# Growing entities that consume nutrients to increase in size.
class Growing(server.Thing):
    tick_interval = 30

    def __init__(self, cpp):
        init_ticks(self, self.tick_interval)

    def tick_operation(self, op):
        res = Oplist()
        if verify_tick(self, op, res, self.tick_interval):
            # Default to 1.0 for max scale, unless something is set.
            max_scale = 1.0
            if self.props.maxscale:
                max_scale = self.props.maxscale
            if self.props.mass and self.props.density and self.props.bbox and self.props._nutrients and self.props._nutrients > 0:
                # Use half of the nutrients to grow
                new_mass = self.props.mass + (self.props._nutrients * 0.5)
                bbox_unscaled = self.props.bbox
                volume_vector = bbox_unscaled.high_corner - bbox_unscaled.low_corner
                volume_unscaled = volume_vector.x * volume_vector.y * volume_vector.z
                volume_new = new_mass / self.props.density
                new_scale = min(pow(volume_new / volume_unscaled, 0.33333), max_scale)

                if not self.props.scale or new_scale != self.props.scale:
                    set_ent = Entity(scale=[new_scale])
                    # check how much nutrient really was used
                    final_new_mass = new_scale * volume_new * self.props.density
                    set_ent._nutrients = self.props._nutrients - (final_new_mass - self.props.mass)

                    res += Operation("set", set_ent, to=self)

            return (server.OPERATION_BLOCKED, res)
        return server.OPERATION_IGNORED

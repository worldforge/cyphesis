from atlas import Operation, Entity

import physics
import server
from world.utils import Ticks


class Explosion(server.Thing):
    """An explosion which damages things and then disappears"""

    tick_interval = 1

    def __init__(self, cpp):
        Ticks.init_ticks(self, self.tick_interval)

        domain = self.get_parent_domain()
        if domain:
            blast_radius = self.location.radius
            sphere = physics.Ball(self.location.pos, blast_radius)
            collisions = domain.query_collisions(sphere)
            for collision in collisions:
                entity = collision.entity
                if entity != self:
                    print("Exploded on {}".format(entity.describe_entity()))
                    # TODO: add "id" with the entity that fired the entity, if available
                    # TODO: add "damage"
                    self.send_world(Operation('hit', Entity(hit_type="explosion"), to=entity, id=self.id))

    def tick_operation(self, op):
        return server.OPERATION_HANDLED, Operation("delete", Entity(self.id), to=self.id)

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

            # If there's an "entity_ref" prop it's the reference to the actor which caused the explosion.
            actor_id = self.id
            entity_ref_prop = self.props.entity_ref
            if entity_ref_prop is not None:
                actor_id = entity_ref_prop["$eid"]

            blast_radius = self.location.radius
            sphere = physics.Ball(self.location.pos, blast_radius)
            collisions = domain.query_collisions(sphere)
            for collision in collisions:
                entity = collision.entity
                if entity != self:
                    # TODO: add "damage"
                    self.send_world(Operation('hit', Entity(hit_type="explosion", id=actor_id), to=entity))

    def tick_operation(self, op):
        return server.OPERATION_HANDLED, Operation("delete", Entity(self.id), to=self.id)

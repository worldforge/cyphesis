import physics
import server
from atlas import Operation, Entity, Oplist

from world.utils import Ticks


class Explosion(server.Thing):
    """An explosion which damages things and then disappears"""

    tick_interval = 1

    def __init__(self, cpp):
        # At init time not all properties have been applied yet, so we need to send ourselves a "setup"
        # op which will be handled immediately, but with a guarantee that all props are installed.
        self.send_world(Operation("setup", to=self.id))
        Ticks.init_ticks(self, self.tick_interval)

    def setup_operation(self, op):
        domain = self.get_parent_domain()
        if domain:

            # If there's an "entity_ref" prop it's the reference to the actor which caused the explosion.
            actor_id = self.id
            entity_ref_prop = self.props.entity_ref
            if entity_ref_prop is not None:
                actor_id = entity_ref_prop["$eid"]

            blast_radius = self.location.radius
            if blast_radius > 0:
                sphere = physics.Ball(self.location.pos, blast_radius)
                collisions = domain.query_collisions(sphere)
                base_damage = self.get_prop_float("damage", 0)
                for collision in collisions:
                    entity = collision.entity
                    damage = base_damage * ((blast_radius - collision.distance) / blast_radius)
                    if entity != self:
                        self.send_world(
                            Operation('hit', Entity(hit_type="explosion", id=actor_id, damage=damage), to=entity))

    def tick_operation(self, op):
        if Ticks.verify_tick(self, op, Oplist()):
            return server.OPERATION_HANDLED, Operation("delete", Entity(self.id), to=self.id)
        return server.OPERATION_IGNORED

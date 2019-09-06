from atlas import Operation, Entity, Oplist
import server
import physics


# Used by items that should explode themselves when hit.
class Explodable(server.Thing):

    def hit_operation(self, op):
        res = Oplist()
        domain = self.get_parent_domain()
        if domain:
            blast_radius = self.get_prop_float("blast_radius", 0.0)
            sphere = physics.Ball(self.location.pos, blast_radius)
            collisions = domain.query_collisions(sphere)
            for collision in collisions:
                entity = collision.entity
                if entity != self:
                    print("Exploded on {}".format(entity.describe_entity()))
                    # TODO: add "id" with the entity that fired the entity, if available
                    # TODO: add "damage"
                    res += Operation('hit', Entity(hit_type="explosion"), to=entity, id=self.id)

        return server.OPERATION_HANDLED, Operation("delete", Entity(self.id), to=self.id), res

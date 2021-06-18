import rules
import server
from atlas import Operation, Entity, Oplist


def get_actor_id_from_mode_data(thing):
    mode_data = thing.props.mode_data
    actor_id = thing.id
    # Check if there's an entity ref contained in the mode_data prop,
    # and if so attach that to the "entity_ref" prop of the explosion.
    # This way the explosion can properly attribute any Hit op it sends to the actor which fired the item.
    if mode_data:
        entity_ref = mode_data['$eid']
        if entity_ref is not None:
            actor_id = entity_ref
    return actor_id


# Used by items that should explode themselves when hit.
class Explodable(server.Thing):

    def hit_operation(self, op):
        res = Oplist()
        arg = op[0]
        if arg:
            # Place the explosion at the point of collision.
            new_location = rules.Location(self.parent, arg.pos)

            entity = Entity(parent="explosion", location=new_location, mode="fixed")
            entity["entity_ref"] = {"$eid": get_actor_id_from_mode_data(self)}
            damage_explosion = self.props.damage_explosion
            if damage_explosion is not None:
                entity["damage"] = damage_explosion

            res.append(Operation("create", entity, to=self.id))

        res.append(Operation("delete", Entity(self.id), to=self.id))
        return server.OPERATION_HANDLED, res


# Like a fireball, but creates poison effects on the hit entity.
class Poisonable(server.Thing):

    def hit_operation(self, op):
        res = Oplist()
        arg = op[0]
        if arg:
            # Create a poisoning instance
            new_entity = Entity(parent="poisoning", loc=op["from"])
            new_entity["entity_ref"] = {"$eid": get_actor_id_from_mode_data(self)}
            damage_prop = self.props.damage_poison
            if damage_prop is not None:
                new_entity.damage = damage_prop
            res.append(Operation("create", new_entity, to=arg.id))

        res.append(Operation("delete", Entity(self.id), to=self.id))
        return server.OPERATION_HANDLED, res

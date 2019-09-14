from atlas import Operation, Entity, Oplist
import server
import rules


# Used by items that should explode themselves when hit.
class Explodable(server.Thing):

    def hit_operation(self, op):
        res = Oplist()
        arg = op[0]
        if arg:
            # Place the explosion at the point of collision.
            new_location = rules.Location(self.location.parent, arg.pos)

            entity = Entity(parent="explosion", location=new_location, mode="fixed")
            mode_data = self.props.mode_data
            actor_id = self.id
            # Check if there's an entity ref contained in the mode_data prop, and if so attach that to the "entity_ref" prop of the explosion.
            # This way the explosion can properly attribute any Hit op it sends to the actor which fired the item.
            if mode_data:
                entity_ref = mode_data['$eid']
                if entity_ref is not None:
                    actor_id = entity_ref
            entity["entity_ref"] = {"$eid": actor_id}

            res.append(Operation("create", entity, to=self.id))
            res.append(Operation("delete", Entity(self.id), to=self.id))
        return server.OPERATION_HANDLED, res

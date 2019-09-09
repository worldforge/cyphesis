from atlas import Operation, Entity
import server


# Apply to all entities that should take damage when hit.
# Modifiers can be applied for various things. The damage value will be multiplied with the modifier.
# To modify on the type of hit, apply a "__modifier_hit_type_*" modifier.
# For example, if you want to increase the damage taken when chopping you should add a "__modifier_hit_type_chop" property.
class Hittable(server.Thing):

    def hit_operation(self, op):
        arg = op[0]
        if arg:
            status_decrease = 0.1
            if hasattr(arg, 'damage'):
                status_decrease = arg.damage / 100.0
            # Check if there's a modifier for the specific type of hit.
            if hasattr(arg, 'hit_type') and self.props["__modifier_hit_type_" + arg.hit_type]:
                status_decrease = status_decrease * self.props["__modifier_hit_type_" + arg.hit_type]

            print("Hit for {} damage".format(status_decrease))

            new_status = self.props.status - status_decrease
            hit_op = op.copy()
            hit_op[0].damage = status_decrease
            return server.OPERATION_BLOCKED, Operation("set", Entity(self.id, status=new_status), to=self.id), Operation('sight', hit_op)

        return server.OPERATION_IGNORED

import server
from atlas import Operation, Entity, Oplist


# Apply to all entities that should take damage when hit.
# Modifiers can be applied for various things. The damage value will be multiplied with the modifier.
# To modify on the type of hit, apply a "__modifier_hit_type_*" modifier.
# For example, if you want to increase the damage taken when chopping you
# should add a "__modifier_hit_type_chop" property.
class Hittable(server.Thing):

    def hit_operation(self, op):
        arg = op[0]
        if arg:
            hit_op = op.copy()
            res = Oplist()
            if hasattr(arg, 'damage'):
                # Apply any armor modifiers
                armor = self.get_prop_float("armor", 0)
                status_decrease = (arg.damage - armor) / 100.0
                # Check if there's a modifier for the specific type of hit.
                if hasattr(arg, 'hit_type') and self.props["__modifier_hit_type_" + arg.hit_type]:
                    status_decrease = status_decrease * self.props["__modifier_hit_type_" + arg.hit_type]
                # print("Hit for {} damage".format(status_decrease))
                status_decrease = max(0.0, status_decrease)  # Make sure it's not negative
                hit_op[0].damage = status_decrease
                if status_decrease > 0:
                    res.append(Operation("set", Entity(self.id, {"status!subtract": status_decrease}), to=self.id))
            return server.OPERATION_BLOCKED, res, Operation('sight', hit_op)

        return server.OPERATION_IGNORED

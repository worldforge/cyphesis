from atlas import *
import server

#Apply to all entities that should take damage when hit.
#Modifiers can be applied for various things. The damage value will be multiplied with the modifier.
#To modify on the type of hit, apply a "__modifier_hit_type_*" modifier.
# For example, if you want to increase the damage taken when chopping you should add a "__modifier_hit_type_chop" property.
class Hittable(server.Thing):

    def hit_operation(self, op):
        print("Hit!")
        arg = op[0]
        if arg:
            statusDecrease = 0.1
            if hasattr(arg, 'damage'):
                statusDecrease = arg.damage / 100.0
            #Check if there's a modifier for the specific type of hit.
            if hasattr(arg, 'hit_type') and self.props["__modifier_hit_type_"+arg.hit_type]:
                statusDecrease = statusDecrease * self.props["__modifier_hit_type_"+arg.hit_type]

            return (server.OPERATION_BLOCKED, Operation("set", Entity(self.id, status=self.props.status - statusDecrease), to=self.id))

        return server.OPERATION_IGNORED

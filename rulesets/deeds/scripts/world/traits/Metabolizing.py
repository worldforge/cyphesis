from atlas import *
import server

#Metabolizing entities that consume energy and receive nourishment
class Metabolizing(server.Thing):

    def nourish_operation(self, op):
        #Get the mass of the contained arg, convert it to food through _modifier_eat* properties, and increase the "_food" property.
        #Check any limits on the amount of food we can contain in our stomach/reserves through the _food_max prop too.
        if len(op) > 0:
            arg = op[0]
            if hasattr(arg, 'mass'):
                #Check if we can convert to food through the _modifier_eat property.
                #We also check if there are specific values for herbivores and omnivores (_modifier_eat_type_meat and _modifier_eat_type_plant)
                eat_factor = 0
                if self.props._modifier_eat:
                    eat_factor = self.props._modifier_eat
                if hasattr(arg, 'eat_type'):
                    if self.props["_modifier_eat_type_" + arg.eat_type]:
                        eat_factor = self.props["_modifier_eat_type_" + arg.eat_type]

                if eat_factor != 0:
                    food = 0
                    if self.props._food:
                        food = self.props._food
                    food_new = food + (arg.mass * eat_factor)
                    #Check if there's a limit to the food we can contain in our stomach
                    if self.props._food_max_factor and self.props.mass:
                        food_new = min(self.props._food_max_factor * self.props.mass, food_new)

                    if food_new != food:
                        return (server.OPERATION_BLOCKED, Operation("set", Entity(self.id, _food=food_new), to=self))

        return (server.OPERATION_BLOCKED)

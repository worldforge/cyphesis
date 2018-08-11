from atlas import *
import server
from world.utils.Ticks import *


# Metabolizing entities that consume energy and receive nourishment
class Metabolizing(server.Thing):
    tick_refno = 0
    tick_interval = 30

    def __init__(self, cpp):
        init_ticks(self, self.tick_interval)

    def nourish_operation(self, op):
        # Get the mass of the contained arg, convert it to food through _modifier_eat* properties, and increase the "_food" property.
        # Check any limits on the amount of food we can contain in our stomach/reserves through the _food_max prop too.
        if len(op) > 0:
            arg = op[0]
            if hasattr(arg, 'mass'):
                # Check if we can convert to food through the _modifier_eat property.
                # We also check if there are specific values for herbivores and omnivores (_modifier_eat_type_meat and _modifier_eat_type_plant)
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
                    # Check if there's a limit to the food we can contain in our stomach
                    if self.props._food_max_factor and self.props.mass:
                        food_new = min(self.props._food_max_factor * self.props.mass, food_new)

                    if food_new != food:
                        return (server.OPERATION_BLOCKED, Operation("set", Entity(self.id, _food=food_new), to=self))

        return (server.OPERATION_BLOCKED)

    def tick_operation(self, op):
        res = Oplist()
        if verify_tick(self, op, res, self.tick_interval):

            # The simple case is that food will only be consumed to fill up status
            status = self.mod_property("status")
            food = 0
            if self.props._food:
                food = self.props._food
            # If no food, we're starving and should decrease status
            if food <= 0:
                res += Operation("set", Entity(status=status - 0.01), to=self)
            else:
                # Else we'll see if we can increase status by consuming food
                if status < 1.0:
                    # We need to know the mass to know the mass-to-status ratio
                    if self.props.mass:
                        set_ent = Entity()
                        # Consuming 5% of the total mass as food will increase status from 0 to 1.
                        mass_to_status_ratio = 0.05
                        # Convert that into actual mass
                        mass_for_full_status = self.props.mass * mass_to_status_ratio
                        # Then we define that we'll only increase status by a certain step each tick
                        status_increase_per_tick = 0.1
                        # Which gives us the total mass we can consume this tick
                        food_consumed = min(status_increase_per_tick * mass_for_full_status, food)
                        set_ent._food = food - food_consumed
                        set_ent.status = status + (food_consumed / mass_for_full_status)
                        res += Operation("set", set_ent, to=self)

            return (server.OPERATION_BLOCKED, res)
        return server.OPERATION_IGNORED

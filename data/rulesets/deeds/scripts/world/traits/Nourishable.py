import server
from atlas import Operation, Entity


# Nourishable entities that receive nourishment and increase their '_nutrients' value.
class Nourishable(server.Thing):

    def nourish_operation(self, op):
        # Get the mass of the contained arg, convert it to nutrient through _modifier_eat* properties,
        # and increase the "_nutrients" property.
        # Check any limits on the amount of nutrient we can contain in our stomach/reserves
        # through the _nutrients_max prop too.
        if len(op) > 0:
            arg = op[0]
            if hasattr(arg, 'mass'):
                # print('mass {}'.format(arg.mass))
                # Check if we can convert to nutrient through the _modifier_eat property.
                # We also check if there are specific values for herbivores and omnivores
                # (_modifier_consume_type_meat and _modifier_consume_type_plant)
                consume_factor = 0
                if self.props._modifier_eat:
                    consume_factor = self.props._modifier_eat
                if hasattr(arg, 'consume_type'):
                    if self.props["_modifier_consume_type_" + arg.consume_type]:
                        consume_factor = self.props["_modifier_consume_type_" + arg.consume_type]

                # print("consume factor {}".format(consume_factor))
                if consume_factor != 0:
                    nutrient = 0
                    if self.props._nutrients:
                        nutrient = self.props._nutrients
                    nutrient_new = nutrient + (arg.mass * consume_factor)
                    # Check if there's a limit to the nutrient we can contain in our stomach
                    if self.props._nutrients_max_factor and self.props.mass:
                        nutrient_new = min(self.props._nutrients_max_factor * self.props.mass, nutrient_new)

                    if nutrient_new != nutrient:
                        return server.OPERATION_BLOCKED, \
                               Operation("set", Entity(self.id, _nutrients=nutrient_new), to=self)

        return server.OPERATION_BLOCKED

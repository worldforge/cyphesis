import server
from atlas import Operation, Entity
from physics import Vector3D


# Plant nourishing entities are those that can sustain plants in them.
# When a "consume" op of type "soil" is received a "nourish" op is returned.
# This would normally be placed on an entity representing "ground".
# In those cases, if it also has a "terrain" property a check if done against that to
# make sure that the ground can sustain plants.
class PlantNourishing(server.Thing):

    def consume_operation(self, op):
        if len(op) > 0:
            arg = op[0]
            if arg.consume_type == "soil":

                # The plant should specify how much mass it wants to absorb with each "bite"
                mass_arg = arg.mass
                if not mass_arg:
                    print("Plant consume op without mass specified")
                    return server.OPERATION_BLOCKED

                nourish_ent = Entity()
                nourish_ent.consume_type = arg.consume_type

                terrain_prop = self.props.terrain
                # If there's a terrain prop, check that the surface has dirt, otherwise just return a nourish op
                if terrain_prop:
                    material = terrain_prop.get_surface_name(arg.pos[0], arg.pos[2])
                    if material != "grass":
                        # No earth here
                        return server.OPERATION_BLOCKED
                    else:
                        # Check the slope
                        normal = terrain_prop.get_normal(arg.pos[0], arg.pos[2])
                        slope = normal.dot(Vector3D(1, 0, 0)) / normal.mag()
                        # The mass received is dependent on the slope
                        nourish_ent.mass = mass_arg * (1 - slope)
                else:
                    # Just supply the same mass as was requested
                    nourish_ent.mass = mass_arg

                # Note that we don't change our own status;
                # unlike creatures being "eaten" soil doesn't lose status in this simulation
                nourish_op = Operation("nourish", nourish_ent, to=op.from_)

                return server.OPERATION_BLOCKED, nourish_op

        return server.OPERATION_IGNORED

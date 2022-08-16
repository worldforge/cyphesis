import math
import random

import entity_filter
import physics
import server
from atlas import Oplist, Operation, Entity
from common import new_serial_no

from world.utils.Ticks import init_ticks, verify_tick


def get_spawn_pos(entity):
    # First select one of the locations
    locations = entity.get_prop_list("__spawner_locations")
    if locations:
        selected_location = locations[random.randint(0, len(locations) - 1)]
        location_radius = selected_location["radius"]

        if "id" in selected_location:
            entity = server.world.get_entity(selected_location["id"])

        if entity:
            pos = entity.location.pos

            if "radius" in selected_location:
                # Find a random point on a sphere
                a = random.random() * 2 * math.pi
                r = location_radius * math.sqrt(random.random())

                x = r * math.cos(a)
                z = r * math.sin(a)
                pos.x = pos.x + x
                pos.z = pos.z + z
                return pos
    else:
        # Fall back to entity position
        return entity.location.pos
    return None


class SpawnerBase(server.Thing):
    tick_interval = 30

    def tick_operation(self, op):
        interval = self.get_prop_int("__spawner_interval", self.tick_interval)
        res = Oplist()
        if verify_tick(self, op, res, interval):
            # Check if the spawner is currently disabled through the optional "__spawner_disabled" property
            if self.get_prop_int("__spawner_disabled", 0) == 0:
                self.check_spawning(res)

        return res

    def spawn_new_entity(self, res):
        pos = get_spawn_pos(self)

        if pos:
            entity_map = self.get_prop_map("__spawner_entity")
            entity_map["pos"] = pos

            # Randomize orientation
            rotation = random.random() * math.pi * 2
            orientation = physics.Quaternion(physics.Vector3D(0, 1, 0), rotation)
            entity_map["orientation"] = orientation.as_list()

            return Operation("create", entity_map, to=self.id)


class SpawnerVicinity(SpawnerBase):
    """
    Trait which spawns new entities by checking if there are enough within a radius from itself.
    The "__spawner_criteria" property determines which entities that will be counted.

    Some care needs to be taken to avoid this spawner from running amok and spawning an endless number of entities.
    If the "criteria" doesn't match the entities that are created, this can happen.
    It could also happen if the area within which the check is performed differs from the locations where new entities are spawned.

    """

    criteria = None

    def __init__(self, cpp):
        self.send_world(Operation("setup", to=self.id))

    def setup_operation(self, op):
        init_ticks(self, self.get_prop_int("__spawner_interval", self.tick_interval))
        self.create_criteria()

    def __spawner_criteria_property_update(self):
        self.create_criteria()

    def create_criteria(self):
        criteria_string = self.get_prop_string("__spawner_criteria")
        if criteria_string:
            self.criteria = entity_filter.Filter(criteria_string)

    def check_spawning(self, res):
        # Check that there are enough entities fulfilling the critieria in vicinity.
        if self.criteria and self.get_prop_float("__spawner_radius", 0) > 0:
            domain = self.get_parent_domain()
            if domain:
                missing_amount = self.get_prop_int("__spawner_amount", 0)
                if missing_amount > 0:
                    sphere = physics.Ball(self.location.pos, self.get_prop_float("__spawner_radius", 0))
                    collisions = domain.query_collisions(sphere)
                    for collision in collisions:
                        entity = collision.entity
                        if server.world.match_entity(self.criteria, entity):
                            missing_amount = missing_amount - entity.get_prop_int("amount", 1)

                        if missing_amount == 0:
                            break

                    if missing_amount > 0:
                        # We need to spawn an entity.
                        create_op = self.spawn_new_entity(res)
                        if create_op:
                            res.append(create_op)


class SpawnerInstances(SpawnerBase):
    """
    Trait which spawns new entities by checking against a list of entities previously created.

    To keep track of this we attach a serial_no to all outgoing Create operations, and then listen to all incoming Info
    operations and pick up those that match.
    """

    criteria = None
    # Keep track of the "create" operations that we've sent, so we can update the __spawner_instances property when new entities are created by us.
    create_serials = []

    def __init__(self, cpp):
        init_ticks(self, self.get_prop_int("__spawner_interval", self.tick_interval))

    def info_operation(self, op):
        # Check if it's as a response to an entity we've created
        if op.refno in self.create_serials:
            self.create_serials.remove(op.refno)
            return Operation("set", Entity({"__spawner_instances!append": [{"$eid": op[0].id}]}), to=self.id)

    def check_spawning(self, res):

        domain = self.get_parent_domain()
        if domain:
            missing_amount = self.get_prop_int("__spawner_amount", 0)
            if missing_amount > 0:
                instances_list = self.get_prop_list("__spawner_instances", [])
                for entity_id_entry in instances_list:
                    entity_id = entity_id_entry["$eid"]
                    entity = server.world.get_entity(entity_id)
                    if entity is not None:
                        missing_amount = missing_amount - 1
                    else:
                        # Entity doesn't exist anymore; remove it from our list of entities.
                        res.append(Operation("set", Entity({"__spawner_instances!subtract": [entity_id_entry]}), to=self.id))
                    if missing_amount == 0:
                        break

                if missing_amount > 0:
                    # We need to spawn an entity.
                    create_op = self.spawn_new_entity(res)
                    if create_op:
                        serial_no = new_serial_no()
                        self.create_serials.append(serial_no)
                        create_op.set_serialno(serial_no)
                        res.append(create_op)

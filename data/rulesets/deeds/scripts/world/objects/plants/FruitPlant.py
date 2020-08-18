import sys

import server
from atlas import Operation, Entity, Oplist
from physics import Vector3D

from world.utils.Ticks import *


class FruitPlant(server.Thing):
    """
    When touched will drop fruits randomly, if there are fruits.
    In addition there's a tick handler which randomly drops any fruits.
    """

    # Check each 10 minutes
    tick_interval = 600

    def __init__(self, cpp):
        init_ticks(self, self.tick_interval, self.tick_interval * 0.5)

    def drop_fruit(self, res, parent):
        height = self.location.bbox.high_corner.y

        newloc = self.location.copy()
        newloc.velocity = Vector3D()
        newloc.pos = newloc.pos + Vector3D(random.uniform(-height, height), 0, random.uniform(-height, height))

        res.append(Operation("create", Entity(parent=parent, location=newloc), to=self))

    def tick_operation(self, op):
        res = Oplist()
        if verify_tick(self, op, res, self.tick_interval):

            fruits = 0
            if self.props.fruits:
                fruits = self.props.fruits

            # Check if we should drop any fruit
            fruits_change = self.handle_drop_fruit(res, fruits)

            # And then check if we should create any new fruits
            if fruits + fruits_change > 0:
                fruits_change += self.handle_fruiting()

            if fruits_change != 0:
                res += Operation("set", Entity(self.id, {"fruits!append": fruits_change}), to=self)

            return server.OPERATION_BLOCKED, res
        return server.OPERATION_IGNORED

    def handle_drop_fruit(self, res, fruits):
        if fruits > 0:
            if self.props.fruit_name and self.props.fruit_chance:
                # hard coded to 5% chance of dropping fruits
                if random.uniform(0, 100) < 5:
                    self.drop_fruit(res, self.props.fruit_name)
                    return -1
        return 0

    def handle_fruiting(self):
        if not self.has_prop_int("fruits_max"):
            print('FruitPlant script on entity without any fruits_max.', file=sys.stderr)
        else:
            fruits_max = self.get_prop_int("fruits_max")
            if fruits_max:
                # The tree will drop fruits if it's at least fruiting_min_scale large (if there's no "scale" it's 1.0)
                scale_prop = self.props.scale
                if scale_prop is not None:
                    scale = 0
                    if len(scale_prop) == 1:
                        scale = scale_prop[0]
                    else:
                        scale = scale_prop[1]  # we want the height
                    if scale > self.props.fruiting_min_scale:
                        if not self.props.fruits or self.props.fruits < fruits_max:
                            if self.props.fruit_name and self.props.fruit_chance:
                                if random.uniform(0, 100) < self.props.fruit_chance:
                                    return 1
        return 0

    def harvest_operation(self, op):
        res = Oplist()
        if self.props.fruits and self.props.fruits > 0 and self.props.fruit_name:
            res.append(Operation("create", Entity(parent=self.props.fruit_name, loc=op.id), to=self))
            res.append(Operation("set", Entity(self.id, {"fruits!subtract": 1}), to=self))
            res.append(Operation("imaginary", Entity(
                description="You harvest an {} from the {}.".format(self.props.fruit_name, self.type)), to=op.id,
                                 from_=op.id))
        else:
            res.append(Operation("imaginary", Entity(
                description="There aren't any {}s in this {}.".format(self.props.fruit_name, self.type)), to=op.id,
                                 from_=op.id))
        return server.OPERATION_BLOCKED, res

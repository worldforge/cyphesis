import sys

from atlas import Operation, Entity, Oplist
from physics import Vector3D
import server
from world.utils.Ticks import *


class FruitPlant(server.Thing):
    """
    When touched will drop fruits randomly, if there are fruits.
    In addition there's a tick handler which randomly drops any fruits.
    """
    tick_interval = 60
    jitter = 20

    def __init__(self, cpp):
        init_ticks(self, self.tick_interval, self.jitter)

    def drop_fruit(self, res, parent):
        height = self.location.bbox.high_corner.y

        newloc = self.location.copy()
        newloc.velocity = Vector3D()
        newloc.pos = newloc.pos + Vector3D(random.uniform(-height, height), 0, random.uniform(-height, height))

        res.append(Operation("create", Entity(parent=parent, location=newloc), to=self))

    def touch_operation(self, op):
        res = Oplist()
        self.handle_drop_fruit(res)
        return server.OPERATION_BLOCKED, res

    def tick_operation(self, op):
        res = Oplist()
        if verify_tick(self, op, res, self.tick_interval, self.jitter):
            # Check if we should drop any fruit
            self.handle_drop_fruit(res)

            # And then check if we should create any new fruits
            self.handle_fruiting(res)

            return server.OPERATION_BLOCKED, res
        return server.OPERATION_IGNORED

    def handle_drop_fruit(self, res):

        if self.props.fruits and self.props.fruits > 0:
            if self.props.fruit_name and self.props.fruit_chance:
                # hard coded to 5% chance of dropping fruits
                if random.uniform(0, 100) < 5:
                    self.drop_fruit(res, self.props.fruit_name)
                    # TODO: use 'modify' op
                    res.append(Operation("set", Entity(self.id, fruits=self.props.fruits - 1), to=self))

    def handle_fruiting(self, res):
        if not self.has_prop_int("fruits_max"):
            print('FruitPlant script on entity without any fruits_max.', file=sys.stderr)
        else:
            fruits_max = self.get_prop_int("fruits_max")
            if fruits_max:
                # The tree will drop fruits if it's at least fruiting_min_scale large (if there's no "scale" it's 1.0)
                if not self.props.scale or self.props.scale[1] > self.props.fruiting_min_scale:
                    if not self.props.fruits or self.props.fruits < fruits_max:
                        if self.props.fruit_name and self.props.fruit_chance:
                            if random.uniform(0, 100) < self.props.fruit_chance:
                                # TODO: use 'modify' op
                                res += Operation("set", Entity(self.id, fruits=self.props.fruits - 1), to=self)

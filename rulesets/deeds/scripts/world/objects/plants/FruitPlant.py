from atlas import *
from physics import Vector3D
import server
import random

### When touched will drop fruits randomly, if there are fruits.
class FruitPlant(server.Thing):

    def dropFruit(self, res, parent):
        height = self.location.bbox.high_corner.y

        newloc=self.location.copy()
        newloc.velocity=Vector3D()
        newloc.position = newloc.position + Vector3D(random.uniform(-height, height), 0, random.uniform(-height, height))

        res += Operation("create", Entity(parent=parent,location=newloc), to=self)

    def touch_operation(self, op):
        res = Oplist()

        if self.props.sizeAdult:
            if self.location.bbox.high_corner.y > self.props.sizeAdult:
                if self.props.fruits and self.props.fruits > 0:
                    if self.props.fruitName and self.props.fruitChance:
                        if random.uniform(0, 100) < self.props.fruitChance:
                            self.dropFruit(res, self.props.fruitName)
                            res += Operation("set", Entity(self.id, fruits=self.props.fruits-1), to=self)
                            return (server.OPERATION_BLOCKED, res)




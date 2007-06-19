from atlas import *
from cyphesis.Thing import Thing

import atlas

class Fir(Thing):
    def eat_operation(self, op):
        ent = Entity(self.id, status = self.status - 0.001)
        res = Operation("set", ent, to=self)
        to_ = op.from_
        nour= Entity(to_.id, mass= 0.002)
        res = res + Operation("nourish", nour, to=to_)
        return res

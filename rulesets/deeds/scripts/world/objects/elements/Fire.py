# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 1999 Aloril (See the file COPYING for details).

from atlas import Operation, Entity, Oplist
from common import const
from world.utils import Ticks

import server


class Fire(server.Thing):
    """fire to burn things up"""

    tick_interval = 5

    def __init__(self, cpp):
        Ticks.init_ticks(self, self.tick_interval)
        if self.location.parent:
            print('initial burn')
            self.send_world(Operation("eat", Entity(eat_type='fire'), to=self.location.parent))
        else:
            print('no parent')

    def tick_operation(self, op):
        res = Oplist()
        if Ticks.verify_tick(self, op, res, self.tick_interval):
            status_prop = self.props.status
            if status_prop:
                if self.location.parent:
                    print("Flame eating into parent")
                    # We should send an Eat op to our parent.
                    # A 'fire' eat op should be ignored by most entities except those that are flammable.
                    res += Operation("eat", Entity(eat_type='fire'), to=self.location.parent)
                    # Reduce status of fire
                    res += Operation("set", Entity(status=status_prop - 0.2), to=self)

            else:
                print("Flammable entity without status props.")

            return server.OPERATION_BLOCKED, res
        return server.OPERATION_IGNORED

    def nourish_operation(self, op):
        print("Flame is nourished")
        status_prop = self.props.status
        if not status_prop:
            print("Flammable entity without status prop")
            return server.OPERATION_IGNORED
        if len(op) > 0:
            arg = op[0]
            if arg.eat_type == "fire":
                return server.OPERATION_BLOCKED, Operation("set", Entity(status=status_prop + 0.2), to=self)

        return server.OPERATION_IGNORED

    # CHEAT! make it more realistic (like spreading to things that burn near)
    def extinguish_operation(self, op):
        """If somebody tries to extinguish us, change status lower"""
        status_prop = self.props.status
        if not status_prop:
            print("Flammable entity without status prop")
            return server.OPERATION_IGNORED
        return Operation("set", Entity(self.id, status=status_prop - 0.25), to=self)

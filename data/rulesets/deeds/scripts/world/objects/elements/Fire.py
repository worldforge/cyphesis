# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 1999 Aloril (See the file COPYING for details).

import server
from atlas import Operation, Entity, Oplist

from world.utils import Ticks


class Fire(server.Thing):
    """fire to burn things up"""

    tick_interval = 30

    def __init__(self, cpp):
        Ticks.init_ticks(self, self.tick_interval)
        if self.parent:
            print('initial burn')
            self.send_world(Operation("consume", Entity(consume_type='fire'), to=self.parent))
        else:
            print('no parent')

    def tick_operation(self, op):
        res = Oplist()
        if Ticks.verify_tick(self, op, res, self.tick_interval):
            status_prop = self.props.status
            if status_prop is not None:
                if self.parent:
                    print("Flame eating into parent")
                    # We should send an Consume op to our parent.
                    # A 'fire' consume op should be ignored by most entities except those that are flammable.
                    res += Operation("consume", Entity(consume_type='fire'), to=self.parent)
                    # Reduce status of fire
                    res += Operation("set", Entity({"status!subtract": 0.2}), to=self)

            else:
                print("Fire entity without status props.")

            return server.OPERATION_BLOCKED, res
        return server.OPERATION_IGNORED

    def nourish_operation(self, op):
        print("Flame is nourished")
        if not self.has_prop_float('status'):
            print("Fire entity without status prop")
            return server.OPERATION_BLOCKED
        status_prop = self.props.status
        if len(op) > 0:
            arg = op[0]
            if arg.consume_type == "fire":
                new_status_prop = min(1, status_prop + 0.3)
                return server.OPERATION_BLOCKED, Operation("set", Entity(status=new_status_prop), to=self)

        return server.OPERATION_BLOCKED

    # CHEAT! make it more realistic (like spreading to things that burn near)
    def extinguish_operation(self, op):
        """If somebody tries to extinguish us, change status lower"""
        status_prop = self.props.status
        if status_prop is not None:
            return server.OPERATION_HANDLED, Operation("set", Entity(self.id, {"status!subtract": 0.25}), to=self)
        else:
            print("Fire entity without status prop")
            return server.OPERATION_HANDLED

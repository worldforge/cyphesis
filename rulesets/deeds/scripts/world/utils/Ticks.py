from xml.dom.minidom import Entity

from atlas import *

# Contains helper functions for handling ticks in entity scripts.
# Typically you would want to call 'init_ticks' in your __init__ method,
# and then call 'verify_tick' in 'tick_operation' method.

def init_ticks(self, interval):
    self.tick_refno = 0
    self.send_world(Operation("tick", Entity(name=self.__class__.__name__), refno=self.tick_refno, future_seconds=interval, to=self.id))


def verify_tick(self, op, res, interval):
    if len(op) > 0:
        arg = op[0]

        if arg.name == self.__class__.__name__ and op.refno == self.tick_refno:
            self.tick_refno += 1
            res.append(Operation("tick", arg, refno=self.tick_refno, future_seconds=interval, to=self.id))
            return True
    return False

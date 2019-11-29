import random

from atlas import Operation, Entity


# Contains helper functions for handling ticks in entity scripts.
# Typically you would want to call 'init_ticks' in your __init__ method,
# and then call 'verify_tick' in 'tick_operation' method.

def init_ticks(self, interval, jitter=0):
    self.tick_refno = 0
    self.send_world(Operation("tick", Entity(name=self.__class__.__name__), refno=self.tick_refno,
                              future_seconds=interval + random.uniform(0, jitter), to=self.id))


def verify_tick(self, op, res, interval=0, jitter=0):
    """Checks that the tick contained in the op is valid, i.e. is the next one for us.
        If so, a new tick op is added to res, and True is returned.
    """
    if len(op) > 0:
        arg = op[0]

        if arg.name == self.__class__.__name__ and op.refno == self.tick_refno:
            if interval > 0:
                self.tick_refno += 1
                res.append(Operation("tick", arg, refno=self.tick_refno,
                                     future_seconds=interval + random.uniform(0, jitter), to=self.id))
            return True
    return False

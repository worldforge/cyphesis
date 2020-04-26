# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 2020 Erik Ogenvik (See the file COPYING for details).

import server
from atlas import Operation, Entity


class Mineable(server.Thing):
    """
    Allows mining of resources by sending an "excavate" op.
    The property "__deposits" determine how many deposits can be mines.
    The property "__deposits_entity" determines the entity that's created when mining.
    """

    def excavate_operation(self, op):
        deposits = self.get_prop_int("__deposits", 0)
        if deposits > 0:
            entity_map = self.get_prop_map("__deposits_entity")
            entity_map["loc"] = op.id

            create_op = Operation("create", entity_map, to=self)
            set_op = Operation("set", Entity(self.id, {"__deposits!subtract": 1}), to=self)
            imaginary_op = Operation("imaginary",
                                     Entity(description="You extract from the deposit."), to=op.id, from_=op.id)

            return create_op, set_op, imaginary_op
        else:
            return Operation("imaginary",
                             Entity(description="You couldn't find anything to extract from this deposit."), to=op.id, from_=op.id)

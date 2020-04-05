# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 2019 Erik Ogenvik (See the file COPYING for details).

import server
from atlas import Operation, Entity


class Pile(server.Thing):
    """
    When sifting through a pile, earthworms might be extracted.
    """

    def sift_operation(self, op):
        if self.props._worms and self.props._worms > 0:
            create_op = Operation("create", Entity(name="earthworm", parent="annelid", loc=op.id), to=self)
            set_op = Operation("set", Entity(self.id, {"_worms!subtract": 1}), to=self)
            imaginary_op = Operation("imaginary",
                                     Entity(description="You found a worm in the pile."), to=op.id, from_=op.id)

            return create_op, set_op, imaginary_op
        else:
            return Operation("imaginary",
                             Entity(description="You couldn't find any worms in this pile."), to=op.id, from_=op.id)

    def dig_operation(self, op):
        """Digging into the pile will disperse it, making it smaller"""
        scale_reduction = 0.5
        set_ent = Entity(self.id, scale=[1.0 - scale_reduction])
        scale_prop = self.props.scale
        if scale_prop:
            if len(scale_prop) == 3:
                set_ent.scale = [scale_prop[0] - scale_reduction,
                                 scale_prop[1] - scale_reduction,
                                 scale_prop[2] - scale_reduction]
            else:
                set_ent.scale = [scale_prop[0] - scale_reduction]

        return Operation("set", set_ent, to=self), \
               Operation("imaginary", Entity(description="You dispersed the pile."), to=op.id, from_=op.id)

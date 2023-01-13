# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 1999 Al Riddoch (See the file COPYING for details).

from random import randint, uniform

import server
from atlas import Oplist, Operation, Entity


class Weather(server.Thing):
    def tick_operation(self, op):
        world = self.parent
        res = Oplist()
        optick = Operation("tick", to=self)
        res = res + optick
        if self.props.rain < 0.1:
            optick.set_future_seconds(randint(60, 300))
            self.rain = uniform(0.1, 0.9)
            self.visibility = 10 / (self.props.rain * self.props.rain)
            res = res + Operation("set", Entity(world.id, moisture=1), to=world)
        else:
            moisture = world.moisture
            optick.set_future_seconds(randint(600, 2400))
            self.props.rain = 0.0
            self.props.visibility = 1000
            res = res + Operation("set", Entity(world.id, moisture=moisture - 0.5), to=world)
        res = res + Operation("set", Entity(self.id, rain=self.props.rain, visibility=self.props.visibility), to=self)
        return res

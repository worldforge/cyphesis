# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 2019 Erik Ogenvik (See the file COPYING for details).

from atlas import Operation, Entity
from world.utils import Usage

import server


class StoppableTask(server.Task):

    def __init__(self, *args, **kwargs):
        super().__init__(self, *args, **kwargs)
        self.usages = self.usages + ["stop"]

    def stop_usage(self):
        self.irrelevant()
        return server.OPERATION_HANDLED

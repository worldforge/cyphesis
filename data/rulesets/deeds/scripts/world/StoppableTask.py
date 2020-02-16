# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 2019 Erik Ogenvik (See the file COPYING for details).

import server


class StoppableTask(server.Task):
    """Base class for all classes that are "stoppable", i.e. have a "stop" usage.
    The "stop" usage will just stop the task."""

    def __init__(self, *args, **kwargs):
        super().__init__(self, *args, **kwargs)
        self.usages = self.usages + [{"name": "stop"}]

    def stop_usage(self, args):
        self.irrelevant()
        return server.OPERATION_HANDLED

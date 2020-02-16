# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 2019 Erik Ogenvik (See the file COPYING for details).

import server

from world.StoppableTask import StoppableTask


class PendingTask(StoppableTask):
    """Utility class to use when another task isn't ready yet. An instance of this will always
    wrap another task, contained in the "task" attribute. Upon the next tick the wrapped task
    will be activated."""

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.task_id = ""

    def setup(self, task_id):
        self.task_id = task_id

    def tick(self):
        # Reactive the initial task, which by now should be available
        return server.OPERATION_BLOCKED, self.usage.actor.start_task(self.task_id, self.task)

import server
from atlas import Entity
from atlas import Operation

from world.PendingTask import PendingTask


def set_cooldown_on_attached(tool, actor):
    """Updated the "_ready_at_attached" property with a new timestamp obtained from the "cooldown" property of the tool."""
    cooldown = tool.props.cooldown
    if cooldown and cooldown > 0.0:

        # Get the attach point name from the attached tool
        mode_data = tool.props.mode_data
        if mode_data is not None and mode_data["mode"] == "planted":
            if mode_data["attachment"]:
                attachment_name = mode_data["attachment"]
                ready_at_attached_prop = actor.props._ready_at_attached
                if not ready_at_attached_prop:
                    ready_at_attached_prop = {}

                ready_at_attached_prop[attachment_name] = server.world.get_time() + cooldown
                actor.send_world(Operation('set',
                                           Entity(actor.id, _ready_at_attached=ready_at_attached_prop), to=actor.id))
    return cooldown


def set_cooldown_on_tool(tool):
    """Sets the "ready_at" property of the supplied tool to a new value calculated from it's "cooldown" property, if it exists."""
    cooldown = tool.props.cooldown
    if cooldown and cooldown > 0.0:
        tool.send_world(Operation('set', Entity(tool.id, ready_at=server.world.get_time() + cooldown), to=tool.id))


def delay_task_if_needed(task):
    """Will check if the supplied task can instantly be activated given the tool used, or if it should be wrapped in a PendingTask.
    Either the original task, or a PendingTask instance will be returned.
    Use this whenever you want to start a task, and want to honour the cooldown properties.
    """
    # Check that the tool is ready
    tool_ready_at = task.usage.tool.get_prop_float("ready_at")
    if tool_ready_at and tool_ready_at > server.world.get_time():
        return PendingTask(task.usage, name=task.name, tick_interval=tool_ready_at - server.world.get_time(), task=task)
    else:
        # First just check if we're using a tool
        if task.usage.tool != task.usage.actor:
            # Check if the tools is attached, and if so the attachment is ready
            actor_ready_at = task.usage.actor.get_prop_map("_ready_at_attached")
            if actor_ready_at:
                mode_data = task.usage.tool.get_prop_map("mode_data")
                if mode_data:
                    if mode_data["mode"] == "planted":
                        attachment = mode_data["attachment"]
                        if attachment and attachment in actor_ready_at:
                            attachment_ready_at = actor_ready_at[attachment]
                            if attachment_ready_at > server.world.get_time():
                                return PendingTask(task.usage, name=task.name, tick_interval=attachment_ready_at - server.world.get_time(), task=task)
    return task

from atlas import Operation
from atlas import Entity
import server

def set_cooldown_on_attached(tool, actor):
    cooldown = tool.props.cooldown
    if cooldown and cooldown > 0.0:

        #Get the attach point name from the attached tool
        if tool.props.planted_on:
            planted_on_data = tool.props.planted_on
            if planted_on_data["attachment"]:
                attachment_name = planted_on_data["attachment"]
                ready_at_attached_prop = actor.props._ready_at_attached
                if not ready_at_attached_prop:
                    ready_at_attached_prop = {}

                ready_at_attached_prop[attachment_name] = server.world.get_time() + cooldown
                actor.send_world(Operation('set', Entity(actor.id, _ready_at_attached=ready_at_attached_prop), to=actor.id))


def set_cooldown_on_tool(tool):
    cooldown = tool.props.cooldown
    if cooldown and cooldown > 0.0:
        tool.send_world(Operation('set', Entity(tool.id, ready_at=server.world.get_time() + cooldown), to=tool.id))

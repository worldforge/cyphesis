from atlas import Operation
from atlas import Entity
import server

def set_cooldown_on_attached(tool, actor):
    cooldown = tool.props.cooldown
    if cooldown and cooldown > 0.0:
        ready_at_attached_prop = actor.props._ready_at_attached
        if not ready_at_attached_prop:
            ready_at_attached_prop = {}

        ready_at_attached_prop["right_hand_wield"] = server.world.get_time() + cooldown
        actor.send_world(Operation('set', Entity(actor.id, _ready_at_attached=ready_at_attached_prop), to=actor.id))


def set_cooldown_on_tool(tool):
    cooldown = tool.props.cooldown
    if cooldown and cooldown > 0.0:
        tool.send_world(Operation('set', Entity(tool.id, ready_at=server.world.get_time() + cooldown), to=tool.id))

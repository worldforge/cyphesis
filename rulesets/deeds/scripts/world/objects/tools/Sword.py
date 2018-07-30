from atlas import *

import server

def strike(tool, actor, op, targets, consumed):
    # Check that we can reach the target with our weapon
    extraReach = 0.0
    if tool.props.reach:
        extraReach = tool.props.reach

    # If there's a cooldown we need to mark the actor
    cooldown = tool.props.cooldown
    if cooldown and cooldown > 0.0:
        ready_at_attached_prop = actor.props._ready_at_attached
        if not ready_at_attached_prop:
            ready_at_attached_prop = {}

        ready_at_attached_prop["right_hand_wield"] = server.world.get_time() + cooldown
        actor.send_world(Operation('set', Entity(actor.id, _ready_at_attached=ready_at_attached_prop), to=actor.id))

    # Sword only handles one target
    target = targets[0]
    # Ignore pos
    if target.entity.is_reachable_for_other_entity(actor, None, extraReach):
        print("Strike!")
        damage = 0
        if tool.props.damage:
            damage = tool.props.damage
        hitOp = Operation('hit', Entity(damage=damage), to=target.entity)
        return (server.OPERATION_BLOCKED, hitOp, Operation('sight', hitOp))
    else:
        print("Too far away.")
        return (server.OPERATION_BLOCKED)

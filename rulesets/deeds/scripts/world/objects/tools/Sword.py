from atlas import *
import server

def strike(tool, actor, op, targets, consumed):

    #Check that we can reach the target with our weapon
    extraReach = 0.0
    if tool.props.reach:
        extraReach = tool.props.reach

    #Sword only handles one target
    target = targets[0]
    if target.is_reachable_for_other_entity(actor, None, extraReach):
        print("Strike!")
        damage = 0
        if tool.props.damage:
            damage = tool.props.damage
        hitOp = Operation('hit', Entity(damage=damage), to=target)
        return (server.OPERATION_BLOCKED, hitOp, Operation('sight', hitOp))
    else:
        print("Too far away.")
        return (server.OPERATION_BLOCKED)

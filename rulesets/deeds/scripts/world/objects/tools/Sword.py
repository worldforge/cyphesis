from atlas import *
import server

def strike(from_, op, targets, consumed):
    print("Strike!")
    return (server.OPERATION_HANDLED, Operation('hit', Entity(damage=10), to=targets[0]))

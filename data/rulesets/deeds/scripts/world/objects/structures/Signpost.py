import server
from atlas import Operation, Entity


def read(instance):
    op = Operation("imaginary",
                   Entity(description="The signs says:\"{}\"".format(instance.tool.props.message)),
                   to=instance.actor.id, from_=instance.actor.id)
    return server.OPERATION_HANDLED, op

from atlas import *
import server

class Hittable(server.Thing):

    def hit_operation(self, op):
        print("Hit!")

        return (server.OPERATION_HANDLED, Operation("set", Entity(self.id, status=self.props.status - 0.1), to=self.id))

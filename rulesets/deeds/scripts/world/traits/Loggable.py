from atlas import *
import server

#Turn the entity into lumber when it's destroyed.
class Loggable(server.Thing):

    def delete_operation(self, op):
        create_loc = self.location.copy()
        create = Operation("create",
                           Entity(parent="lumber",
                                  mass = self.props.mass,
                                  location = create_loc,
                                  #bbox = self.target().bbox,
                                  mode = "free"),
                           to = self.location.parent)
        #Send through parent since this entity will be destroyed once the op is handled.
        self.location.parent.send_world(create)

        return (server.OPERATION_HANDLED)

import entity_filter
import server
from atlas import Operation, Entity, Oplist

from world.StoppableTask import StoppableTask

ingot_filter = entity_filter.Filter("entity instance_of types.iron_ingot")


def craft(instance):
    task = Craft(instance, duration=5, tick_interval=1, name="Crafting")

    instance.actor.send_world(Operation("sight", instance.op))

    return server.OPERATION_BLOCKED, instance.actor.start_task("craft", task)


class Craft(StoppableTask):
    """ Craft an item """

    def setup(self, task_id):
        """ Setup code, could do something """
        pass

    def tick(self):
        (valid, err) = self.usage.is_valid()
        if not valid:
            return self.irrelevant(err)

    def completed(self):
        res = Oplist()
        ingots = self.usage.actor.find_in_contains(ingot_filter)
        if len(ingots):
            res.append(Operation("delete", Entity(ingots[0].id), to=ingots[0]))

            new_entity = Entity(parent="sword_a1",
                                loc=self.usage.actor.id)
            res.append(Operation("create", new_entity, to=self.usage.tool.id))
            res.append(Operation("imaginary", Entity(description="You successfully create a sword."),
                                 to=self.usage.actor.id, from_=self.usage.actor.id))
            return res

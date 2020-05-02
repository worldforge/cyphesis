import entity_filter
import server
from atlas import Operation, Entity, Oplist

from world.StoppableTask import StoppableTask

#Allow both charcoal and lumber to feed the fire. Charcoal is preferable since it weights less (in the future we could also only allow charcoal, on the basis that it allows higher temperatures).
charcoal_filter = entity_filter.Filter("entity instance_of types.charcoal or entity instance_of types.lumber")
hematite_filter = entity_filter.Filter("entity instance_of types.hematite")


def craft(instance):
    task = Smelt(instance, duration=5, tick_interval=1, name="Smelting")

    instance.actor.send_world(Operation("sight", instance.op))

    return server.OPERATION_BLOCKED, instance.actor.start_task("smelt", task)


class Smelt(StoppableTask):
    """ Smelting iron from iron ore """

    def setup(self, task_id):
        """ Setup code, could do something """
        pass

    def tick(self):
        (valid, err) = self.usage.is_valid()
        if not valid:
            return self.irrelevant(err)

    def completed(self):
        res = Oplist()
        # Find one charcoal and one hematite amongst the children and delete them, and create one iron ingot.
        hematites = self.usage.tool.find_in_contains(hematite_filter)
        charcoals = self.usage.tool.find_in_contains(charcoal_filter)
        if len(hematites) and len(charcoals):
            res.append(Operation("delete", Entity(hematites[0].id), to=hematites[0]))
            res.append(Operation("delete", Entity(charcoals[0].id), to=charcoals[0]))

            new_entity = Entity(parent="iron_ingot",
                                loc=self.usage.tool.id)
            res.append(Operation("create", new_entity, to=self.usage.tool.id))
            res.append(Operation("imaginary", Entity(description="You successfully refine the ore."),
                                 to=self.usage.actor.id, from_=self.usage.actor.id))
            return res

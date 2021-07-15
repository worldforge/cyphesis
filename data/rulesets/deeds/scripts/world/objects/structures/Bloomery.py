import entity_filter
import server
from atlas import Operation, Entity, Oplist

from world.StoppableTask import StoppableTask

# Allow both charcoal and lumber to feed the fire. Charcoal is preferable since it weights less
# (in the future we could also only allow charcoal, on the basis that it allows higher temperatures).
charcoal_filter = entity_filter.Filter("entity instance_of types.charcoal or entity instance_of types.lumber")
ores = {"iron_ingot": entity_filter.Filter("entity instance_of types.hematite"),
        "copper_ingot": entity_filter.Filter("entity instance_of types.copper_ore")}


def craft(instance):
    task = Smelt(instance, duration=5, tick_interval=1, name="Smelting")

    instance.actor.send_world(Operation("sight", instance.op))

    return server.OPERATION_BLOCKED, instance.actor.start_task("smelt", task)


class Smelt(StoppableTask):
    """ Smelting iron from iron ore """

    def setup(self, task_id):
        # TODO: match with animation in client
        self.start_action("smelting")

    def tick(self):
        (valid, err) = self.usage.is_valid()
        if not valid:
            return self.irrelevant(err)

    def completed(self):
        res = Oplist()
        # Find one charcoal and one ore amongst the children and delete them, and create one iron ingot.
        charcoals = self.usage.tool.find_in_contains(charcoal_filter)
        if len(charcoals):
            # We have charcoal, lets see if we also have something to smelt. We'll go through the map of ores to find a match.
            for key, value in ores.items():
                inputs = self.usage.tool.find_in_contains(value)
                if len(inputs) > 0:
                    res.append(Operation("delete", Entity(inputs[0].id), to=inputs[0]))
                    res.append(Operation("delete", Entity(charcoals[0].id), to=charcoals[0]))

                    new_entity = Entity(parent=key, loc=self.usage.tool.id)
                    res.append(Operation("create", new_entity, to=self.usage.tool.id))
                    res.append(Operation("imaginary", Entity(description="You successfully refine the ore."),
                                         to=self.usage.actor.id, from_=self.usage.actor.id))

        return res

import entity_filter
import server
from atlas import Operation, Entity, Oplist

from world.StoppableTask import StoppableTask


def general_crafting(instance):
    """
    A general crafting mechanism. The behavior of the crafting is determined by the properties of the tool.
    Possible props:
    * craft_duration:   In seconds how long the crafting takes. Defaults to 5.0.
    * craft_input:      A list of the ingredients needed to be present on the actor, which will be consumed.
                            Each entry is a map containing a required Entity Filter as a "constraint" string, and an optional "amount" (defaults to 1).
    * craft_output:     A list of entity declarations which will be created as a result of the crafting.
    * craft_name:       An optional name of the thing(s) created. To be emitted as an Imaginary.
    """
    duration = instance.tool.get_prop_float("craft_duration", 5.0)

    task = Craft(instance, duration=duration, tick_interval=min(1, duration), name="Crafting")

    (valid, err) = task.is_valid()
    if not valid:
        return server.OPERATION_BLOCKED, task.irrelevant(err)

    instance.actor.send_world(Operation("sight", instance.op))

    return server.OPERATION_BLOCKED, instance.actor.start_task("craft", task)


class Craft(StoppableTask):
    """ Craft an item """

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)

        self.temporaries["inputs"] = []
        self.temporaries["outputs"] = []

        inputs_list = self.usage.tool.get_prop_list("craft_input")

        if inputs_list:
            for input_def in inputs_list:
                amount = 1
                if "amount" in input_def:
                    amount = input_def["amount"]
                criteria = entity_filter.Filter(input_def["criteria"])
                input_def = {"criteria": criteria, "amount": amount}
                self.temporaries["inputs"].append(input_def)

        self.temporaries["outputs"] = self.usage.tool.get_prop_list("craft_output")

    def setup(self, task_id):
        # TODO: match with animation in client
        self.start_action("crafting")

    def tick(self):
        (valid, err) = self.usage.is_valid()
        if not valid:
            return self.irrelevant(err)
        if not self.is_valid():
            return self.irrelevant()

    def completed(self):
        res = Oplist()
        valid, err = self.is_valid()
        if valid:
            for input_def in self.temporaries["inputs"]:
                input_entities = self.usage.actor.find_in_contains(input_def["criteria"])
                needed_amount = input_def["amount"]
                for entity in input_entities:
                    entity_amount = entity.get_prop_int("amount", 1)
                    amount_to_delete = min(needed_amount, entity_amount)
                    res.append(Operation("delete", Entity(entity.id, amount=amount_to_delete), to=entity))
                    needed_amount = needed_amount - amount_to_delete
                    if needed_amount <= 0:
                        break
            for output_def in self.temporaries["outputs"]:
                res.append(Operation("create", output_def, to=self.usage.tool.id))

            craft_name = self.usage.tool.get_prop_string("craft_name", "tool")
            res.append(Operation("imaginary", Entity(description="You successfully create '{}'.".format(craft_name)),
                                 to=self.usage.actor.id, from_=self.usage.actor.id))
            return res

    def is_valid(self):
        for input_def in self.temporaries["inputs"]:
            input_entities = self.usage.actor.find_in_contains(input_def["criteria"])
            found_amount = 0
            for entity in input_entities:
                found_amount = found_amount + entity.get_prop_int("amount", 1)
                if found_amount >= input_def["amount"]:
                    continue
            if found_amount < input_def["amount"]:
                return False, "You are missing all required items for crafting"
        return True, None

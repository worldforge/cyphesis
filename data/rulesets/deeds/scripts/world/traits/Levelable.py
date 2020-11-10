import math

import server
from atlas import Operation, Entity


class Levelable(server.Thing):
    """Attached to entities that increase in level as their "xp" increases."""

    def xp_property_update(self):
        xp = self.get_prop_float("xp")
        if xp is not None:
            level_from_xp = int(math.floor(xp / 100))
            level = self.get_prop_int("level")
            if level is None or level < level_from_xp:
                return Operation("set", Entity(self.id, level=level_from_xp), to=self.id), \
                       Operation("imaginary", Entity(description="You are now at level {}!".format(level_from_xp)), to=self.id, from_=self.id)


class Levelled(server.Thing):
    """Attached to entities which have improved stats due to their level.

    The property "levelled_modifiers" is used to alter properties. This property is a map where each key defines the affected property
    Each value is a map containing the modifiers to apply. Each modifier will have it's value multiplied with the

    For example:
    <map name="damage">
        <float name="add_fraction">0.1</float>
    </map>

    If the entity had a level of 6 this would add an add_fraction modifier of 0.6 to the entity.

    """

    def level_property_update(self):
        level = self.get_prop_int("level")
        if level is not None:
            ent = Entity(self.id)
            levelled_modifiers = self.get_prop_map("levelled_modifiers")
            if level > 0 and levelled_modifiers:
                modifiers = {}
                for prop, modifierDef in levelled_modifiers.items():
                    property_modifiers = {}
                    for modifierName, value in modifierDef.items():
                        property_modifiers[modifierName] = value * level
                    modifiers[prop] = property_modifiers

                ent["modify_self!append"] = {
                    "trait_levelled": {
                        "modifiers": modifiers
                    }
                }
            else:
                # Reset any modifiers
                ent["modify_self!subtract"] = {
                    "trait_levelled": {}
                }
            return Operation("set", ent, to=self.id)

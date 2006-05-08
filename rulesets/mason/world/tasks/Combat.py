#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 2005-2006 Al Riddoch (See the file COPYING for details).

from atlas import *
from physics import *
from Quaternion import Quaternion
from Vector3D import Vector3D

try:
  from random import *
except ImportError:
  from whrandom import *

from world.objects.Thing import Thing

class Combat(Thing):
    """A very simple combat system example."""
    def attack_operation(self, op):
        """ The attack op is FROM the the character that initiated combat which
            we term the attacker, TO the character that is attacker which we
            term the defender. We store the IDs of both. """
        # Check if the attacked characters stamina is too low for combat
        if self.character.stamina < 0.1:
            # print "Aborting defender stamina low"
            self.irrelevant()
            return
        assert(op.from_ != op.to)
        if op.to != self.character.id:
            self.oponent = op.to
            # print "Attack operation is not to this character"
            # We have initiative
        else:
            self.oponent = op.from_
            # print "Attack operation is to this character"
            self.surprise = True
            # We do not have initiative
        # Attach this task to the attacker. Its already implicitly attached
        # to the defender who owns this task.
        a=self.character.world.get_object(self.oponent)
        # Check if the attacking characters stamina is too low for combat
        if not a or a.stamina < 0.1:
            self.irrelevant()
            return
        # a.set_task(self.cppthing)
        self.square_range = 25
    def tick_operation(self, op):
        """ This method is called repeatedly, each time a combat turn occurs.
            In this example the interval is fixed, but it can be varied.
            self.attacker is the ID of the character that initiated the combat
            self.defender is the ID of the character that was initially
            attacked The self.attack flag is used to alternate the attack from
            one combatant to the other. """
        # if self.count() < 2:
            # print "Someone has dropped out"
            # self.irrelevant()
            # return

        assert(self.character.id == op.to)

        if self.character.stamina <= 0:
            # print "I am exhausted"
            self.irrelevant()
            return

        attacker = self.character
        if not attacker:
            sys.stderr.write("Attacker owning combat task destroyed, but task still running")
            self.irrelevant()
            return

        if attacker.stamina <= 0:
            # print "Attacker exhausted"
            self.irrelevant()
            return

        defender = self.character.world.get_object(self.oponent)
        if not defender:
            # print "No defender"
            self.irrelevant()
            return

        if hasattr(self, 'surprise') and self.surprise:
            # print 'Surprised!'
            self.surprise = False
            tick=Operation("tick", Entity(name="task",serialno=self.new_tick()), to=op.to)
            tick.setFutureSeconds(0.75 + uniform(0,0.25))
            return tick

        if square_distance(self.character.location, defender.location) > self.square_range:
            tick=Operation("tick", Entity(name="task",serialno=self.new_tick()), to=op.to)
            tick.setFutureSeconds(1.75 + uniform(0,0.25))
            return tick

        a=self.character.id
        d=self.oponent

        # A very simple formula is used to determine the damage done
        damage = (attacker.statistics.attack / defender.statistics.defence) / uniform(2,10)
        # Damage is counted against stamina, to ensure combat is non lethal,
        # and make recovery easier.
        stamina=defender.stamina-damage
        if stamina<0: stamina=0
        set_arg=Entity(self.oponent, stamina=stamina)

        # We send 3 operations to indicate what is going on. The imginary ops
        # provide emotes for the actions. The sight(attack) operation
        # indicates that a singleshot animation of attacking should be
        # triggered on the attacker.
        attacker.send_world(Operation("imaginary", Entity(description="hits for massive damage."), to=attacker))
        attacker.send_world(Operation("sight", Operation("attack", to=d, from_=a)))
        defender.send_world(Operation("imaginary", Entity(description="defends skillfully."), to=defender))

        # If the defenders stamina has reached zero, combat is over, and emotes
        # are sent to indicate this.
        if stamina <= 0:
            set_arg.status = defender.status - 0.1
            defender.send_world(Operation("imaginary", Entity(description="has been defeated"), to=defender))
            defender.send_world(Operation("sight", Operation("collapse", from_=d)))
            attacker.send_world(Operation("imaginary", Entity(description="is victorious"), to=attacker))
            self.irrelevant()

        res=Message()
        # This set op makes the change to defenders stamina, and a small health
        # change if they have been defeated
        res.append(Operation("set", set_arg, to=defender))

        # Turn the attacker to face the defender. This has to go through
        # the mind2body interface, so it does not interrupt what the
        # the character is doing.
        faceop=self.face(defender)
        if faceop:
            faceop=attacker.mind2body(faceop)
            if faceop:
                res.append(faceop)

        # Don't return the following tick op if this task is now complete
        if self.obsolete():
            return res

        # Schedule a new tick op
        tick=Operation("tick", Entity(name="task",serialno=self.new_tick()), to=op.to)
        tick.setFutureSeconds(1.75 + uniform(0,0.25))
        res.append(tick)
        return res
    def face(self, other):
        """ Turn to face that another character, ensuring that
            we are facing the character we are hitting """
        vector = distance_to(self.character.location, other.location)
        vector.z = 0
        if vector.square_mag() < 0.1:
            return
        vector = vector.unit_vector()
        newloc = Location(self.character.location.parent)
        newloc.orientation = Quaternion(Vector3D(1,0,0), vector)
        return Operation("move", Entity(self.character.id, location=newloc))

# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 1999 Aloril (See the file COPYING for details).

import random
import sys
import traceback
import importlib

from atlas import *
from physics import *
from physics import Quaternion
from common import const
from types import *
from rules import Location

from physics import Vector3D

import ai

from mind.Memory import Memory
from mind.Knowledge import Knowledge
from mind.panlingua import interlinguish, ontology
from mind.compass import vector_to_compass
from common import log, const
from . import dictlist
import mind.goals
import mind.goals.common

reverse_cmp = {'>': '<'}


def get_dict_func(self, func_str, func_undefined):
    """get method by name from instance or return default handler"""
    try:
        func = getattr(self, func_str)
    except AttributeError:
        func = func_undefined
    return func


class NPCMind(ai.Mind):
    """Mind class for most mobile entities in the game.

    An NPCMind object is associated with all NPC and similar entities on a
    game server. It handles perception data from the world, tracks what
    the NPC knows about, and handles its goals.

    The data is organized into three key data structures:

    self.map is handled by the underlying C++ code, and contains a copy of
    all the entities in the world that this NPC is currently able to perceive.

    self.knowledge contains data triples which define relations between
    entities.

    self.goals and self.trigger_goals contain trees of goals which represent
    current and potential activities that NPC might engage in. self.goals are
    goals which are checked each tick, self.trigger_goals are goals which
    are activated by an event."""

    ########## Initialization
    def __init__(self, cppthing):
        # FIXME: this shouldn't be needed
        self.mind = cppthing

        self.print_debug('init')

        self.knowledge = Knowledge()
        self.mem = Memory(map=self.map)
        self.things = {}
        self.pending_things = []
        self._reverse_knowledge()
        self.goals = []
        self.money_transfers = []
        self.transfers = []
        self.trigger_goals = {}
        self.jitter = random.uniform(-0.1, 0.1)
        # ???self.debug=debug(self.name+".mind.log")
        self.message_queue = None
        # This is going to be really tricky
        self.map.add_hooks_append("add_map")
        self.map.update_hooks_append("update_map")
        self.map.delete_hooks_append("delete_map")
        self.goal_id_counter = 0
        self.add_property_callback('_goals', 'goals_updated')
        self.add_property_callback('_knowledge', 'knowledge_updated')

    def goals_updated(self, entity):
        self.print_debug('Goals updated.')
        # For now just clear and recreate all goals when _goals changes. We would probably rather only recreate those that have changed though.
        goals = entity.props._goals
        self.goals.clear()
        if goals:
            for goal_element in goals:
                if hasattr(goal_element, 'class'):
                    goal_class = goal_element['class']
                    splits = goal_class.split('.')
                    module_name = '.'.join(splits[0:-1])
                    class_name = splits[-1]

                    module = importlib.import_module(module_name)
                    class_ = getattr(module, class_name)
                    params = {}
                    if hasattr(goal_element, 'params'):
                        params = goal_element['params']

                    self.print_debug('Creating an instance of {}'.format(goal_class))
                    try:
                        instance = class_(**params)

                        if instance:
                            self.insert_goal(instance)
                    except:
                        self.print_debug('Error when creating goal from data\n {}'.format(str(goal_element)))
                        raise

    def knowledge_updated(self, entity):
        self.print_debug('Knowledge updated.')
        if entity.has_prop_map('_knowledge'):
            knowledge = entity.get_prop_map('_knowledge')
            for key, knowledge_element in knowledge.items():
                (predicate, subject) = key.split(':')
                object = knowledge_element

                if predicate == 'location':
                    #If it's just a string it's a reference to an entity id (with zero position).
                    if isinstance(object, str):
                        entity_id_string = object
                        # A prefix of "$eid:" denotes an entity id; it should be stripped first.
                        if entity_id_string.startswith("$eid:"):
                            entity_id_string = entity_id_string[5:]
                        where = self.map.get_add(entity_id_string)
                        object = Location(where)
                    else:
                        if len(object) == 3:
                            loc = self.entity.location.copy()
                            loc.pos = Vector3D(object)
                            object = loc
                        elif len(object == 4):
                            entity_id_string = object[0]
                            # A prefix of "$eid:" denotes an entity id; it should be stripped first.
                            if entity_id_string.startswith("$eid:"):
                                entity_id_string = entity_id_string[5:]
                            where = self.map.get_add(entity_id_string)
                            object = Location(where, Vector3D(object[:3]))

                self.add_knowledge(predicate, subject, object)


    def print_debug(self, message):
        """Prints a debug message using 'print', prepending the message with a description of the entity."""
        print(str(self) + ": " + str(message))

    def find_op_method(self, op_id, prefix="", undefined_op_method=None):
        """find right operation to invoke"""
        if not undefined_op_method: undefined_op_method = self.undefined_op_method
        return get_dict_func(self, prefix + op_id + "_operation", undefined_op_method)

    def undefined_op_method(self, op):
        """this operation is used when no other matching operation is found"""
        pass

    def get_op_name_and_sub(self, op):
        event_name = op.id
        sub_op = op
        # I am not quite sure why this is while, as it's only over true
        # for one iteration.
        while len(sub_op) and sub_op[0].get_name() == "op":
            sub_op = sub_op[0]
            event_name = event_name + "_" + sub_op.id
        return event_name, sub_op

    def is_talk_op_addressed_to_me_or_none(self, op):
        """Checks whether a Talk op is addressed either to none or to me.
           This is useful is we want to avoid replying to queries addressed
           to other entities."""
        talk_entity = op[0]
        if hasattr(talk_entity, "address"):
            addressElement = talk_entity.address
            if len(addressElement) == 0:
                return True
            return self.entity.id in addressElement
        return True

    ########## Map updates
    def add_map(self, obj):
        """Hook called by underlying map code when an entity is added."""
        # print "Map add",obj
        print('See entity ' + str(obj))
        pass

    def update_map(self, obj):
        """Hook called by underlying map code when an entity is updated.

        Fix ownership category for objects owned temporary under 'Foo' type."""
        # print "Map update",obj
        foo_lst = self.things.get('Foo', [])
        for foo in foo_lst[:]:  # us copy in loop, because it might get modified
            print("Oh MY GOD! We have a Foo thing!")
            if foo.id == obj.id:
                self.remove_thing(foo)
                self.add_thing(obj)

    def delete_map(self, obj):
        """Hook called by underlying map code when an entity is deleted."""
        # print "Map delete",obj
        self.remove_thing(obj)

    ########## Operations
    def setup_operation(self, op):
        """called once by world after object has been made
           send first tick operation to object
           
        This method is automatically invoked by the C++ BaseMind code, due to its *_operation name."""
        # CHEAT!: add memory, etc... initialization (or some of it to __init__)

        # Setup a tick operation for thinking
        think_tick_op = Operation("tick")
        think_tick_op.set_to(self.id)
        think_tick_op.set_args([Entity(name="think")])

        # Setup a tick operation for moving
        move_tick_op = Operation("tick")
        think_tick_op.set_to(self.id)
        move_tick_op.set_args([Entity(name="move")])
        move_tick_op.set_future_seconds(0.2)

        # Setup a tick operation for periodical persistence of thoughts to the server
        send_thoughts_tick_op = Operation("tick")
        think_tick_op.set_to(self.id)
        send_thoughts_tick_op.set_args([Entity(name="persistthoughts")])
        send_thoughts_tick_op.set_future_seconds(5)

        return Operation("look") + think_tick_op + move_tick_op + send_thoughts_tick_op

    def tick_operation(self, op):
        """periodically reassess situation
        
        This method is automatically invoked by the C++ BaseMind code, due to its *_operation name.
        """
        args = op.get_args()
        if len(args) != 0:
            if args[0].name == "think":
                # It's a "thinking" op, which is the base of the AI behaviour.
                # At regular intervals the AI needs to assess its goals; this is done through "thinking" ops.
                op_tick = Operation("tick")
                # just copy the args from the previous tick
                op_tick.set_args(args)
                op_tick.set_future_seconds(const.basic_tick + self.jitter)
                op_tick.set_to(self.id)
                for t in self.pending_things:
                    thing = self.map.get(t)
                    if thing and thing.type[0]:
                        self.add_thing(thing)
                self.pending_things = []
                result = self.think()
                if self.message_queue:
                    result = self.message_queue + result
                    self.message_queue = None
                return op_tick + result
            elif args[0].name == "persistthoughts":
                # It's a periodic tick for sending thoughts to the server (so that they can be persisted)
                # TODO: only send thoughts when they have changed.
                op_tick = Operation("tick")
                # just copy the args from the previous tick
                op_tick.set_args(args)
                # Persist the thoughts to the server at 30 second intervals.
                op_tick.set_future_seconds(30)
                op_tick.set_to(self.id)
                result = self.commune_all_thoughts(op, "persistthoughts")
                return op_tick + result

    ########## Sight operations
    def sight_create_operation(self, op):
        """Note our ownership of entities we created.
        
        This method is automatically invoked by the C++ BaseMind code, due to its *_*_operation name."""
        # BaseMind version overridden!
        obj = self.map.add(op[0], op.get_seconds())
        if op.to == self.id:
            self.add_thing(obj)

    def sight_move_operation(self, op):
        """change position in our local map
        
        This method is automatically invoked by the C++ BaseMind code, due to its *_*_operation name."""
        obj = self.map.update(op[0], op.get_seconds())
        if obj.location.parent and obj.location.parent.id == self.entity.id:
            self.add_thing(obj)
            if op.to != self.id:
                self.transfers.append((op.from_, obj.id))
            if obj.type[0] == "coin" and op.from_ != self.id:
                self.money_transfers.append([op.from_, 1])
                return Operation("imaginary", Entity(description="accepts"))

    def think_get_operation(self, op):
        """A Think op wrapping a Get op is used to inquire about the status of a mind.
        It's often sent from authoring clients, as well as the server itself when 
        it wants to persist the thoughts of a mind.
        A Get op without any args means that the mind should dump all its thoughts.
        If there are args however, the meaning of what's to return differs depending on the
        args.
        * If "goal" is specified, a "think" operation only pertaining to goals is returned. The 
        "goal" arg should be a map, where the keys and values are used to specify exactly what goals
        to return. An empty map returns all goals.
        
        This method is automatically invoked by the C++ BaseMind code, due to its *_*_operation name."""

        args = op.get_args()
        # If there are no args we should send all of our thoughts
        if len(args) == 0:
            return self.commune_all_thoughts(op, None)
        else:
            arg_entity = args[0]

            if hasattr(arg_entity, "goal"):
                goal_entity = arg_entity.goal
                return self.commune_goals(op, goal_entity)
            if hasattr(arg_entity, "path"):
                return self.commune_path(op)

            # TODO: allow for finer grained query of specific thoughts

    def commune_path(self, op):
        """Sends back information about the path."""
        think_op = Operation("think")
        path = []
        my_path = self.path
        # self.print_debug("path size: " + str(len(my_path)))
        for point in my_path:
            path.append([point.x, point.y, point.z])

        think_op.set_args([Entity(path=path)])

        res = Oplist()
        res = res + think_op
        return res

    def commune_goals(self, op, goal_entity):
        """Sends back information about goals only."""
        think_op = Operation("think")
        set_op = Operation("set")
        thoughts = []

        # It's important that the order of the goals is retained
        for goal in self.goals:
            goal_string = ""
            if hasattr(goal, "str"):
                goal_string = goal.str
            else:
                goal_string = goal.__class__.__name__

            thoughts.append(Entity(goal=goal_string, id=goal_string))

        for (trigger, goallist) in sorted(self.trigger_goals.items()):
            for goal in goallist:
                goal_string = ""
                if hasattr(goal, "str"):
                    goal_string = goal.str
                else:
                    goal_string = goal.__class__.__name__

                thoughts.append(Entity(goal=goal_string, id=goal_string))

        set_op.set_args(thoughts)
        think_op.set_args([set_op])
        think_op.set_refno(op.get_serialno())
        res = Oplist()
        res = res + think_op
        return res

    def find_goal(self, definition):
        """Searches for a goal, with the specified id"""
        # Goals are either stored in "self.goals" or "self.trigger_goals", so we need
        # to check both
        for goal in self.goals:
            if goal.str == definition:
                return goal
        for (trigger, goallist) in sorted(self.trigger_goals.items()):
            for goal in goallist:
                if goal.str == definition:
                    return goal
        return None

    def think_look_operation(self, op):
        """Sends back information about goals. This is mainly to be used for debugging minds.
        If no arguments are specified all goals will be reported, else a match will be done
        using 'id'.
        The information will be sent back as a Think operation wrapping an Info operation.
        
        This method is automatically invoked by the C++ BaseMind code, due to its *_*_operation name.
        """
        think_op = Operation("think")
        goal_info_op = Operation("info")
        goal_infos = []

        if not op.get_args():
            # get all goals
            for goal in self.goals:
                goal_infos.append(Entity(id=goal.str, report=goal.report()))
            for (trigger, goallist) in sorted(self.trigger_goals.items()):
                for goal in goallist:
                    goal_infos.append(Entity(id=goal.str, report=goal.report()))
        else:
            for arg in op.get_args():
                goal = self.find_goal(arg.id)
                if goal and goal is not None:
                    goal_infos.append(Entity(id=goal.str, report=goal.report()))

        goal_info_op.set_args(goal_infos)
        think_op.set_refno(op.get_serialno())
        think_op.set_args([goal_info_op])
        res = Oplist()
        res = res + think_op
        return res

    def commune_all_thoughts(self, op, name):
        """Sends back information on all thoughts. This includes knowledge and goals, 
        as well as known things.
        The thoughts will be sent back as a "think" operation, wrapping a Set operation, in a manner such that if the
        same think operation is sent back to the mind all thoughts will be restored. In
        this way the mind can support server side persistence of its thoughts.
        A name can optionally be supplied, which will be set on the Set operation.
        """
        think_op = Operation("think")
        set_op = Operation("set")
        thoughts = []

        for what in sorted(self.knowledge.knowings.keys()):
            d = self.knowledge.knowings[what]
            for key in sorted(d):
                if what != "goal":
                    object_val = d[key]
                    if type(object_val) is Location:
                        # Serialize Location as tuple, with parent if available
                        if object_val.parent is None:
                            location = object_val.position
                        else:
                            location = ("$eid:" + object_val.parent.id, object_val.pos)
                        goal_object = str(location)
                    else:
                        goal_object = str(d[key])

                    thoughts.append(Entity(predicate=what, subject=str(key), object=goal_object))

        if len(self.things) > 0:
            things = {}
            for (id, thinglist) in sorted(self.things.items()):
                idlist = []
                for thing in thinglist:
                    idlist.append(thing.id)
                things[id] = idlist
            thoughts.append(Entity(things=things))

        if len(self.pending_things) > 0:
            thoughts.append(Entity(pending_things=self.pending_things))

        set_op.set_args(thoughts)
        think_op.set_args([set_op])
        if not op.is_default_serialno():
            think_op.set_refno(op.get_serialno())
        if name:
            set_op.set_name(name)
        res = Oplist()
        res = res + think_op
        return res

    def think_set_operation(self, op):
        """Sets a new thought, or updates an existing one
        
        This method is automatically invoked by the C++ BaseMind code, due to its *_*_operation name."""

        # If the Set op has the name "peristthoughts" it's a Set op sent to ourselves meant for the server
        # (so it can persist the thoughts in the database). We should ignore it.
        if op.get_name() == "persistthoughts":
            return

        args = op.get_args()
        for thought in args:
            # Check if there's a 'predicate' set; if so handle it as knowledge.
            # Else check if it's things that we know we own or ought to own.
            if hasattr(thought, "predicate") == False:
                if hasattr(thought, "things"):
                    things = thought.things
                    for (id, thinglist) in list(things.items()):
                        # We can't iterate directly over the list, as it's of type atlas.Message; we must first "pythonize" it.
                        # This should be reworked into a better way.
                        thinglist = thinglist.pythonize()
                        for thingId in thinglist:
                            thingId = str(thingId)
                            thing = self.map.get(thingId)
                            if thing and thing.type[0]:
                                self.add_thing(thing)
                            else:
                                self.pending_things.append(thingId)
                elif hasattr(thought, "pending_things"):
                    for id in thought.pending_things:
                        self.pending_things.append(str(id))

            else:
                subject = thought.subject
                predicate = thought.predicate
                object = thought.object

                # Handle locations.
                if len(object) > 0 and object[0] == '(':
                    # CHEAT!: remove eval
                    locdata = eval(object)
                    # If only coords are supplied, it's handled as a location within the same parent space as ourselves
                    if (len(locdata) == 3):
                        loc = self.entity.location.copy()
                        loc.pos = Vector3D(list(locdata))
                    elif (len(locdata) == 2):
                        entity_id_string = locdata[0]
                        # A prefix of "$eid:" denotes an entity id; it should be stripped first.
                        if entity_id_string.startswith("$eid:"):
                            entity_id_string = entity_id_string[5:]
                        where = self.map.get_add(entity_id_string)
                        coords = Point3D(list(locdata[1]))
                        loc = Location(where, coords)
                    self.add_knowledge(predicate, subject, loc)
                else:
                    self.add_knowledge(predicate, subject, object)

    ########## Talk operations
    def admin_sound(self, op):
        assert (op.from_ == op.to)
        return op.from_ == self.entity.id

    def interlinguish_warning(self, op, say, msg):
        log.debug(1, str(self.entity.id) + " interlinguish_warning: " + str(msg) + \
                  ": " + str(say[0].lexlink.id[1:]), op)

    def interlinguish_desire_verb3_buy_verb1_operation(self, op, say):
        """Handle a sentence of the form 'I would like to buy a ....'

        Check if we have any of the type of thing the other character is
        interested in, and whether we know what price to sell at. If so
        set up the transaction goal, which offers to sell it."""
        object = say[1].word
        thing = self.things.get(object)
        if thing:
            price = self.get_knowledge("price", object)
            if not price:
                return
            goal = mind.goals.common.misc_goal.transaction(object, op.to, price)
            who = self.map.get(op.to)
            self.goals.insert(0, goal)
            return Operation("talk", Entity(
                say=self.thing_name(who) + " one " + object + " will be " + str(price) + " coins")) + self.face(who)

    def interlinguish_desire_verb3_operation(self, op, say):
        """Handle a sentence of the form 'I would like to ...'"""
        object = say[2:]
        verb = interlinguish.get_verb(object)
        operation_method = self.find_op_method(verb, "interlinguish_desire_verb3_",
                                               self.interlinguish_undefined_operation)
        res = Oplist()
        res = res + self.call_interlinguish_triggers(verb, "interlinguish_desire_verb3_", op, object)
        res = res + operation_method(op, object)
        return res

    def interlinguish_be_verb1_operation(self, op, say):
        """Handle sentences of the form '... is more important that ...'

        Accept instructions about the priority of goals relative to each
        based on key verbs associated with those goals."""
        if not self.admin_sound(op):
            return self.interlinguish_warning(op, say, "You are not admin")
        res = interlinguish.match_importance(say)
        if res:
            return self.add_importance(res['sub'].id, '>', res['obj'].id)
        else:
            return self.interlinguish_warning(op, say, "Unknown assertion")

    def interlinguish_know_verb1_operation(self, op, say):
        """Handle a sentence of the form 'know subject predicate object'

        Accept admin instruction about knowledge, and store the triple
        in our knowledge base."""
        if not self.admin_sound(op):
            return self.interlinguish_warning(op, say, "You are not admin")
        subject = say[1].word
        predicate = say[2].word
        object = say[3].word
        ##        print "know:",subject,predicate,object
        if object[0] == '(':
            # CHEAT!: remove eval
            xyz = list(eval(object))
            loc = self.entity.location.copy()
            loc.pos = Vector3D(xyz)
            self.add_knowledge(predicate, subject, loc)
        else:
            self.add_knowledge(predicate, subject, object)

    def interlinguish_tell_verb1_operation(self, op, say):
        """Handle a sentence of the form 'Tell (me) ....'

        Accept queries about what we know. Mostly this is for debugging
        and for the time being it is useful to answer these queries no matter
        who hasks."""

        # Ignore messages addressed to others
        if not self.is_talk_op_addressed_to_me_or_none(op):
            return None

        # Currently no checking for trus here.
        # We are being liberal with interpretation of "subject" and "object"
        subject = say[1].word
        predicate = say[2].word
        object = say[3].word
        k = self.get_knowledge(predicate, object)
        if k == None:
            pass
        # return Operation('talk',Entity(say="I know nothing about the "+predicate+" of "+object))
        else:
            k_type = type(k)
            if k_type == type(Location()):
                dist = distance_to(self.entity.location, k)
                dist.y = 0
                distmag = dist.mag()
                if distmag < 8:
                    k = 'right here'
                else:
                    # Currently this assumes dist is relative to TLVE
                    k = '%f metres %s' % (distmag, vector_to_compass(dist))
            elif k_type != str:
                k = 'difficult to explain'
            elif predicate == 'about':
                return self.face_and_address(op.to, k)
            return self.face_and_address(op.to, "The " + predicate + " of " +
                                         object + " is " + k)

    def interlinguish_list_verb1_operation(self, op, say):
        """Handle a sentence of the form 'List (me) ....'

        Accept queries about what we know. Mostly this is for debugging
        and for the time being it is useful to answer these queries no matter
        who asks.
        Querying for "all knowledge" will list all knowledge.
        """

        # Ignore messages addressed to others
        if not self.is_talk_op_addressed_to_me_or_none(op):
            return None

        # Currently no checking for trus here.
        # We are being liberal with interpretation of "subject" and "object"
        subject = say[1].word
        predicate = say[2].word
        if predicate == 'all knowledge':
            res = Oplist()
            res = res + self.face(self.map.get(op.to))
            for attr in dir(self.knowledge.knowings):
                d = self.knowledge.knowings[attr]
                for key in d:
                    # print attr + " of "+key+": " +str(d[key])
                    res = res + self.address(op.to, "The " + attr + " of " +
                                             key + " is " + str(d[key]))
            return res
        else:
            d = self.knowledge.get(predicate)
            if len(d) == 0:
                return None
            res = Oplist()
            res = res + self.face(self.map.get(op.to))
            for key in d:
                res = res + self.address(op.to, "The " + predicate + " of " +
                                         key + " is " + str(d[key]))
            return res

    def interlinguish_own_verb1_operation(self, op, say):
        """Handle a sentence of the form ' own ...'

        Sentences of this form from the admin inform us that we own an
        entity. This is essential when an entity needs to be used as a
        tool, or raw material."""
        if not self.admin_sound(op):
            return self.interlinguish_warning(op, say, "You are not admin")
        ##         print self,"own:",say[1].word,say[2].word
        subject = self.map.get_add(say[1].word)
        ##         print "subject found:",subject
        object = self.map.get_add(say[2].word)
        ##         print "object found:",object
        ##         if subject.id==self.entity.id:
        ##             foo
        if subject.id == self.entity.id:
            self.add_thing(object)

    def interlinguish_undefined_operation(self, op, say):
        # CHEAT!: any way to handle these?
        log.debug(2, str(self.entity.id) + " interlinguish_undefined_operation:", op)
        log.debug(2, str(say))

    ########## Sound operations
    def sound_talk_operation(self, op):
        """Handle the sound of a talk operation from another character.

        The spoken sentence comes in as a sentence string, which
        is converted into a structure representation by the interlinguish
        code. Embedded in the structure is the interlinguish string which
        is then used to call methods and activate triggers, such as
        dynamic goals."""

        talk_entity = op[0]
        say = interlinguish.convert_english_to_interlinguish(self, talk_entity)
        if say:
            verb = interlinguish.get_verb(say)
            operation_method = self.find_op_method(verb, "interlinguish_",
                                                   self.interlinguish_undefined_operation)
            res = self.call_interlinguish_triggers(verb, "interlinguish_", op, say)
            res2 = operation_method(op, say)
            if res:
                res += res2
            else:
                res = res2
            return res

    ########## Other operations
    def call_interlinguish_triggers(self, verb, prefix, op, say):
        """Call trigger goals that have registered a trigger string that
        matches the current interlinguish string.

        Given an interlinguish verb string, and a prefix, find any trigger
        goals that should be activated by the combined trigger string, and
        activate them."""
        # FIXME Don't need this call to get_op_name_and_sub, as we don't use
        # the result.
        null_name, sub_op = self.get_op_name_and_sub(op)
        event_name = prefix + verb
        reply = Oplist()
        for goal in self.trigger_goals.get(event_name, []):
            reply += goal.event(self, op, say)
        return reply

    def call_triggers_operation(self, op):
        event_name, sub_op = self.get_op_name_and_sub(op)
        reply = Oplist()
        for goal in self.trigger_goals.get(event_name, []):
            reply += goal.event(self, op, sub_op)
        return reply

    ########## Generic knowledge
    def _reverse_knowledge(self):
        """normally location: tell where items reside
           reverse location tells what resides in this spot"""
        self.reverse_knowledge = Knowledge()
        if "location" in self.knowledge.knowings:
            for (k, v) in list(self.knowledge.knowings['location'].items()):
                self.reverse_knowledge.add("location", v, k)

    def get_reverse_knowledge(self, what, key):
        """get certain reverse knowledge value
           what: what kind of knowledge (location only so far)"""
        return self.reverse_knowledge.get(what, key)

    def get_knowledge(self, what, key):
        """get certain knowledge value
           what: what kind of knowledge (see Knowledge.py for list)"""
        return self.knowledge.get(what, key)

    def add_knowledge(self, what, key, value):
        """add certain type of knowledge"""
        self.knowledge.add(what, key, value)
        # forward thought
        if type(value) == object:
            if what == "goal":
                thought_value = value.info()
            else:
                thought_value = repr(value)
        else:
            thought_value = value
        desc = "%s knowledge about %s is %s" % (what, key, thought_value)
        #        ent = Entity(description=desc, what=what, key=key, value=thought_value)
        #        self.send(Operation("thought",ent))
        if what == "location":
            # and reverse too
            self.reverse_knowledge.add("location", value, key)

    def remove_knowledge(self, what, key):
        """remove certain type of knowledge"""
        self.knowledge.remove(what, key)

    ########## Importance: Knowledge about how things compare in urgency, etc..
    def add_importance(self, sub, cmp, obj):
        """add importance: both a>b and b<a"""
        self.add_knowledge('importance', (sub, obj), cmp)
        self.add_knowledge('importance', (obj, sub), reverse_cmp[cmp])

    def cmp_goal_importance(self, g1, g2):
        """which of goals is more important?
           also handle more generic ones:
           for example if you are comparing breakfast to sleeping
           it will note that having breakfast is a (isa) type of eating"""
        try:
            id1 = g1.key[1]
            id2 = g2.key[1]
        except AttributeError:
            return 1
        l1 = ontology.get_isa(id1)
        l2 = ontology.get_isa(id2)
        for s1 in l1:
            for s2 in l2:
                cmp = self.knowledge.get('importance', (s1.id, s2.id))
                if cmp:
                    return cmp == '>'
        return 1

    ########## things we own
    def thing_name(self, thing):
        if hasattr(thing, 'name'):
            return thing.name
        return thing.type[0]

    ########## things we own
    def add_thing(self, thing):
        """I own this thing"""
        # CHEAT!: this feature not yet supported
        ##         if not thing.location:
        ##             thing.location=self.get_knowledge("location",thing.place)
        log.debug(3, str(self) + " " + str(thing) + " before add_thing: " + str(self.things))
        # thought about owing thing
        name = self.thing_name(thing)
        if not name:
            self.pending_things.append(thing.id)
            return
        #        desc="I own %s." % name
        #        what=thing.as_entity()
        #        ent = Entity(description=desc, what=what)
        #        self.send(Operation("thought",ent))
        dictlist.add_value(self.things, name, thing)
        log.debug(3, "\tafter: " + str(self.things))

    def find_thing(self, thing):
        if str == type(thing):
            # return found list or empty list
            return self.things.get(thing, [])
        found = []
        for t in self.things.get(self.thing_name(thing), []):
            if t == thing: found.append(t)
        return found

    def remove_thing(self, thing):
        """I don't own this anymore (it may not exist)"""
        dictlist.remove_value(self.things, thing)

    ########## goals
    def add_goal(self, str_goal):
        """add goal..."""
        print('Adding goal: ' + str_goal)
        try:
            goal = self.create_goal(str_goal)
        except BaseException as e:
            print(("Error when adding goal: " + str(e)))
            return

        self.insert_goal(goal)
        return goal

    def insert_goal(self, goal, id=None):
        if not id:
            self.goal_id_counter = self.goal_id_counter + 1
            id = str(self.goal_id_counter)

        goal.id = id
        if hasattr(goal, "trigger"):
            dictlist.add_value(self.trigger_goals, goal.trigger(), goal)
            return
        for i in range(len(self.goals) - 1, -1, -1):
            if self.cmp_goal_importance(self.goals[i], goal):
                self.goals.insert(i + 1, goal)
                return
        self.goals.insert(0, goal)

    def update_goal(self, goal, str_goal):
        try:
            new_goal = self.create_goal(goal.key, str_goal)
        except BaseException as e:
            print(("Error when updating goal: " + str(e)))
            return

        new_goal.id = goal.id
        # We need to handle the case where a goal which had a trigger is replaced by one
        # that hasn't, and the opposite
        if hasattr(goal, "trigger"):
            dictlist.remove_value(self.trigger_goals, goal)
            self.insert_goal(new_goal, goal.id)
        else:
            if hasattr(new_goal, "trigger"):
                self.goals.remove(goal)
                self.insert_goal(new_goal, goal.id)
            else:
                index = self.goals.index(goal)
                self.goals[index] = new_goal

    def create_goal(self, str_goal):
        # CHEAT!: remove eval (this and later)
        goal = eval("mind.goals." + str_goal)
        if const.debug_thinking:
            goal.debug = 1
        goal.str = str_goal
        return goal

    def remove_goal(self, goal):
        """Removes a goal."""
        self.print_debug('Removing goal')
        if hasattr(goal, "trigger"):
            dictlist.remove_value(self.trigger_goals, goal)
        else:
            self.goals.remove(goal)

    def fulfill_goals(self, time):
        "see if all goals are fulfilled: if not try to fulfill them"
        for g in self.goals[:]:
            if g.irrelevant:
                self.print_debug('Removing irrelevant goal')
                self.goals.remove(g)
                continue
            # Don't process goals which have had three errors in them.
            # The idea is to allow for some leeway in goal processing, but to punish repeat offenders.
            if g.errors > 3:
                continue
            try:
                res = g.check_goal(self, time)
                if res: return res
            except:
                stacktrace = traceback.format_exc()
                g.errors += 1
                g.lastError = stacktrace
                # If there's an error, print to the log, mark the goal, and continue with the next goal
                # Some goals have a "str" attribute which represents the constructor; if so use that
                if hasattr(g, "str"):
                    goalstring = g.str
                else:
                    goalstring = g.__class__.__name__
                if hasattr(self, "name"):
                    print("Error in NPC with id " + self.entity.id + " of type " + str(
                        self.entity.type) + " and name '" + self.name + "' when checking goal " + goalstring + "\n" + stacktrace)
                else:
                    print("Error in NPC with id " + self.entity.id + " of type " + str(
                        self.entity.type) + " when checking goal " + goalstring + "\n" + stacktrace)
                continue
            # if res!=None: return res

    def teach_children(self, child):
        res = Oplist()
        locations = self.knowledge.get('location')
        for k in list(locations.keys()):
            es = Entity(verb='know', subject=k, object=locations[k])
            res.append(Operation('say', es, to=child))
        places = self.knowledge.get('place')
        for k in list(places.keys()):
            es = Entity(verb='know', subject=k, object=places[k])
            res.append(Operation('say', es, to=child))
        for g in self.goals:
            es = Entity(verb='learn', subject=g.key, object=g.str)
            res.append(Operation('say', es, to=child))
        importances = self.knowledge.get('importance')
        for im in list(importances.keys()):
            cmp = importances[im]
            if cmp == '>':
                s, i = il.importance(im[0], cmp, im[1])
                es = Entity(say=s, interlinguish=i)
                res.append(Operation('say', es, to=child))
        return res

    ########## thinking (needs rewrite)
    def think(self):
        output = self.fulfill_goals(self.time)
        #        if output and const.debug_thinking:
        #            log.thinking(str(self)+" result at "+str(self.time)+": "+output[-1][0].description)
        return output

    ########## communication: here send it locally
    def send(self, op):
        if not self.message_queue:
            self.message_queue = Oplist(op)
        else:
            self.message_queue.append(op)

    ########## turn to face other entity
    def face(self, other):
        vector = distance_to(self.entity.location, other.location)
        vector.y = 0
        if vector.sqr_mag() < 0.1:
            return
        vector = vector.unit_vector()
        newloc = Location(self.entity.location.parent)
        newloc.orientation = Quaternion(Vector3D(0, 0, 1), vector, Vector3D(0, 1, 0))
        return Operation("move", Entity(self.entity.id, location=newloc))

    def address(self, entity_id, message):
        """Creates a new Talk op which is addressed to an entity"""
        return Operation('talk', Entity(say=message, address=[entity_id]))

    def face_and_address(self, entity_id, message):
        """Utility method for generating ops for both letting the NPC face
           as well as address another entity. In most cases this is what you
           want to do when conversing."""
        return self.address(entity_id, message) + \
               self.face(self.map.get(entity_id))

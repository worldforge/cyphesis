# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 1999 Aloril (See the file COPYING for details).

import random
import traceback

import ai
import entity_filter
from atlas import Operation, Entity, Oplist
from common import log, const
from physics import Vector3D, Point3D, Quaternion
from rules import Location

from mind.Goal import goal_create
from mind.Knowledge import Knowledge
from mind.Memory import Memory
from mind.compass import vector_to_compass
from mind.panlingua import interlinguish
from . import dictlist
from .goals.common.misc_goal import Transaction

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

    # Initialization
    def __init__(self, cppthing):
        # FIXME: this shouldn't be needed
        self.mind = cppthing

        # print('init')

        self.knowledge = Knowledge()
        self.mem = Memory(a_map=self.map)
        self.things = {}
        self.pending_things = []
        self._reverse_knowledge()
        self.goals = []
        self.transfers = []
        # A map containing lists of goals which are to be triggered
        self.trigger_goals = {}
        self.entity_appear_goals = set()
        self.jitter = random.uniform(-0.1, 0.1)
        self.message_queue = None
        self.goal_id_counter = 0
        self.relation_rules = []
        self.entities = {}

        # Fill up any existing entities
        existing_entities = self.map.get_all()
        for entity in existing_entities:
            self.entities[entity.id] = entity

        self.add_hook_set("add_map")
        self.delete_hook_set("delete_map")
        self.add_property_callback('_goals', 'goals_updated')
        self.add_property_callback('_knowledge', 'knowledge_updated')
        self.add_property_callback('_relations', 'relations_updated')
        self.add_property_callback('_origin', 'origin_updated')

    def goals_updated(self, entity):
        # For now just clear and recreate all goals when _goals changes. We would probably rather only recreate those that have changed though.
        goals = entity.props['_goals']
        # First clear all goals
        while len(self.goals):
            self.remove_goal(self.goals[0])
        if goals:
            for goal_element in goals:
                goal = goal_create(goal_element)
                self.insert_goal(goal)

    def origin_updated(self, entity):
        origin = entity.get_prop_map("_origin")
        if origin:
            self.add_knowledge("location", "origin", Location(self.map.get_add(origin["$eid"]), Point3D(origin["pos"])))

    def knowledge_updated(self, entity):
        if entity.has_prop_map('_knowledge'):
            knowledge = entity.get_prop_map('_knowledge')

            for key, knowledge_element in knowledge.items():
                (predicate, subject) = key.split(':')
                object_word = knowledge_element

                if predicate == 'location':
                    # If it's just a string it's a reference to an entity id (with zero position).
                    if isinstance(object_word, str):
                        entity_id_string = object_word
                        # A prefix of "$eid:" denotes an entity id; it should be stripped first.
                        if entity_id_string.startswith("$eid:"):
                            entity_id_string = entity_id_string[5:]
                        where = self.map.get_add(entity_id_string)
                        object_word = Location(where)
                    else:
                        if len(object_word) == 3:
                            loc = self.entity.location.copy()
                            loc.pos = Vector3D(object_word)
                            object_word = loc
                        elif len(object_word) == 4:
                            entity_id_string = object_word[0]
                            # A prefix of "$eid:" denotes an entity id; it should be stripped first.
                            if entity_id_string.startswith("$eid:"):
                                entity_id_string = entity_id_string[5:]
                            where = self.map.get_add(entity_id_string)
                            object_word = Location(where, Vector3D(object_word[:3]))

                self.add_knowledge(predicate, subject, object_word)

    def relations_updated(self, entity):
        self.relation_rules.clear()
        if entity.has_prop_list('_relations'):
            relations = entity.get_prop_list('_relations')
            for relation_element in relations:
                rule = {}
                if "filter" in relation_element:
                    rule["filter"] = entity_filter.Filter(relation_element.filter)
                if "disposition" in relation_element:
                    rule["disposition"] = relation_element.disposition
                else:
                    rule["disposition"] = 0

                if "threat" in relation_element:
                    rule["threat"] = relation_element.threat
                else:
                    rule["threat"] = 0

                self.relation_rules.append(rule)

            # update relations for existing entities
            for (_, entity) in self.entities.items():
                self.update_relation_for_entity(entity)

    def find_op_method(self, op_id, prefix="", undefined_op_method=None):
        """find right operation to invoke"""

        if not undefined_op_method:
            undefined_op_method = self.undefined_op_method
        return get_dict_func(self, prefix + op_id + "_operation", undefined_op_method)

    def undefined_op_method(self, op):
        """this operation is used when no other matching operation is found"""
        pass

    def get_op_name_and_sub(self, op):
        event_name = op.parent
        sub_op = op
        # I am not quite sure why this is while, as it's only over true
        # for one iteration.
        while len(sub_op) and sub_op[0].get_name() == "op":
            sub_op = sub_op[0]
            event_name = event_name + "_" + sub_op.parent
        return event_name, sub_op

    def is_talk_op_addressed_to_me_or_none(self, op):
        """Checks whether a Talk op is addressed either to none or to me.
           This is useful is we want to avoid replying to queries addressed
           to other entities."""
        talk_entity = op[0]
        if hasattr(talk_entity, "address"):
            address_element = talk_entity.address
            if len(address_element) == 0:
                return True
            return self.entity.id in address_element
        return True

    def update_relation_for_entity(self, entity):
        """
        Called when new entities appear or are changed.
        The "_relations" rules will be used to calculate the "disposition" and "threat" for the entity.
        """
        disposition = self.map.recall_entity_memory(entity.id, "disposition_base", 0)
        threat = self.map.recall_entity_memory(entity.id, "threat_base", 0)
        for rule in self.relation_rules:
            # If there's no 'filter' the rule applies to all entities
            if "filter" not in rule or self.match_entity(rule["filter"], entity):
                if "disposition" in rule:
                    disposition += rule["disposition"]
                if "threat" in rule:
                    threat += rule["threat"]

        # print("Disposition %s, threat %s for entity %s" % (disposition, threat, entity.describe_entity()))

        self.map.add_entity_memory(entity.id, "disposition", disposition)
        self.map.add_entity_memory(entity.id, "threat", threat)

    # Map updates
    def add_map(self, obj):
        """Hook called by underlying map code when an entity is added.
            This is called when the entity type has been fully resolved
            (so it might in some cases not be exactly when the Mind received the entity information, if the type at that moment wasn't resolved).
        """
        # print('See entity ' + str(obj))
        self.entities[obj.id] = obj

        self.update_relation_for_entity(obj)

        res = Oplist()

        for goal in self.entity_appear_goals:
            op_res = goal.entity_appears(self, obj)
            if op_res:
                res += op_res
        return res

    def delete_map(self, obj):
        """Hook called by underlying map code when an entity is deleted."""
        # print("Removing entity %s" % obj.id)
        self.entities.pop(obj.id)
        self.remove_thing(obj)

    # Operations
    def setup_operation(self, op):
        """called once by world after object has been made
           send first tick operation to object
           
        This method is automatically invoked by the C++ BaseMind code, due to its *_operation name."""

        # Setup a tick operation for thinking
        # think_tick_op = Operation("tick")
        # think_tick_op.set_to(self.id)
        # think_tick_op.set_args([Entity(name="think")])

        return None

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
        my_path = self.steering.path
        # print("path size: " + str(len(my_path)))
        for point in my_path:
            path.append([point.x, point.y, point.z])

        think_op.set_args([Entity(path=path, current_path_index=self.steering.current_path_index)])

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

    def think_look_operation(self, op):
        """Sends back information about goals. This is mainly to be used for debugging minds.
        If no arguments are specified all goals will be reported, else a match will be done
        using 'index'.
        The information will be sent back as a Think operation wrapping an Info operation.
        
        This method is automatically invoked by the C++ BaseMind code, due to its *_*_operation name.
        """
        think_op = Operation("think")
        goal_info_op = Operation("info")
        goal_infos = []

        if not op.get_args():
            # get all goals
            for (index, goal) in enumerate(self.goals):
                goal_infos.append(Entity(index=index, report=goal.report()))
        else:
            for arg in op.get_args():
                goal = self.goals[arg.index]
                if goal and goal is not None:
                    goal_infos.append(Entity(index=arg.index, report=goal.report()))

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
                    if isinstance(object_val, Location):
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

    # Talk operations
    def admin_sound(self, op):
        assert (op.from_ == op.to)
        return op.from_ == self.entity.id

    def interlinguish_warning(self, op, say, msg):
        log.debug(1, str(self.entity.id) + " interlinguish_warning: " + str(msg) + ": " +
                  str(say[0].lexlink.id[1:]), op)

    def interlinguish_desire_verb3_buy_verb1_operation(self, op, say):
        """Handle a sentence of the form 'I would like to buy a ....'

        Check if we have any of the type of thing the other character is
        interested in, and whether we know what price to sell at. If so
        set up the transaction goal, which offers to sell it."""
        word_object = say[1].word
        word_thing = self.things.get(word_object)
        if word_thing:
            price = self.get_knowledge("price", word_object)
            if not price:
                return
            goal = Transaction(word_object, op.to, price)
            who = self.map.get(op.to)
            self.goals.insert(0, goal)
            return Operation("talk", Entity(
                say=self.thing_name(who) + " one " + word_object + " will be " + str(price) + " coins")) + self.face(who)

    def interlinguish_desire_verb3_operation(self, op, say):
        """Handle a sentence of the form 'I would like to ...'"""
        word_object = say[2:]
        word_verb = interlinguish.get_verb(word_object)
        operation_method = self.find_op_method(word_verb, "interlinguish_desire_verb3_",
                                               self.interlinguish_undefined_operation)
        res = Oplist()
        res = res + self.call_interlinguish_triggers(word_verb, "interlinguish_desire_verb3_", op, word_object)
        res = res + operation_method(op, word_object)
        return res

    def interlinguish_know_verb1_operation(self, op, say):
        """Handle a sentence of the form 'know subject predicate object'

        Accept admin instruction about knowledge, and store the triple
        in our knowledge base."""
        if not self.admin_sound(op):
            return self.interlinguish_warning(op, say, "You are not admin")
        word_subject = say[1].word
        word_predicate = say[2].word
        word_object = say[3].word
        #        print "know:",subject,predicate,object
        if word_object[0] == '(':
            # CHEAT!: remove eval
            xyz = list(eval(word_object))
            loc = self.entity.location.copy()
            loc.pos = Vector3D(xyz)
            self.add_knowledge(word_predicate, word_subject, loc)
        else:
            self.add_knowledge(word_predicate, word_subject, word_object)

    def interlinguish_tell_verb1_operation(self, op, say):
        """Handle a sentence of the form 'Tell (me) ....'

        Accept queries about what we know. Mostly this is for debugging
        and for the time being it is useful to answer these queries no matter
        who asks."""

        # Ignore messages addressed to others
        if not self.is_talk_op_addressed_to_me_or_none(op):
            return None

        # Currently no checking for trus here.
        # We are being liberal with interpretation of "subject" and "object"
        word_subject = say[1].word
        word_predicate = say[2].word
        word_object = say[3].word
        k = self.get_knowledge(word_predicate, word_object)
        if k is None:
            pass
        # return Operation('talk',Entity(say="I know nothing about the "+predicate+" of "+object))
        else:
            k_type = type(k)
            if isinstance(k_type, Location):
                dist = self.steering.direction_to(k)
                dist.y = 0
                distmag = dist.mag()
                if distmag < 8:
                    k = 'right here'
                else:
                    # Currently this assumes dist is relative to TLVE
                    k = '%f metres %s' % (distmag, vector_to_compass(dist))
            elif k_type != str:
                k = 'difficult to explain'
            elif word_predicate == 'about':
                return self.face_and_address(op.to, k)
            return self.face_and_address(op.to, "The " + word_predicate + " of " +
                                         word_object + " is " + k)

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
        word_subject = say[1].word
        word_predicate = say[2].word
        if word_predicate == 'all knowledge':
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
            d = self.knowledge.get(word_predicate)
            if len(d) == 0:
                return None
            res = Oplist()
            res = res + self.face(self.map.get(op.to))
            for key in d:
                res = res + self.address(op.to, "The " + word_predicate + " of " +
                                         key + " is " + str(d[key]))
            return res

    def interlinguish_own_verb1_operation(self, op, say):
        """Handle a sentence of the form ' own ...'

        Sentences of this form from the admin inform us that we own an
        entity. This is essential when an entity needs to be used as a
        tool, or raw material."""
        if not self.admin_sound(op):
            return self.interlinguish_warning(op, say, "You are not admin")
        #         print self,"own:",say[1].word,say[2].word
        word_subject = self.map.get_add(say[1].word)
        #         print "subject found:",subject
        word_object = self.map.get_add(say[2].word)
        #         print "object found:",object
        #         if subject.id==self.entity.id:
        #             foo
        if word_subject.id == self.entity.id:
            self.add_thing(word_object)

    def interlinguish_undefined_operation(self, op, say):
        # CHEAT!: any way to handle these?
        log.debug(2, str(self.entity.id) + " interlinguish_undefined_operation:", op)
        log.debug(2, str(say))

    # Sound operations
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

    # Other operations
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

    # Generic knowledge
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
        if isinstance(value, object):
            if what == "goal":
                thought_value = value.info()
            else:
                thought_value = repr(value)
        else:
            thought_value = value
        # desc = "%s knowledge about %s is %s" % (what, key, thought_value)
        #        ent = Entity(description=desc, what=what, key=key, value=thought_value)
        #        self.send(Operation("thought",ent))
        if what == "location":
            # and reverse too
            self.reverse_knowledge.add("location", value, key)

    def remove_knowledge(self, what, key):
        """remove certain type of knowledge"""
        self.knowledge.remove(what, key)

    def thing_name(self, thing):
        """Things we own"""
        if hasattr(thing, 'name'):
            return thing.name
        return thing.type[0]

    # things we own

    def get_attached_entity(self, attachment_name):
        attachment_value = self.entity.get_prop_map("attached_" + attachment_name)
        if attachment_value:
            entity_id = attachment_value["$eid"]
            if entity_id:
                return self.entity.get_child(entity_id)

    def add_thing(self, thing):
        """I own this thing"""
        # CHEAT!: this feature not yet supported
        #         if not thing.location:
        #             thing.location=self.get_knowledge("location",thing.place)
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
            if t == thing:
                found.append(t)
        return found

    def remove_thing(self, thing):
        """I don't own this anymore (it may not exist)"""
        dictlist.remove_value(self.things, thing)

    # goals

    def insert_goal(self, goal):
        # Collect all triggering goals and add them
        if hasattr(goal, "triggering_goals") and goal.triggering_goals is not None:
            triggering_goals = goal.triggering_goals()
            for g in triggering_goals:
                print("Adding trigger goal: {}".format(str(g)))
                # Allow the trigger to either be a single string, or a list of strings
                trigger = g.trigger()
                if isinstance(trigger, list):
                    for trigger_instance in trigger:
                        dictlist.add_value(self.trigger_goals, trigger_instance, g)
                else:
                    dictlist.add_value(self.trigger_goals, trigger, g)

        if hasattr(goal, "entity_appears") and goal.entity_appears is not None:
            self.entity_appear_goals.add(goal)

        self.goals.append(goal)

    def remove_goal(self, goal):
        """Removes a goal."""
        # print('Removing goal')
        if hasattr(goal, "triggering_goals") and goal.triggering_goals is not None:
            triggering_goals = goal.triggering_goals()
            for g in triggering_goals:
                print("Removing trigger goal: {}".format(str(g)))
                dictlist.remove_value(self.trigger_goals, g)

        if hasattr(goal, "entity_appears") and goal.entity_appears is not None:
            self.entity_appear_goals.remove(goal)

        self.goals.remove(goal)

    def fulfill_goals(self):
        """see if all goals are fulfilled: if not try to fulfill them"""
        for g in self.goals[:]:
            if g is None:
                continue
            if g.irrelevant:
                # Irrelevant goals should be kept, to match what's in _goals.
                continue
            try:
                res = g.check_goal(self)
                if res:
                    if isinstance(res, Operation) or isinstance(res, Oplist):
                        return res
                    return
            except Exception:
                stacktrace = traceback.format_exc()
                # Keep track of the number of errors in this goal. This could be used for better logging in the future.
                g.errors += 1
                g.lastError = stacktrace
                # If there's an error, print to the log, mark the goal, and continue with the next goal
                # Some goals have a "str" attribute which represents the constructor; if so use that
                if hasattr(g, "str"):
                    goal_string = g.str
                else:
                    goal_string = g.__class__.__name__
                if hasattr(self, "name"):
                    print("Error in NPC with id " + self.entity.id + " of type " + str(
                        self.entity.type) + " and name '" + self.name + "' when checking goal " + goal_string + "\n" + stacktrace)
                else:
                    print("Error in NPC with id " + self.entity.id + " of type " + str(
                        self.entity.type) + " when checking goal " + goal_string + "\n" + stacktrace)
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
                s, i = interlinguish.importance(im[0], cmp, im[1])
                es = Entity(say=s, interlinguish=i)
                res.append(Operation('say', es, to=child))
        return res

    # thinking (needs rewrite)
    def think(self, ent):

        for t in self.pending_things:
            thing = self.map.get(t)
            if thing and thing.type[0]:
                self.add_thing(thing)
        self.pending_things = []
        output = self.fulfill_goals()
        if self.message_queue:
            output = self.message_queue + output
            self.message_queue = None

        #        if output and const.debug_thinking:
        #            log.thinking(str(self)+" result at "+str(self.time)+": "+output[-1][0].description)
        return output

    # communication: here send it locally
    def send(self, op):
        if not self.message_queue:
            self.message_queue = Oplist(op)
        else:
            self.message_queue.append(op)

    def face(self, other):
        """turn to face other entity"""
        vector = self.steering.direction_to(other)
        if vector is None:
            return
        vector.y = 0
        if vector.sqr_mag() < 0.1:
            return
        vector = vector.unit_vector()
        return Operation("set", Entity(self.entity.id, _direction=Quaternion(Vector3D(0, 0, 1), vector, Vector3D(0, 1, 0)).as_list()))

    def address(self, entity_id, message):
        """Creates a new Talk op which is addressed to an entity"""
        return Operation('talk', Entity(say=message, address=[entity_id]))

    def face_and_address(self, entity_id, message):
        """Utility method for generating ops for both letting the NPC face
           as well as address another entity. In most cases this is what you
           want to do when conversing."""
        return self.address(entity_id, message) + self.face(self.map.get(entity_id))

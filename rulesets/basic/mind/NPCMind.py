#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 1999 Aloril (See the file COPYING for details).

from atlas import *
from physics import *
from Quaternion import Quaternion
from common import const
from types import *

from Vector3D import Vector3D
from cyphesis.Thing import Thing

from mind.Memory import Memory
from mind.Knowledge import Knowledge
from mind.panlingua import interlinguish,ontology
from mind.compass import vector_to_compass
from common import log,const
import dictlist
import mind.goals
import mind.goals.common

reverse_cmp={'>':'<'}

def get_dict_func(self, func_str, func_undefined):
    """get method by name from instance or return default handler"""
    try:
        func=getattr(self,func_str)
    except AttributeError:
        func=func_undefined
    return func

class NPCMind(Thing):
    """Mind class for most mobile entities in the game.

    An NPCMind object is associated with all NPC and similar entities on a
    game server. It handles perception data from the world, tracks what
    the NPC knows about, and handles its goals.

    The data is organised into three key data structures:

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
        self.cinit(cppthing)

        self.knowledge=Knowledge()
        self.mem=Memory(map=self.map)
        self.things={}
        self.pending_things=[]
        self._reverse_knowledge()
        self.goals=[]
        self.money_transfers=[]
        self.transfers=[]
        self.trigger_goals={}
        #???self.debug=debug(self.name+".mind.log")
        self.message_queue=None
        #This is going to be really tricky
        self.map.add_hooks_append("add_map")
        self.map.update_hooks_append("update_map")
        self.map.delete_hooks_append("delete_map")
    def find_op_method(self, op_id, prefix="",undefined_op_method=None):
        """find right operation to invoke"""
        if not undefined_op_method: undefined_op_method=self.undefined_op_method
        return get_dict_func(self, prefix+op_id+"_operation",undefined_op_method)
    def undefined_op_method(self, op):
        """this operation is used when no other matching operation is found"""
        pass
    def get_op_name_and_sub(self, op):
        event_name = op.id
        sub_op = op
        # I am not quite sure why this is while, as it's only over true
        # for one iteration.
        while len(sub_op) and sub_op[0].get_name()=="op":
            sub_op = sub_op[0]
            event_name = event_name + "_" + sub_op.id
        return event_name, sub_op
    ########## Map updates
    def add_map(self, obj):
        """Hook called by underlying map code when an entity is added."""
        #print "Map add",obj
        pass
    def update_map(self, obj):
        """Hook called by underlying map code when an entity is added.

        Fix ownership category for objects owned temporary under 'Foo' type."""
        #print "Map update",obj
        foo_lst = self.things.get('Foo',[])
        for foo in foo_lst[:]: #us copy in loop, because it might get modified
            print "Oh MY GOD! We have a Foo thing!"
            if foo.id==obj.id:
                self.remove_thing(foo)
                self.add_thing(obj)
    def delete_map(self, obj):
        """Hook called by underlying map code when an entity is added."""
        #print "Map delete",obj
        self.remove_thing(obj)
    ########## Operations
    def setup_operation(self, op):
        """called once by world after object has been made
           send first tick operation to object"""
        #CHEAT!: add memory, etc... initialization (or some of it to __init__)
        return Operation("look")+Operation("tick")
    def tick_operation(self, op):
        """periodically reasses situation"""
        opTick=Operation("tick")
        opTick.setFutureSeconds(const.basic_tick)
        for t in self.pending_things:
            thing = self.map.get(t)
            if thing and thing.type[0]:
                self.add_thing(thing)
        self.pending_things=[]
        result=self.think()
        if self.message_queue:
            result = self.message_queue + result
            self.message_queue = None
        return opTick+result
    def unseen_operation(self, op):
        obsolete_id = op[0].id
        self.map.delete(obsolete_id)
    ########## Sight operations
    def sight_create_operation(self, op):
        """Note our ownership of entities we created."""
        #BaseMind version overridden!
        obj=self.map.add(op[0], op.getSeconds())
        if op.to==self.id:
            self.add_thing(obj)
    def sight_move_operation(self, op):
        """change position in out local map"""
        obj=self.map.update(op[0], op.getSeconds())
        if obj.location.parent.id==self.id:
            self.add_thing(obj)
            if op.to != self.id:
                self.transfers.append((op.from_, obj.id))
            if obj.type[0]=="coin" and op.from_ != self.id:
                self.money_transfers.append([op.from_, 1])
                return Operation("imaginary", Entity(description="accepts"))
    ########## Talk operations
    def admin_sound(self, op):
        assert(op.from_ == op.to)
        return op.from_ == self.id

    def interlinguish_warning(self, op, say, msg):
        log.debug(1,str(self.id)+" interlinguish_warning: "+str(msg)+\
                  ": "+str(say[0].lexlink.id[1:]),op)
    def interlinguish_desire_verb3_buy_verb1_operation(self, op, say):
        """Handle a sentence of the form 'I would iike to buy a ....'

        Check if we have any of the type of thing the other character is
        interested in, and whether we know what price to sell at. If so
        set up the transaction goal, which offers to sell it."""
        object=say[1].word
        thing=self.things.get(object)
        if thing:
            price=self.get_knowledge("price", object)
            if not price:
                return
            goal=mind.goals.common.misc_goal.transaction(object, op.to, price)
            who=self.map.get(op.to)
            self.goals.insert(0,goal)
            return Operation("talk", Entity(say=self.thing_name(who)+" one "+object+" will be "+str(price)+" coins")) + self.face(who)
    def interlinguish_desire_verb3_operation(self, op, say):
        """Handle a sentence of the form 'I would like to ...'"""
        object=say[2:]
        verb=interlinguish.get_verb(object)
        operation_method=self.find_op_method(verb,"interlinguish_desire_verb3_",
                                             self.interlinguish_undefined_operation)
        res = Message()
        res = res + self.call_interlinguish_triggers(verb, "interlinguish_desire_verb3_", op, object)
        res = res + operation_method(op, object)
        return res
    def interlinguish_be_verb1_operation(self, op, say):
        """Handle sentences of the form '... is more important that ...'

        Accept instructions about the priority of goals relative to each
        based on key verbs associated with those goals."""
        if not self.admin_sound(op):
            return self.interlinguish_warning(op,say,"You are not admin")
        res=interlinguish.match_importance(say)
        if res:
            return self.add_importance(res['sub'].id,'>',res['obj'].id)
        else:
            return self.interlinguish_warning(op,say,"Unkown assertion")
    def interlinguish_know_verb1_operation(self, op, say):
        """Handle a sentence of the form 'know subject predicate object'

        Accept admin instruction about knowledge, and store the triple
        in our knowledge base."""
        if not self.admin_sound(op):
            return self.interlinguish_warning(op,say,"You are not admin")
        subject=say[1].word
        predicate=say[2].word
        object=say[3].word
##        print "know:",subject,predicate,object
        if object[0]=='(':
            #CHEAT!: remove eval
            xyz=list(eval(object))
            loc=self.location.copy()
            loc.coordinates=Vector3D(xyz)
            self.add_knowledge(predicate,subject,loc)
        else:
            self.add_knowledge(predicate,subject,object)
    def interlinguish_tell_verb1_operation(self, op, say):
        """Handle a sentence of the form 'Tell (me) ....'

        Accept queries about what we know. Mostly this is for debugging
        and for the time being it is useful to answer these queries no matter
        who hasks."""
        # Currently no checking for trus here.
        # We are being liberal with interpretation of "subject" and "object"
        subject=say[1].word
        predicate=say[2].word
        object=say[3].word
        k=self.get_knowledge(predicate, object)
        if k==None: pass
            # return Operation('talk',Entity(say="I know nothing about the "+predicate+" of "+object))
        else:
            k_type = type(k)
            if k_type==type(Location()):
                dist = distance_to(self.location, k)
                dist.z = 0
                distmag = dist.mag()
                if distmag < 8:
                    k = 'right here'
                else:
                    # Currently this assumes dist is relative to TLVE
                    k='%f metres %s' % (distmag, vector_to_compass(dist))
            elif k_type!=StringType:
                k='difficult to explain'
            elif predicate=='about':
                return Operation('talk', Entity(say=k)) + \
                       self.face(self.map.get(op.to))
            return Operation('talk', Entity(say="The " + predicate + " of " +
                                                object + " is " + k)) + \
                   self.face(self.map.get(op.to))
    def interlinguish_list_verb1_operation(self, op, say):
        """Handle a sentence of the form 'List (me) ....'

        Accept queries about what we know. Mostly this is for debugging
        and for the time being it is useful to answer these queries no matter
        who hasks."""
        # Currently no checking for trus here.
        # We are being liberal with interpretation of "subject" and "object"
        subject=say[1].word
        predicate=say[2].word
        if not hasattr(self.knowledge, predicate):
            return None
        d=getattr(self.knowledge, predicate)
        res = Message()
        res = res + self.face(self.map.get(op.to))
        for key in d:
            res = res + Operation('talk',
                                  Entity(say="The " + predicate + " of " + key +
                                             " is " + str(d[key])))
        return res
    def interlinguish_learn_verb1_operation(self, op, say):
        """Handle a sentence of the form 'learn ....'

        The learn sentence contains two components, and can hardly
        be concidered a real sentence at all. The first is the verb
        to be associated with a goal activity, and the second is
        the python fragment executed in order to create the goal instance.
        Obviously accepting a code fragment from the client is very
        dangerous, and we need to be carefull that the admin_sound() check
        works."""
        if not self.admin_sound(op):
            return self.interlinguish_warning(op,say,"You are not admin")
        subject=say[1].word
        object=say[2].word
        self.add_goal(subject,object)
    def interlinguish_own_verb1_operation(self, op, say):
        """Handle a sentence of the form ' own ...'

        Sentences of this form from the admin inform us that we own an
        entity. This is essential when an entity needs to be used as a
        tool, or raw material."""
        if not self.admin_sound(op):
            return self.interlinguish_warning(op,say,"You are not admin")
##         print self,"own:",say[1].word,say[2].word
        subject=self.map.get_add(say[1].word)
##         print "subject found:",subject
        object=self.map.get_add(say[2].word)
##         print "object found:",object
##         if subject.id==self.id:
##             foo
        if subject.id==self.id:
            self.add_thing(object)
    def interlinguish_undefined_operation(self, op, say):
        #CHEAT!: any way to handle these?
        log.debug(2,str(self.id)+" interlinguish_undefined_operation:",op)
        log.debug(2,str(say))
    ########## Sound operations
    def sound_talk_operation(self, op):
        """Handle the sound of a talk operation from another character.

        The spoken sentence comes in as a sentence string, which
        is converted into a structure representation by the interlinguish
        code. Embedded in the structure is the interlinguish string which
        is then used to call methods and activate triggers, such as
        dynamic goals."""

        talk_entity=op[0]
        if interlinguish.convert_english_to_interlinguish(self, talk_entity):
            say=talk_entity.interlinguish
            verb=interlinguish.get_verb(say)
            operation_method=self.find_op_method(verb,"interlinguish_",
                                  self.interlinguish_undefined_operation)
            res = self.call_interlinguish_triggers(verb, "interlinguish_", op, say)
            res2 = operation_method(op,say)
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
        event_name = prefix+verb
        reply = Message()
        for goal in self.trigger_goals.get(event_name,[]):
            reply += goal.event(self, op, say)
        return reply
    def call_triggers_operation(self, op):
        event_name, sub_op = self.get_op_name_and_sub(op)
        reply = Message()
        for goal in self.trigger_goals.get(event_name,[]):
            reply += goal.event(self, op, sub_op)
        return reply
    ########## Generic knowledge
    def _reverse_knowledge(self):
        """normally location: tell where items reside
           reverse location tells what resides in this spot"""
        self.reverse_knowledge=Knowledge()
        for (k,v) in self.knowledge.location.items():
            if not self.reverse_knowledge.location.get(v):
                self.reverse_knowledge.add("location",v,k)
    def get_reverse_knowledge(self, what, key):
        """get certain reverse knowledge value
           what: what kind of knowledge (location only so far)"""
        d=getattr(self.reverse_knowledge,what)
        return d.get(key)
    def get_knowledge(self, what, key):
        """get certain knowledge value
           what: what kind of knowledge (see Knowledge.py for list)"""
        if not hasattr(self.knowledge, what):
            return None
        d=getattr(self.knowledge,what)
        return d.get(key)
    def add_knowledge(self,what,key,value):
        """add certain type of knowledge"""
        self.knowledge.add(what,key,value)
        #forward thought
        if type(value)==InstanceType:
            if what=="goal":
                thought_value = value.info()
            else:
                thought_value = `value`
        else:
            thought_value = value
        desc="%s knowledge about %s is %s" % (what,key,thought_value)
        ent = Entity(description=desc, what=what, key=key, value=thought_value)
        self.send(Operation("thought",ent))
        if what=="location":
            #and reverse too
            self.reverse_knowledge.add("location",value,key)
    def remove_knowledge(self,what,key):
        """remove certain type of knowledge"""
        self.knowledge.remove(what,key)
    ########## Importance: Knowledge about how things compare in urgency, etc..
    def add_importance(self, sub, cmp, obj):
        """add importance: both a>b and b<a"""
        self.add_knowledge('importance',(sub,obj),cmp)
        self.add_knowledge('importance',(obj,sub),reverse_cmp[cmp])
    def cmp_goal_importance(self, g1, g2):
        """which of goals is more important?
           also handle more generic ones:
           for example if you are comparing breakfast to sleeping
           it will note that having breakfast is a (isa) type of eating"""
        try:
            id1=g1.key[1]
            id2=g2.key[1]
        except AttributeError:
            return 1
        l1=ontology.get_isa(id1)
        l2=ontology.get_isa(id2)
        for s1 in l1:
            for s2 in l2:
                cmp=self.knowledge.importance.get((s1.id,s2.id))
                if cmp:
                    return cmp=='>'
        return 1
    ########## things we own
    def thing_name(self,thing):
        if hasattr(thing, 'name'):
            return thing.name
        return thing.type[0]
    ########## things we own
    def add_thing(self,thing):
        """I own this thing"""
        #CHEAT!: this feature not yet supported
##         if not thing.location:
##             thing.location=self.get_knowledge("location",thing.place)
        log.debug(3,str(self)+" "+str(thing)+" before add_thing: "+str(self.things))
        #thought about owing thing
        name = self.thing_name(thing)
        if not name:
            self.pending_things.append(thing.id)
            return
        desc="I own %s." % name
        what=thing.as_entity()
        ent = Entity(description=desc, what=what)
        self.send(Operation("thought",ent))
        dictlist.add_value(self.things,name,thing)
        log.debug(3,"\tafter: "+str(self.things))
    def find_thing(self, thing):
        if StringType==type(thing):
            #return found list or empty list
            return self.things.get(thing,[])
        found=[]
        for t in self.things.get(self.thing_name(thing),[]):
            if t==thing: found.append(t)
        return found
    def remove_thing(self, thing):
        """I don't own this anymore (it may not exist)"""
        dictlist.remove_value(self.things, thing)
    ########## goals
    def add_goal(self, name, str_goal):
        """add goal..."""
        #CHEAT!: remove eval (this and later)
        goal=eval("mind.goals."+str_goal)
        if const.debug_thinking:
            goal.debug=1
        goal.str=str_goal
        if type(name)==StringType: goal.key=eval(name)
        else: goal.key=name
        self.add_knowledge("goal",name,goal)
        if hasattr(goal,"trigger"):
            dictlist.add_value(self.trigger_goals, goal.trigger(), goal)
            return
        for i in range(len(self.goals)-1,-1,-1):
            if self.cmp_goal_importance(self.goals[i],goal):
                self.goals.insert(i+1,goal)
                return
        self.goals.insert(0,goal)
    def fulfill_goals(self,time):
        "see if all goals are fulfilled: if not try to fulfill them"
        for g in self.goals[:]:
            if g.irrelevant:
                self.goals.remove(g)
                continue
            res=g.check_goal(self,time)
            if res: return res
            # if res!=None: return res
    def teach_children(self, child):
        res=Message()
        for k in self.knowledge.location.keys():
            es=Entity(verb='know',subject=k,object=self.knowledge.location[k])
            res.append(Operation('say',es,to=child))
        for k in self.knowledge.place.keys():
            es=Entity(verb='know',subject=k,object=self.knowledge.place[k])
            res.append(Operation('say',es,to=child))
        for g in self.goals:
            es=Entity(verb='learn',subject=g.key,object=g.str)
            res.append(Operation('say',es,to=child))
        for im in self.knowledge.importance.keys():
            cmp=self.knowledge.importance[im]
            if cmp=='>':
                s,i=il.importance(im[0],cmp,im[1])
                es=Entity(say=s,interlinguish=i)
                res.append(Operation('say',es,to=child))
        return res
    ########## thinking (needs rewrite)
    def think(self):
        if const.debug_thinking:
            log.thinking("think: "+str(self))
        output=self.fulfill_goals(self.time)
        if output and const.debug_thinking:
            log.thinking(str(self)+" result at "+str(self.time)+": "+output[-1][0].description)
        return output
    ########## communication: here send it locally
    def send(self, op):
        if not self.message_queue:
            self.message_queue=Message(op)
        else:
            self.message_queue.append(op)
    ########## turn to face other entity
    def face(self, other):
        vector = distance_to(self.location, other.location)
        vector.z = 0
        if vector.square_mag() < 0.1:
            return
        vector = vector.unit_vector()
        newloc = Location(self.location.parent)
        newloc.orientation = Quaternion(Vector3D(1,0,0), vector)
        return Operation("move", Entity(self.id, location=newloc))

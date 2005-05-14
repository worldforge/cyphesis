#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 1999 Aloril (See the file COPYING for details).

from atlas import *
from physics import *
from Quaternion import Quaternion
from common import const
from types import *

if const.server_python:
    from world.physics.Vector3D import Vector3D
    from mind.BaseMind import BaseMind
else:
    from Vector3D import Vector3D
    from world.objects.Thing import Thing
    from server import WorldTime
    BaseMind = Thing

from mind.Memory import Memory
from mind.Knowledge import Knowledge
from mind.panlingua import interlinguish,ontology
from mind.compass import vector_to_compass
from common import log,const
from server import dictlist
import mind.goals
import mind.goals.common

reverse_cmp={'>':'<'}

class NPCMind(BaseMind):
    """base class for all NPCs"""
    ########## Initialization
    def __init__(self, cppthing, body=None, **kw):
        self.base_init(cppthing, kw)
        self.body=body

        self.knowledge=Knowledge()
        self.mem=Memory(map=self.map)
        self.things={}
        self._reverse_knowledge()
        self.goals=[]
        self.money_transfers=[]
        self.trigger_goals={}
        #???self.debug=debug(self.name+".mind.log")
        self.tick_count=0
        self.message_queue=None
        self.time=WorldTime(31104000)
        #This is going to be really tricky
        self.map.add_hooks_append("add_map")
        self.map.update_hooks_append("update_map")
        self.map.delete_hooks_append("delete_map")
    ########## Map updates
    def add_map(self, obj):
        #print "Map add",obj
        pass
    def update_map(self, obj):
        """fix ownership category for objects owned temporary under 'Foo' type"""
        #print "Map update",obj
        foo_lst = self.things.get('Foo',[])
        for foo in foo_lst[:]: #us copy in loop, because it might get modified
            if foo.id==obj.id:
                self.remove_thing(foo)
                self.add_thing(obj)
    def delete_map(self, obj):
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
        self.tick_count=self.tick_count+1
        opTick=Operation("tick")
        opTick.setFutureSeconds(const.basic_tick)
        result=self.think()
        if self.message_queue:
            result = self.message_queue + result
            self.message_queue = None
        return opTick+result
    def unseen_operation(self, op):
        obsolete_id = op[0].id
        print "Got unseen op for ", obsolete_id
        self.map.delete(obsolete_id)
    ########## Persistance operations
    def save_operation(self, op):
        mind = Entity(self.id)
        mind.place = self.knowledge.place
        mind.location = self.knowledge.location
        mind.goal = self.knowledge.goal
        mind.importance = self.knowledge.importance
        mind.price = self.knowledge.price
        print "SAVE"
        return Operation("info", mind)
    def load_operation(self, op):
        print "LOAD"
    ########## Sight operations
    def sight_create_operation(self, original_op, op):
        #BaseMind version overridden!
        obj=self.map.add(op[0], op.getSeconds())
        if original_op.from_==self:
            self.add_thing(obj)
    def sight_move_operation(self, original_op, op):
        """change position in out local map"""
        obj=self.map.update(op[0], op.getSeconds())
        if obj.location.parent.id==self.id:
            self.add_thing(obj)
            if obj.type[0]=="coin":
                self.money_transfers.append([op.from_.id, 1])
                return Operation("action", Entity(self.id, action="sell"))
    #replaced with dynamically added add_extinguish_fire -goal
    #ie: NPC is first teached that when it sees "sight_burn" it needs to
    #execute above goal, which then adds extinguish_fire goal and executes it
##     def sight_burn_operation(self, original_op, op):
##         #CHEAT!:
##         fire=self.map.get(op[0].id)
##         if fire:
##             log.debug(2,"sight_burn_operation: found fire obj")
##             if not self.goals or self.goals[0].__class__.__name__!="extinguish_fire":
##                 log.debug(2,"sight_burn_operation: added extinguish_fire goal")
##                 goal=mind.goals.humanoid.fire.extinguish_fire(fire)
##                 self.goals.insert(0,goal)
##         log.debug(2,"sight_burn_operation: add goal updating")
    ########## Talk operations
    def admin_sound(self, op):
        return op[0].from_.id==self.id

    def interlinguish_warning(self, op, say, msg):
        log.debug(1,str(self.id)+" interlinguish_warning: "+str(msg)+\
                  ": "+str(say[0].lexlink.id[1:]),op)
    def interlinguish_desire_verb3_buy_verb1_operation(self, op, say):
        object=say[1].word
        thing=self.things.get(object)
        if thing:
            price=self.get_knowledge("price", object)
            if not price:
                return
            goal=mind.goals.common.misc_goal.transaction(object, op.from_, price)
            self.goals.insert(0,goal)
            return Operation("talk", Entity(say=op.from_.name+" one "+object+" will be "+str(price)+" coins")) + self.face(op.from_)
    def interlinguish_desire_verb3_operation(self, op, say):
        object=say[2:]
        verb=interlinguish.get_verb(object)
        operation_method=self.find_op_method(verb,"interlinguish_desire_verb3_",
                                             self.interlinguish_undefined_operation)
        res = Message()
        res = res + self.call_interlinguish_triggers(verb, "interlinguish_desire_verb3_", op, object)
        res = res + operation_method(op, object)
        return res
    def interlinguish_be_verb1_operation(self, op, say):
        if not self.admin_sound(op):
            return self.interlinguish_warning(op,say,"You are not admin")
        res=interlinguish.match_importance(say)
        if res:
            return self.add_importance(res['sub'].id,'>',res['obj'].id)
        else:
            return self.interlinguish_warning(op,say,"Unkown assertion")
    def interlinguish_know_verb1_operation(self, op, say):
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
        # Currently no checking for trus here.
        # We are being liberal with interpretation of "subject" and "object"
        subject=say[1].word
        predicate=say[2].word
        object=say[3].word
        k=self.get_knowledge(predicate, object)
        if k==None:
            return Operation('talk',Entity(say="I know nothing about the "+predicate+" of "+object))
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
            return Operation('talk', Entity(say="The "+predicate+" of "+object+" is "+k)) + self.face(op.from_)
    def interlinguish_learn_verb1_operation(self, op, say):
        if not self.admin_sound(op):
            return self.interlinguish_warning(op,say,"You are not admin")
        subject=say[1].word
        object=say[2].word
        self.add_goal(subject,object)
    def interlinguish_own_verb1_operation(self, op, say):
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
    def talk_undefined_operation(self, op, say):
        #CHEAT!: any way to handle these?
        pass
    ########## Sound operations
    def sound_talk_operation(self, original_op, op):
        talk_entity=op[0]
        res = Message()
        if interlinguish.convert_english_to_interlinguish(self,talk_entity):
            say=talk_entity.interlinguish
            verb=interlinguish.get_verb(say)
            operation_method=self.find_op_method(verb,"interlinguish_",
                                  self.interlinguish_undefined_operation)
            res = res + self.call_interlinguish_triggers(verb, "interlinguish_", original_op,say)
        else:
            operation_method=self.talk_undefined_operation
            say=talk_entity
            if hasattr(say,"say"): say=say.say
        log.debug(3,"talk: "+str(operation_method))
        res = res + operation_method(original_op,say)
        return res
    ########## Other operations
    def call_interlinguish_triggers(self, verb, prefix, op, say):
        null_name, sub_op = self.get_op_name_and_sub(op)
        event_name = prefix+verb
        reply = Message()
        for goal in self.trigger_goals.get(event_name,[]):
            reply = reply + goal.event(self, op, say)
        return reply
    def call_triggers_operation(self, op):
        event_name, sub_op = self.get_op_name_and_sub(op)
        reply = Message()
        for goal in self.trigger_goals.get(event_name,[]):
            reply = reply + goal.event(self, op, sub_op)
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
    def add_thing(self,thing): 
        """I own this thing"""
        #CHEAT!: this feature not yet supported
##         if not thing.location:
##             thing.location=self.get_knowledge("location",thing.place)
        log.debug(3,str(self)+" "+str(thing)+" before add_thing: "+str(self.things))
        #thought about owing thing
        desc="I own %s." % thing.name
        what=thing.as_entity()
        ent = Entity(description=desc, what=what)
        self.send(Operation("thought",ent))
        name=thing.name
        if not name or name=='':
            name=thing.type[0]
        dictlist.add_value(self.things,name,thing)
        log.debug(3,"\tafter: "+str(self.things))
    def find_thing(self, thing):
        if StringType==type(thing):
            #return found list or empty list
            return self.things.get(thing,[])
        found=[]
        for t in self.things.get(thing.name,[]):
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
        vector = vector.unit_vector()
        newloc = Location(self.location.parent)
        newloc.orientation = Quaternion(Vector3D(1,0,0), vector)
        return Operation("move", Entity(self.id, location=newloc))

#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 1999 Aloril (See the file COPYING for details).

from atlas import *
from common.misc import set_kw
from common.lookup import get_thing_class
from common import log,const

from common.BaseEntity import BaseEntity

class Thing(BaseEntity):
    "thing: base entity for IG things (=base class for NPCs,tools, etc)"
    ########## Initialization
    def __init__(self, **kw):
        self.base_init(kw)
        set_kw(self,kw,"name",'Foo')
        set_kw(self,kw,"description",'some thing')
        set_kw(self,kw,"status",1.0)
        set_kw(self,kw,"age",0.0) #in seconds
        set_kw(self,kw,"mode","birth")
    ########## Miscellanous
    def send_world(self, msg):
        """send operation to world to route for actual recipient"""
        return self.world.message(msg, self)
    ########## Operations
    def setup_operation(self, op):
        """called once by world after object has been made
           send first tick operation to object"""
        return Operation("tick",to=self)
    def tick_operation(self, op):
        """redefine this and do things you want to do periodically
           to get called again after certain time, use this code:
             opTick=Operation("tick",to=self)
             opTick.time.sadd=const.basic_tick
             return opTick
           you can add other operations to return too:
             return Message(Operation("create",Entity(type=['foo'])),opTick)"""
        pass
    def create_operation(self, op):
        """create object by type name and given attributes
           return sight of creation"""
        return
        ent=op[0]
        if len(ent.type)!=1:
            return self.error(op,"Type field should contain exactly one type")
        type=string.capitalize(ent.type[0])
        #get given object type class or use default Thing class
        type_class=get_thing_class(type,"Thing")
        obj=self.world.add_object(type_class,ent)
        if not obj.location:
            log.debug(3,"??????? no obj.location: "+str(self.id)+" "+str(obj.id))
            if hasattr(ent,"copy"):
                log.debug(3,str(ent.copy.id)+" "+str(ent.copy.location))
            obj.location=self.location.copy()
            obj.location.velocity=Vector3D(0,0,0)
        #CHEAT! make instead so that all container changing movement 
        #does update contains attributes too...
        #grep location= and location.parent=
        if not obj in obj.location.parent.contains:
            obj.location.parent.contains.append(obj)
        #log.debug(3,"Created: "+str(obj)+" now: "+str(self.world.objects))
        op[0]=obj.as_entity()
        return Operation("sight",op)
    def delete_operation(self, op):
        """remove ourself from world"""
        self.world.del_object(self)
        log.debug(3,"Deleted: "+str(self)+" now: "+str(self.world.objects))
        return Operation("sight",op)
    def move_operation(self, op):
        """move ourself"""
        ent=op[0]
        if self.location.parent!=ent.location.parent:
            self.location.parent.contains.remove(self)
            ent.location.parent.contains.append(self)
        self.location=ent.location
        if type(self.location.velocity)==NoneType:
            speed_ratio = 0.0
        else:
            speed_ratio = self.location.velocity.mag()/const.base_velocity
        if speed_ratio > 0.5:
            self.mode = "running"
        elif speed_ratio > 0.0:
            self.mode = "walking"
        else:
            self.mode = "standing"
        ent.mode = self.mode
        if const.enable_ranges:
            log.debug(1,"="*60)
            log.debug(1,"%s %s %s" % (self,self.location.parent.id,self.location.coordinates))
            res = self.world.update_all_ranges(self)
            res2 = self.world.collision(self)
        #CHEAT!: handle movement change too...
        #print "Move:",self,op
        return Operation("sight",op) #+ res + res2
    def set_operation(self, op):
        """change our attributes as given in operation Entity"""
        ent=op[0]
        #CHEAT!: check that all attributes in ent.__dict__ are in self.attributes
        #probably add self.attributes_dict and use that (speed)
        needTrueValue=["type","contains","instance","id","location","stamp"]
        for (key,value) in ent.__dict__.items():
            if not key in self.attributes:
                return self.error(op,"Illegal attribute in set_operation:"+key)
            if value or not key in needTrueValue:
                setattr(self,key,value)
        opSight=Operation("sight",op)
        if self.status<0.0: #oops, we stopped existing...
            opDestroy=Operation("delete",Entity(self),to=self)
            return Message(opSight,opDestroy)
##         if hasattr(self,"drunkness"):
##             print self,"?",self.drunkness,"!",op
        return opSight
    def fire_operation(self, op):
        """if fire is burning, change our status down"""
        if not hasattr(self,"burn_speed"):
            return
        fire_status=op[0].status
        consumed=self.burn_speed*fire_status
        self_ent=Entity(self.id,status=self.status - (consumed/self.mass))
        nour_ent=Entity(self.id,mass=consumed)
        to_=self.world.get_object(op[0].id)
        return Message(Operation("set",self_ent,to=self),Operation("nourish",op[0],nour_ent,to=to_,from_=self))

#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 1999 Aloril (See the file COPYING for details).
from atlas import *
from types import *
from mind.panlingua import interlinguish
il=interlinguish

class editor:
    def __init__(self,m):
        self.m=m
        self.list_call={"say":(self._say,1),
                        "own":(self._own,2),
                        "know":(self._know,2),
                        "price":(self._price,2),
                        "learn":(self._learn,2),
                        "tell":(self._tell,1)}
    def call_list_args(self, *args):
        #indent='\t'*self.cl_depth
        for i in range(self.cl_args[1]):
            if type(args[i])==ListType:
                #print indent,"LIST!"
                #print indent,self.cl_args,args
                #self.cl_depth=self.cl_depth+1
                for a in args[i]:
                    apply(self.call_list_args,args[:i]+(a,)+args[i+1:])
                #self.cl_depth=self.cl_depth-1
                return
        #print indent,"SINGLE!"
        #print indent,self.cl_args,args
        apply(self.cl_args[0],args)
    def __getattr__(self, name):
        if self.list_call.has_key(name):
            self.cl_args=self.list_call[name]
            #self.cl_depth=0
            return self.call_list_args
        raise AttributeError,name
    def make(self, name, **kw):
        kw['name']=name
        ent=apply(Entity,(),kw)
        #ent=Entity(kw)
##         if hasattr(ent,"copy"):
##             foo
        return self.m.make(ent)
    def set(self, _id, **kw):
        kw['id']=_id
        ent=apply(Entity,(),kw)
        return self.m.set(_id,ent)
    def look(self, _id=""):
        return self.m.look(_id)
    def look_for(self, **kw):
        ent=apply(Entity,(),kw)
        return self.m.look_for(ent)
    def _say(self,target,verb,subject,object):
##         es=Entity(verb=verb,subject=subject,object=object)
##         self.m.send(Operation("talk",es,from_=self.m,to=target))
        if type(subject)==InstanceType: subject=subject.id
        elif type(subject)==TupleType: subject=`subject`
        elif type(subject)==StringType: pass
        else: subject=subject.id
        if type(object)==InstanceType: object=object.id
        elif type(object)==TupleType: object=`object`
        elif type(object)==StringType: pass
        else: object=object.id
        string,interlinguish=il.verb_subject_object(verb,subject,object)
        self._tell(target,string,interlinguish)
    def _own(self,target,object):
        self._say(target,'own',target,object)
    def _know(self,target,know):
        self._say(target,'know',know[0],know[1])
    def _price(self,target,price):
        self._say(target,'price',price[0],price[1])
    def _learn(self,target,goal):
        self._say(target,'learn',goal[0],goal[1])
    #Interlinguish
    def _tell(self,target,string,interlinguish):
        es=Entity(say=target.name+", "+string)
        self.m.send(Operation("talk",es,from_=self.m,to=target))
    def tell_importance(self,target,sub,cmp,obj):
        s,i=il.importance(sub,cmp,obj)
        self.tell(target,s,i)


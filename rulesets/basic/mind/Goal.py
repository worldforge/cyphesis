#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 1999 Aloril (See the file COPYING for details).

from types import *
from atlas import *
from common import log

## \defgroup PythonGoals Goal Classes

## \brief Base class for all goals
## \ingroup PythonGoals
class Goal:
    def __init__(self,desc="some goal",fulfilled=None,subgoals=[],
                 validity=None,time=None, debug=0):
        self.desc=desc
        #mind sets these:
        #self.str
        #self.key
        if fulfilled: 
            self.fulfilled = fulfilled
        else: 
            self.fulfilled = lambda me:0 #false
            
        #If no validity function is supplied the goal is always considered valid
        if validity:
            self.validity = validity
        else:
            self.validity = lambda me:True

        #filter out any None subgoals
        self.subgoals=[item for item in subgoals if item is not None]
        self.time=time
        self.debug=debug
        self.vars=[]
        #keeps track of whether the goal is fulfilled or not
        #this is mainly of use for inspection and diagnosis
        self.is_fulfilled=0
        #any subgoal/function/method can set this and 
        #it's checked at start of check_goal_rec
        #and NPCMind.py fulfill_goals uses it too to remove goals from list
        self.irrelevant=0
        #Tracks the number of errors this goal (or any of its subgoals) has produced.
        #This is used by NPC code to remove troublesome goals from the processing
        self.errors=0
        #If an error has occurred, the description of the last one is stored here.
        #This is mainly of use for debugging.
        self.lastError=""
    def __repr__(self):
        return self.info()
    def info(self):
        name=self.__class__.__name__
        if name=="Goal":
            return name+"("+`self.desc`+")"
        var=""
        for v in self.vars:
            if var: var=var+","
            var=var+`getattr(self,v)`
        return name+"("+var+")"
    def is_valid(self, me):
        """Checks if this goal is valid. By this we mean whether the goal is possible to fulfill. 
        If no validity function was supplied at goal creation time the goal is always considered valid. """
        return self.validity(me)
    
    def check_goal(self, me, time):
        "executes goal, see top of file"
        if self.debug:
            log.thinking("GOAL desc: "+self.str)
        res,debugInfo=self.check_goal_rec(me,time,0,"")
        if len(debugInfo)!=0:
            #Keep a copy of the debug info for the "report" method.
            self.lastProcessedGoals=debugInfo
        if res!=None:
            info_ent=Entity(op=res,description=debugInfo)
            return res+Operation("goal_info",info_ent)
    def check_goal_rec(self, me, time, depth, debugInfo):
        """check (sub)goal recursively. 
        
        This is done by iterating over all subgoals, breaking if any subgoal returns an operation."""
        res=None
        if self.irrelevant: return res,debugInfo
        #is it right time range?
        if self.time and not time.is_now(self.time): return res,debugInfo
        if self.debug:
            log.thinking("\t"*depth+"GOAL: bef fulfilled: "+self.desc+" "+`self.fulfilled`)
        if self.fulfilled(me): 
            self.is_fulfilled = 1
            if self.debug:
                log.thinking("\t"*depth+"GOAL: is fulfilled: "+self.desc+" "+`self.fulfilled`)
            return res,debugInfo
        else:
            if self.debug:
                log.thinking("\t"*depth+"GOAL: is not fulfilled: "+self.desc+" "+`self.fulfilled`)
            self.is_fulfilled = 0
        debugInfo=debugInfo+"."+self.info()
        #Iterate over all subgoals, but break if any goal returns an operation
        for sg in self.subgoals:
            if sg == None:
                continue
            if type(sg)==FunctionType or type(sg)==MethodType:
                if self.debug:
                    log.thinking("\t"*depth+"GOAL: bef function: "+`sg`+" "+`res`)
                res=sg(me)
                if self.debug:
                    log.thinking("\t"*depth+"GOAL: aft function: "+`sg`+" "+`res`)
                debugInfo=debugInfo+"."+sg.__name__+"()"
                if res!=None:
                    #If the function generated an op, stop iterating here and return
                    return res,debugInfo
            else:
                if self.debug:
                    log.thinking("\t"*depth+"GOAL: bef sg: "+sg.desc)
                res,debugInfo=sg.check_goal_rec(me,time,depth+1,debugInfo)
                if self.debug: 
                    log.thinking("\t"*depth+"GOAL: aft sg: "+sg.desc+", Result: "+str(res))
                #If the subgoal generated an op, stop iterating here and return
                if res!=None:
                    return res,debugInfo
        return res,debugInfo
    
    def report(self):
        """provides extended information about the goal,
        as well as all subgoals"""
        name=self.__class__.__name__
        map={}
        map["name"]=name
        map["description"]=self.desc
        map["fulfilled"]=self.is_fulfilled
        if hasattr(self, "lastProcessedGoals"):
            map["lastProcessedGoals"] = self.lastProcessedGoals

        subgoals=[]
        for sg in self.subgoals:
            if type(sg)!=FunctionType and type(sg)!=MethodType and sg is not None:
                subgoals.append(sg.report())
        if len(subgoals) > 0:
            map["subgoals"]=subgoals
        
        if len(self.vars) > 0:
            variables={}
            for v in self.vars:
                variables[v]=str(getattr(self,v))
            map["variables"]=variables
        
        if self.errors > 0:
            map["errors"]=self.errors
            map["lastError"]=self.lastError
        return map
    

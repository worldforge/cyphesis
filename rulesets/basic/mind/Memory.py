#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 1999 Aloril (See the file COPYING for details).

from types import *

class Memory:
    def __init__(self, map):
        self.events=[]
        self.map=map
    def destroy(self):
        self.events=None
        self.map=None
    def recall_place(self, location, radius, filter):
        try:
            if type(filter)==ListType:
                for i in filter:
                    result = self.map.find_by_location_query(location, radius, i)
                    if len(result)!=0:
                        return result
            else:
                return self.map.find_by_location_query(location, radius, filter)
        except RuntimeError:
            #Expect the location to be incomplete and handle it
            return None
    def remember_event(self, event):
        "add new memory with age"
        self.events.append([event,1.0])
    def recall_event(self, event, cmp):
        "return list of memories with same command"
        found=[]
        for (e,age) in self.events:
            if apply(cmp,(event,e)): found.append(e)
        return found
    def forget(self):
        "age memories and forgot too old ones"
        for m in self.events:
            m[1]=m[1]-0.1
        #remove forgotten things
        self.events=filter(lambda m:m[1]>const.fzero,self.events)

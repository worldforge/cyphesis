#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 1999 Aloril (See the file COPYING for details).

class Knowledge:
    def __init__(self, place=None, location=None, goal=None, importance=None):
        if place: self.place=place
        else: self.place={}
        if location: self.location=location
        else: self.location={}
        if goal: self.goal=goal
        else: self.goal={}
        if importance: self.importance=importance
        else: self.importance={}
    def add(self, what, key, value):
        if not hasattr(self, what):
            setattr(self, what, {})
        d=getattr(self,what)
        d[key]=value
    def __str__(self):
        s="<know: "
        s=s+"place: "+str(self.place)+"\n"
        s=s+"location: "+str(self.location)+"\n"
        s=s+"goal: "+str(self.goal)+"\n"
        s=s+"importance: "+str(self.importance)+"\n"
        s=s+"price: "+str(self.price)+"\n"
        return s+">\n"


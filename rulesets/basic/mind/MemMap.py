#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 1999 Aloril (See the file COPYING for details).

from atlas import *
from world.objects.Thing import Thing

class MemMap:
    """Maps external Events to the internal "map".
       Some kind of knowledge base"""
    def __init__(self):
        self.things={}
        self.additions_by_id=[]
        self.add_hooks=[]
        self.update_hooks=[]
        self.delete_hooks=[]
    def add_object(self, object):
        """add object to things"""
        self.things[object.id]=object
        for hook in self.add_hooks:
            hook(object)
        return object
    def look_id(self):
        if self.additions_by_id:
            id=self.additions_by_id[0]
            del self.additions_by_id[0]
            return Operation("look",Entity(id))
    def add_id(self, id):
        self.additions_by_id.append(id)
        return self.add(Entity(id,type=["thing"]))
    def add(self, entity):
        """create object from entity and add it to the list of known things"""
        if self.get(entity.id):
            return self.update(entity)
        obj=object_from_entity(Thing,entity)
        obj.__dict__.update(entity.__dict__)
        if obj.location:
            obj.location=obj.location.copy()
        return self.add_object(obj)
    def delete(self, id):
        """remove object..."""
        if self.things.has_key(id):
            obj=self.things[id]
            del self.things[id]
            for hook in self.delete_hooks:
                hook(obj)
    def get(self, id):
        """return object by id, return None if not found"""
        return self.things.get(id)
    def __getitem__(self, id):
        """return object by id, raise KeyError if not found"""
        return self.things[id]
    def get_add(self,id):
        """Tries to find a thing with id "id". If one is found, return it. If
           not, create a dummy one"""
        obj=self.get(id)
        if obj: return obj
        return self.add_id(id)
    def update(self, entity):
        """Adds/Updates Things in internal map, according to entoty, which
           is a Atlas Entity"""
        try:
            obj=self.things[entity.id]
        except KeyError:
            return self.add(entity)
        needTrueValue=["type","contains","instance","id","location","stamp"]
        for (key,value) in entity.__dict__.items():
            if value or not key in needTrueValue:
                setattr(obj,key,value)
        if entity.location:
            obj.location=entity.location.copy()
        for hook in self.update_hooks:
            hook(obj)
        return obj
    def find_by_location(self, location, radius=0.0):
        res=[]
        for p in self.things.values():
            if p.location and location.parent==p.location.parent:
                d=location.coordinates.distance(p.location.coordinates)
                if d<=radius:
                    res.append(d,p)
        res.sort()
        return res
    def find_by_type(self, what):
        res=[]
        for thing in self.things.values():
            if thing.type!=[]:
                if thing.type[0]==what:
                    res.append(thing)
        return res


from types import *
import string

import map

#rewrite this to generate actual file list from ruleset/whatever ...
# This belongs in the map.py file of each ruleset
#   object_class_list=["world.objects.Thing", "world.objects.buildings.House"]
#   mind_class_list=["mind.goal.common.move"]

def create_dictionary_from_list(lst):
    """give filename list and create dictionary suitable for importing
       actual class instance:
       ["world.objects.Thing", "world.objects.buildings.House"]
       ->
       {'house': ('world.objects.buildings.House', 'House'), 
        'thing': ('world.objects.Thing', 'Thing')}
    """
    dict={}
    for obj_file_name in lst:
        l=string.split(obj_file_name,".")[-1] #get last element
        dict[string.lower(l)]=(obj_file_name,l)
    return dict

object_class_dictionary=create_dictionary_from_list(map.object_class_list)
mind_class_dictionary=create_dictionary_from_list(map.mind_class_list)

def get_object_from_name(name, dict):
    """tries to find object and import it:
       if it doesn't find object: return None
       if import error: let it go through
       if it finds, return object (class instance for example)

       cache already loaded objects
    """
    obj = dict.get(string.lower(name))
    if not obj: return None #object is not found
    if type(obj)==TupleType: #should we do importing?
        exec("from %s import %s" % obj)
        obj=eval(obj[1])
        dict[name]=obj
        return obj
    else: #it's cached
        return obj


def get_thing_class(name, default, dict_name="world"):
    """find class definition"""
    if dict_name == "world":
        dict = object_class_dictionary
    else:
        dict = mind_class_dictionary
    class_def=get_object_from_name(name, dict)
    if class_def: return class_def
    class_def = get_object_from_name(default, dict)
    print "didn't found:",name,"got default instead:",default,class_def
    return class_def


#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 1999 Aloril (See the file COPYING for details).

import os
from types import *

def farmer_media(obj):
    if obj.sex=="male":
        return "farmer_male_3_us.png"
    return "farmer_female_2_us.png"

def fire_media(obj):
    if obj.status>0.3:
        return "fire_big_2_us.png"
    return "fire_small_2_us.png"

def house_media(obj):
    if obj.status>=0.99: return "house_ready_2_us.png"
    if obj.status<0.5: return "house_ruin_2_us.png"
    return "house_buildup_2_us.png"

media_mapping={"smith":"blacksmith_3_us.png",
               "farmer":farmer_media,
               "fire":fire_media,
               "house":house_media}

def lookup_media_by_object(obj):
    file=media_mapping.get(obj.type[0])
    if not file: return obj.type[0]+".png"
    if type(file)==StringType: return file
    return file(obj)

def is_house(obj):
    return obj.type[0]=="house"

def is_character(obj):
    return obj.type[0] in ["farmer","smith","character", "creator"]

def is_element(obj):
    return obj.type[0]=="fire"

def is_animal(obj):
    return obj.type[0]=="pig"

def has_media(obj):
    return is_house(obj) or  is_character(obj) or \
           is_element(obj) or is_animal(obj)

media_order_value = {"house":0, "oak":0,
                     "fire":1,
                     "pig":2, "wolf":2, "crab":2, "skeleton":2, "squirrel":2,
                     "farmer":3, "smith":3, "character":3, "creator":3}

def cmp_media(obj1, obj2):
    value1=media_order_value[obj1.type[0]]
    value2=media_order_value[obj2.type[0]]
    return cmp(value1,value2)

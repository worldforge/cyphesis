#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 1999 Aloril (See the file COPYING for details).

from types import *
import types
from common import log,const
import string

def flatten1(l):
    """not used"""
    res=[]
    for i in l:
        if type(i)==ListType:
            for j in i: res.append(j)
        else: res.append(i)
    return res

def get_derivation_string(cl):
    """not used"""
    try:
        base_class=cl.__bases__[0]
        return get_derivation_string(base_class)+"."+cl.__name__
    except IndexError:
        return cl.__name__

def get_kw(kw, name, default=None):
    """get argument by name:
       if argument dictionary (kw) has it: return it
       if given object to copy and it has required attribute: return it
       otherwise return default"""
    if kw.has_key(name): return kw[name]
    if kw.has_key("copy"):
        copy=kw["copy"]
        if hasattr(copy,name): return getattr(copy,name)
    return default

def set_kw(self, kw, name, default=None):
    """set attribute by name from argument:
       add name to list of standard atlas attributes
       get value given in arguments or default
       and set attribute to it"""
    if not name in self.attributes:
        self.attributes.append(name)
    value=get_kw(kw,name,default)
    setattr(self,name,value)

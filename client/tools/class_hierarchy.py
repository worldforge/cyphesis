#!/usr/bin/env python
#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 1999 Aloril (See the file COPYING for details).

import re,sys

sys.path.append(".")
from common.recursive_glob import recursive_glob

class_pattern=re.compile(r"class\W+(\w+)\W*\((\w+)\)")

def process_file(file,class_dict,parent_class_dict):
    fp=open(file)
    lines=fp.readlines()
    fp.close()
    for line in lines:
        match=class_pattern.match(line)
        if match:
            child,parent=match.groups()
            old_list=class_dict.get(parent,[])
            old_list.append(child)
            class_dict[parent]=old_list
            parent_class_dict[child]=parent

def print_class_dict(class_dict, parent_class_dict):
    maybe_base_classes=class_dict.keys()
    base_classes=[]
    for cl in maybe_base_classes:
        if not parent_class_dict.has_key(cl):
            base_classes.append(cl)
    for base_class in base_classes:
        print_base_class(base_class,class_dict)

def print_base_class(base_class,class_dict,depth=0):
    print base_class,
    depth=depth+len(base_class)
    child_classes=class_dict.get(base_class,[])
    for child in child_classes:
        if child!=child_classes[0]:
            print " "*depth,
        print "->",
        print_base_class(child,class_dict,depth+4)
    if not child_classes:
        print

if __name__=="__main__":
    files=recursive_glob("*.py")
    class_dict={}
    parent_class_dict={}
    for file in files:
        process_file(file,class_dict,parent_class_dict)
    print_class_dict(class_dict,parent_class_dict)


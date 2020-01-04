# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 1999 Aloril (See the file COPYING for details).

# dictionary with list of items, key is items name


def add_value(dict, key, item):
    """add value to list in dictionary using key"""
    try:
        list = dict[key]
        if item not in list:
            list.append(item)
    except KeyError:
        dict[key] = [item]


def remove_value(dict, item, remove_empty_key=1):
    """removes value from dictionary and removes key too if no values
       and if remove_empty_key true"""
    flag = 0
    for (key, value) in list(dict.items()):
        if item in value:
            flag = 1
            value.remove(item)
            if remove_empty_key and len(value) == 0:
                del dict[key]
    return flag

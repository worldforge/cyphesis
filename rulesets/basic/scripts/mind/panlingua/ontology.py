# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 1999 Aloril (See the file COPYING for details).

from mind.panlingua.panlingua import *

o = semnet


def add_link(fr, type, to, mass=1.0):
    semlink(fr, type, to, mass, o)


add_link("#breakfast_verb1", "isa", "#eat_verb2")
add_link("#lunch_verb1", "isa", "#eat_verb2")
add_link("#sup_verb1", "isa", "#eat_verb2")


def get_isa(id):
    res = []
    if id not in o: return res
    s = o[id]
    res.append(s)
    for l in s.semlinks:
        if l.type == 'isa' and l.fr == s:
            res = res + get_isa(l.to.id)
    return res

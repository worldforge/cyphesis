# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 1999 Aloril (See the file COPYING for details).

from random import *

from common import const
from atlas import Operation, Entity
from physics import Vector3D

from mind.Goal import Goal


############################ FIND HOME ####################################

def is_suitable_place_for_home(me):
    """place is suitable is there is no homes in 100m radius"""
    count = len(me.mem.recall_place(me.entity.location, 100.0, "house"))
    return count == 0


def find_place(me):
    """find place for home: wander randomly"""
    loc = me.entity.location.copy()
    loc.pos = Vector3D([c + uniform(-50, 50) for c in loc.pos])
    ent = Entity(me, location=loc)
    return Operation("move", ent)


find_place_for_home = Goal("find place for home not too near or far from others",
                           is_suitable_place_for_home,
                           [find_place])


def make_home(me):
    """Starts home building"""
    ent = Entity(name="home", type=["house"], location=me.entity.location.copy())
    me.add_knowledge("location", "home", ent.location)
    return Operation("create", ent)


def have_home(me):
    """Do I have home?"""
    return 'home' in me.things


find_home = Goal("find place for home and make it",
                 have_home,
                 [find_place_for_home, make_home])


############################ BUILD HOME ####################################

def have_built_home(me):
    """Do I have home that is fully built?"""
    t = me.things.get('home')
    if t: return t[0].props.status >= 1.0 - const.fzero


def build_house(me):
    """Build my home"""
    h = me.things['home'][0]
    return Operation("set", Entity(h.id, status=h.status + 0.1))


build_home = Goal("find place for home and build it",
                  have_built_home,
                  #                true,
                  [find_home, build_house])

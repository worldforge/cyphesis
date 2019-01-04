# -*- coding: utf-8 -*-
# This file is distributed under the terms of the GNU General Public license.

from atlas import *
from random import *
from mind.panlingua import interlinguish

il = interlinguish
from cyphesis.editor import editor, create_editor
from physics import Quaternion
from physics import Vector3D
from math import *

import server

areaScale = 5

xmin = -30 * areaScale
xmax = 30 * areaScale
ymin = -30 * areaScale
ymax = 30 * areaScale
defaultZ = 5
num_humans = 10
num_obstacles = 400


def default(host='', account='', password='', **args):
    m = create_editor(host, account, password)
    _add_obstacles(m)
    _add_agents(m)


def add_agents(host='', account='', password='', **args):
    m = create_editor(host, account, password)
    _add_agents(m)


def add_obstacles(host='', account='', password='', **args):
    m = create_editor(host, account, password)
    _add_obstacles(m)


def _add_agents(m):
    for i in range(0, num_humans):
        xpos = uniform(xmin, xmax)
        ypos = uniform(ymin, ymax)

        orient = Quaternion(Vector3D(0, 0, 1), uniform(0, pi * 2.0))

        h = m.make('human', pos=(xpos, ypos, defaultZ), orientation=orient.as_list())

        m.know(h, [('w1', 'location', (uniform(xmin, xmax), uniform(ymin, ymax), defaultZ))
            , ('w2', 'location', (uniform(xmin, xmax), uniform(ymin, ymax), defaultZ))
            , ('w3', 'location', (uniform(xmin, xmax), uniform(ymin, ymax), defaultZ))
            , ('w4', 'location', (uniform(xmin, xmax), uniform(ymin, ymax), defaultZ))])
        m.learn(h, [(il.patrol, "patrol(['w1', 'w2', 'w3', 'w4'])")])


def _add_obstacles(m):
    for i in range(0, num_obstacles):
        xpos = uniform(xmin, xmax)
        ypos = uniform(ymin, ymax)

        orient = Quaternion(Vector3D(0, 1, 0), uniform(0, pi * 2.0))

        m.make('stone_palisade', pos=(xpos, ypos, defaultZ), orientation=orient.as_list())

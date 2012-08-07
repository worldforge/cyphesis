# -*- coding: utf-8 -*-
#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 1999 Aloril (See the file COPYING for details).
#Copyright (C) 2000-2008 Alistair Riddoch

from atlas import *
from random import *
#from mind.panlingua import interlinguish
#il=interlinguish
from cyphesis import probability
from cyphesis.editor import editor, create_editor
from physics import Quaternion
from physics import Vector3D
from math import *

import server

spawn_pos=(0,0,0)

#observer calls this
def default(host='', account='', password='', **args):
#   general things

    m=create_editor(host, account, password)

    world=m.look()

    #if not hasattr(world, 'name'):
    _setup_landscape(m, world)
    _setup_objects(m)
    _setup_spawn(m)

#    _add_resources(m)

# This only needs to be called if there is no terrain yet.
def _setup_landscape(m, world):
    """Set up the landscape environment of terrain, trees and natural
       features.
    """

    # Add the terrain surface to the world.
    _setup_terrain(m, world)

    # If there is no boundary, add that.
    if m.look_for(type='boundary') is None:
        m.make('boundary',pos=(-500,-321,-20),bbox=[2,642,300])
        m.make('boundary',pos=(-500,-321,-20),bbox=[821,2,300])
        m.make('boundary',pos=(-500, 320,-20),bbox=[821,2,300])
        m.make('boundary',pos=( 320,-321,-20),bbox=[2,642,300])

#    m.make('ocean',pos=(0,0,0),bbox=[-500,-321,-20,320,321,0])
    m.make('weather', pos=(0,1,0))


def _setup_terrain(m, world):
    """Set up the terrain property on the world object."""

    points = { }
    for i in range(-8, 7):
        for j in range(-6, 7):
            if i>=5 or j>=5:
                points['%ix%i'%(i,j)] = [i, j, uniform(100, 150)]
            elif i<=-5 or j <= -5:
                points['%ix%i'%(i,j)] = [i, j, uniform(-30, -10)]
            elif (i==2 or i==3) and (j==2 or j==3):
                points['%ix%i'%(i,j)] = [i, j, uniform(20, 25)]
            elif i==4 or j==4:
                points['%ix%i'%(i,j)] = [i, j, uniform(30, 80)]
            elif i==-4 or j==-4:
                points['%ix%i'%(i,j)] = [i, j, uniform(-5, 5)]
            else:
                points['%ix%i'%(i,j)] = [i, j, 1+uniform(3, 11)*(abs(i)+abs(j))]

    points['-4x-1'] = [-4, -1, 12.4]
    points['-4x-2'] = [-4, -2, -8.3]
    points['-3x-2'] = [-3, -2, -6.2]
    points['-3x-1'] = [-3, -1, -5.3]
    points['-2x-1'] = [-2, -1, -4.1]
    points['-1x-1'] = [-1, -1, -16.8]
    points['0x-1'] = [0, -1, -3.8]
    points['-1x0'] = [-1, 0, -2.8]
    points['-1x1'] = [-1, 1, -1.8]
    points['-1x2'] = [-1, 2, -1.7]
    points['0x2'] = [0, 2, -1.6]
    points['1x2'] = [1, 2, -1.3]
    points['1x3'] = [1, 3, -1.1]
    points['1x4'] = [1, 4, -0.6]
    points['1x-1'] = [1, -1, 15.8]
    points['0x0'] = [0, 0, 12.8]
    points['1x0'] = [1, 0, 23.1]
    points['0x1'] = [0, 1, 14.2]
    points['1x1'] = [1, 1, 19.7]

    minx=0
    miny=0
    minz=0
    maxx=0
    maxy=0
    maxz=0
    for i in points.values():
        x = i[0]
        y = i[1]
        z = i[2]
        if not minx or x < minx:
            minx = x
        if not miny or y < miny:
            miny = y
        if not minz or z < minz:
            minz = z
        if not maxx or x > maxx:
            maxx = x
        if not maxy or y > maxy:
            maxy = y
        if not maxz or z > maxz:
            maxz = z

    surfaces = [
        {'name': 'rock', 'pattern': 'fill' },
        {'name': 'sand', 'pattern': 'band', 'params': {'lowThreshold': -2.0,
                                                       'highThreshold': 1.5 } },
        {'name': 'grass', 'pattern': 'grass', 'params': {'lowThreshold': 1.0,
                                                         'highThreshold': 80.0,
                                                         'cutoff': 0.5,
                                                         'intercept': 1.0 } },
        {'name': 'silt', 'pattern': 'depth', 'params': {'waterLevel': 0.0,
                                                        'murkyDepth': -10.0 } },
        {'name': 'snow', 'pattern': 'high', 'params': {'threshold': 110.0 } },
    ]

    
        
    # print minx, ":", miny, ":", minz, ":", maxx, ":", maxy, ":", maxz

    m.set(world.id, terrain={'points' : points, 'surfaces' : surfaces}, name="moraf", bbox=[minx * 64, miny * 64, minz, maxx * 64, maxy * 64, maxz])

def _setup_spawn(m):

    spawn_area_points=[]
    for i in range(10, 350, 17):
        spawn_area_points.append([14 * sin(radians(i)), 16 * cos(radians(i))])
    spawn_area={'shape': {'points': spawn_area_points, 'type': 'polygon'},
               'layer': 7 }
    m.make('path', name='spawn_area', pos=spawn_pos, area=spawn_area,
           bbox=[-14, -16, 0, 14, 16, 1],
           spawn={'name': 'spawn area',
                  'character_types': ['human'],
                  'contains': ['rock']})

def _setup_objects(m):

    m.make('rock',pos=(-2,-4,0))

def add_memtest(host='', account='', password='', **args):
#   general things

    m=create_editor(host, account, password)

    m.make('human',pos=(0,5,5))
    m.make('rock',pos=(5,0,5))

def flat_terrain(host='', account='', password='', **args):
#   general things

    m=create_editor(host, account, password)

    world=m.look()
    points = { }
    for i in range(-8, 7):
        for j in range(-6, 7):
            # Give all points a height of 5 above sea level
            points['%ix%i'%(i,j)] = [i, j, 5]

    # Work out the ranges for the bounding box of the world.
    minx=0
    miny=0
    minz=0
    maxx=0
    maxy=0
    maxz=0
    for i in points.values():
        x = i[0]
        y = i[1]
        z = i[2]
        if not minx or x < minx:
            minx = x
        if not miny or y < miny:
            miny = y
        if not minz or z < minz:
            minz = z
        if not maxx or x > maxx:
            maxx = x
        if not maxy or y > maxy:
            maxy = y
        if not maxz or z > maxz:
            maxz = z
        
    m.set(world.id, terrain={'points' : points}, name="moraf", bbox=[minx * 64, miny * 64, minz, maxx * 64, maxy * 64, maxz])

def modify_terrain(host='', account='', password='', **args):
#   general things

    m=create_editor(host, account, password)

    world=m.look()

    _setup_terrain(m, world)
    m.make('ocean',pos=(0,0,0),bbox=[-500,-321,-20,320,321,0])

def _add_resources(m):

    # add more rocks around the map
    for i in range(0, 20):
        m.make('rock',pos=(uniform(-200,0),uniform(-200,0),settlement_height))


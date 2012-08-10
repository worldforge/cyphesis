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

#spawn_pos=(0,0,0)

# Name: default
# Synopsis: entry point into world creation (called by cyclient)
#
# Goals: create editor, setup call other setup items as needed
# Note: called from observer (not sure what that means)
def default(host='', account='', password='', **args):

    m=create_editor(host, account, password)

    world=m.look()

    if not hasattr(world, 'name'):
    	_setup_landscape(m, world)
	_setup_objects(m)

# Name _setup_landscape
# Synopsis: setup the landscape 
def _setup_landscape(m, world):
    """Set up the landscape environment of terrain, trees and natural
       features.
    """

    # Add the terrain surface to the world.
    _setup_terrain_basic(m, world)

    # If there is no boundary, add that.
    if m.look_for(type='boundary') is None:
        m.make('boundary',pos=(-500,-321,-20),bbox=[2,642,300])
        m.make('boundary',pos=(-500,-321,-20),bbox=[821,2,300])
        m.make('boundary',pos=(-500, 320,-20),bbox=[821,2,300])
        m.make('boundary',pos=( 320,-321,-20),bbox=[2,642,300])

def _setup_terrain_basic(m, world):

    world_width  = 10 
    world_length = 10

    # Fill in a matrix of height x width
    points = { }
    for i in range( -world_width, world_width ):
        for j in range( -world_length , world_length ):
            # Give all points a height of 5 above sea level
            points['%ix%i'%(i,j)] = [i, j, 5]

    # Work out the ranges for the bounding box of the world.
    minx=0
    miny=0
    minz=0
    maxx=0
    maxy=0
    maxz=0

    # iterate all points, record the min/max values for the  bounding box
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

    # Patterns:  where are these defined ? what assets ?
    #     fill - default all around
    #     band - probably around edges ... or a stipe type situation
    #     grass - suspect its random from foliage system
    #     depth - ???
    #     high  - ???
    # These names, correspond to the layer shaders defined in the media/shared/modeldefinitions/mars.terrain
    surfaces = [
        {'name': 'rock', 'pattern': 'fill' },
        {'name': 'mars', 'pattern': 'band', 'params': {'lowThreshold': -5.0,
                                                       'highThreshold': 110.0 } },
        #{'name': 'sand', 'pattern': 'band', 'params': {'lowThreshold':  -5.0,
        #                                               'highThreshold': 10.0 } },
    ]
        
    # The name of the terrain must also have a media/shared/modeldefinitions/<name>.terrain (i.e. mars.terrain )
    m.set(world.id, terrain={'points' : points, 'surfaces' : surfaces}, name="mars", bbox=[minx * 64, miny * 64, minz, maxx * 64, maxy * 64, maxz])
    points = {}
    surfaces = {}

    
def _setup_objects(m):

    m.make('rock',pos=(-2,-4,0))


#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 2005 WorldForge (See the file COPYING for details).
# Author: Miguel Guzman
#
# The goal of this map is to have a fancy map for demo purposes,
# to show in conventions like LinuxTag, and also to create nice screenshots.
# As a result, will allow for adding every media that our artists can create
# (for example there's a big hill with room for the castle and keep)
# It's not intented to be a mason game map, though the whole east side of the map
# can be used as a building site.
#
# Changes:
# (This is not a changelog, I'll delete older changes as to not clutter the file
# it's provided mostly as a convenience to see the latest changes)
# 2005-12-06 Created basic map and town

from atlas import *
try:
  from random import *
except ImportError:
  from whrandom import *
from mind.panlingua import interlinguish
il=interlinguish
from world import probability
from editor import editor
from Quaternion import Quaternion
from Vector3D import Vector3D
import time

# heights
town_height=0

#observer calls this
def default(mapeditor):
#   general things

    m=editor(mapeditor)

    world=m.look()

    points = { }

# terrain
    points['-6x-6'] = [-6, -6, 65]
    points['-5x-6'] = [-5, -6, 60]
    points['-4x-6'] = [-4, -6, -2]
    points['-3x-6'] = [-3, -6, -3]
    points['-2x-6'] = [-2, -6, -4]
    points['-1x-6'] = [-1, -6, -6]
    points['0x-6'] = [0, -6, -7]
    points['1x-6'] = [1, -6, -8]
    points['2x-6'] = [2, -6, -9]
    points['3x-6'] = [3, -6, -10]
    points['4x-6'] = [4, -6, -12]
    points['5x-6'] = [5, -6, -14]
    points['6x-6'] = [6, -6, -20]

    points['-6x-5'] = [-6, -5, 65.4]
    points['-5x-5'] = [-5, -5, 62.1]
    points['-4x-5'] = [-4, -5, 4]
    points['-3x-5'] = [-3, -5, 3.9]
    points['-2x-5'] = [-2, -5, 3.7]
    points['-1x-5'] = [-1, -5, 3.8]
    points['0x-5'] = [0, -5, 3.6]
    points['1x-5'] = [1, -5, 3.2]
    points['2x-5'] = [2, -5, 2.5]
    points['3x-5'] = [3, -5, 1]
    points['4x-5'] = [4, -5, 0]
    points['5x-5'] = [5, -5, -3]
    points['6x-5'] = [6, -5, -5]

    points['-6x-4'] = [-6, -4, 67]
    points['-5x-4'] = [-5, -4, 61]
    points['-4x-4'] = [-4, -4, 5]
    points['-3x-4'] = [-3, -4, 4.9]
    points['-2x-4'] = [-2, -4, 4.7]
    points['-1x-4'] = [-1, -4, 4.8]
    points['0x-4'] = [0, -4, 4.6]
    points['1x-4'] = [1, -4, 4.2]
    points['2x-4'] = [2, -4, 3.5]
    points['3x-4'] = [3, -4, 2]
    points['4x-4'] = [4, -4, 1]
    points['5x-4'] = [5, -4, 0]
    points['6x-4'] = [6, -4, -5]

    points['-6x-3'] = [-6, -3, 69]
    points['-5x-3'] = [-5, -3, 65]
    points['-4x-3'] = [-4, -3, 6]
    points['-3x-3'] = [-3, -3, 5.7]
    points['-2x-3'] = [-2, -3, 5.6]
    points['-1x-3'] = [-1, -3, 5.8]
    points['0x-3'] = [0, -3, 5.6]
    points['1x-3'] = [1, -3, 5.2]
    points['2x-3'] = [2, -3, 4.5]
    points['3x-3'] = [3, -3, 4.9]
    points['4x-3'] = [4, -3, 4.5]
    points['5x-3'] = [5, -3, 3]
    points['6x-3'] = [6, -3, 0]

    points['-6x-2'] = [-6, -2, 71]
    points['-5x-2'] = [-5, -2, 30] # keep
    points['-4x-2'] = [-4, -2, 30] # keep
    points['-3x-2'] = [-3, -2, 30] # keep
    points['-2x-2'] = [-2, -2, -16.8] # center of the lake
    points['-1x-2'] = [-1, -2, -3.8] # east side of the lake
    points['0x-2'] = [0, -2, 15.8] # east border of the lake
    points['1x-2'] = [1, -2, 13]
    points['2x-2'] = [2, -2, 10.5]
    points['3x-2'] = [3, -2, 7.9]
    points['4x-2'] = [4, -2, 6.5]
    points['5x-2'] = [5, -2, 5]
    points['6x-2'] = [6, -2, -4]

    points['-6x-1'] = [-6, -1, 70]
    points['-5x-1'] = [-5, -1, 64.6]
    points['-4x-1'] = [-4, -1, 30] # keep
    points['-3x-1'] = [-3, -1, 30] # keep
    points['-2x-1'] = [-2, -1, -2.8] # north part of the lake
    points['-1x-1'] = [-1, -1, 12.7] # town
    points['0x-1'] = [0, -1, 12.7] # town
    points['1x-1'] = [1, -1, 12.5] # town
    points['2x-1'] = [2, -1, 14.7]
    points['3x-1'] = [3, -1, 15.8]
    points['4x-1'] = [4, -1, 11]
    points['5x-1'] = [5, -1, 2]
    points['6x-1'] = [6, -1, -10]

    points['-6x0'] = [-6, 0, 72.4]
    points['-5x0'] = [-5, 0, 65.7]
    points['-4x0'] = [-4, 0, 30] # keep
    points['-3x0'] = [-3, 0, 30] #keep
    points['-2x0'] = [-2, 0, 20] # road to the keep (north border of the lake)
    points['-1x0'] = [-1, 0, 13.2] # town 
    points['0x0'] = [0, 0, 12.8] # town
    points['1x0'] = [1, 0, 12.5] # town
    points['2x0'] = [2, 0, 12]
    points['3x0'] = [3, 0, 10.5]
    points['4x0'] = [4, 0, 5]
    points['5x0'] = [5, 0, 0]
    points['6x0'] = [6, 0, -15]

    points['-6x1'] = [-6, 1, 75]
    points['-5x1'] = [-5, 1, 67.5]
    points['-4x1'] = [-4, 1, 15.2]
    points['-3x1'] = [-3, 1, 14.6]
    points['-2x1'] = [-2, 1, 14.5]
    points['-1x1'] = [-1, 1, 13.1] # town
    points['0x1'] = [0, 1, 12.9] # town
    points['1x1'] = [1, 1, 12.7] # town
    points['2x1'] = [2, 1, 12.5]
    points['3x1'] = [3, 1, 10.2]
    points['4x1'] = [4, 1, 8.9]
    points['5x1'] = [5, 1, 4]
    points['6x1'] = [6, 1, -9]

    points['-6x2'] = [-6, 2, 77]
    points['-5x2'] = [-5, 2, 68]
    points['-4x2'] = [-4, 2, 13] # mine road
    points['-3x2'] = [-3, 2, 55]
    points['-2x2'] = [-2, 2, 57.6]
    points['-1x2'] = [-1, 2, 14]
    points['0x2'] = [0, 2, 20]
    points['1x2'] = [1, 2, 14]
    points['2x2'] = [2, 2, 22.4]
    points['3x2'] = [3, 2, 18]
    points['4x2'] = [4, 2, 10.7]
    points['5x2'] = [5, 2, 6.4]
    points['6x2'] = [6, 2, -4]

    points['-6x3'] = [-6, 3, 81.2]
    points['-5x3'] = [-5, 3, 66]
    points['-4x3'] = [-4, 3, 13.2]
    points['-3x3'] = [-3, 3, 56.4]
    points['-2x3'] = [-2, 3, 60]
    points['-1x3'] = [-1, 3, 51.1]
    points['0x3'] = [0, 3, 19]
    points['1x3'] = [1, 3, 25.6]
    points['2x3'] = [2, 3, 10]
    points['3x3'] = [3, 3, 19.5]
    points['4x3'] = [4, 3, 11.3]
    points['5x3'] = [5, 3, 7]
    points['6x3'] = [6, 3, 0]

    points['-6x4'] = [-6, 4, 85]
    points['-5x4'] = [-5, 4, 66.7]
    points['-4x4'] = [-4, 4, 14.2]
    points['-3x4'] = [-3, 4, 56.4]
    points['-2x4'] = [-2, 4, 57]
    points['-1x4'] = [-1, 4, 52.5]
    points['0x4'] = [0, 4, 31]
    points['1x4'] = [1, 4, 15]
    points['2x4'] = [2, 4, 26.2]
    points['3x4'] = [3, 4, 22.7]
    points['4x4'] = [4, 4, 15]
    points['5x4'] = [5, 4, 10]
    points['6x4'] = [6, 4, -3.5]

    points['-6x5'] = [-6, 5, 91]
    points['-5x5'] = [-5, 5, 68]
    points['-4x5'] = [-4, 5, 65.5]
    points['-3x5'] = [-3, 5, 62.2]
    points['-2x5'] = [-2, 5, 58]
    points['-1x5'] = [-1, 5, 54]
    points['0x5'] = [0, 5, 50]
    points['1x5'] = [1, 5, 44.4]
    points['2x5'] = [2, 5, 45]
    points['3x5'] = [3, 5, 38.6]
    points['4x5'] = [4, 5, 12]
    points['5x5'] = [5, 5, 0]
    points['6x5'] = [6, 5, -8.7]

    points['-6x6'] = [-6, 6, 100]
    points['-5x6'] = [-5, 6, 85]
    points['-4x6'] = [-4, 6, 75]
    points['-3x6'] = [-3, 6, 70]
    points['-2x6'] = [-2, 6, 65]
    points['-1x6'] = [-1, 6, 66.7]
    points['0x6'] = [0, 6, 65]
    points['1x6'] = [1, 6, 64.3]
    points['2x6'] = [2, 6, 59.7]
    points['3x6'] = [3, 6, 55.3]
    points['4x6'] = [4, 6, 46.7]
    points['5x6'] = [5, 6, 12]
    points['6x6'] = [6, 6, -5.8]

    m.set(world.id, terrain={'points' : points}, name="moraf")

# a wall around the world

    m.make('boundary',type='boundary',xyz=(-384,-384,town_height),bbox=[2,514,256])
    m.make('boundary',type='boundary',xyz=(-384,-384,town_height),bbox=[514,2,256])
    m.make('boundary',type='boundary',xyz=(-384, 384,town_height),bbox=[514,2,256])
    m.make('boundary',type='boundary',xyz=(384,-384,town_height),bbox=[2,514,256])

# paths

    statue_path_area={'points' : [ [5,0], [4.33,2.5], [2.5,4.33], [0,5], [-2.5, 4.33], [-4.33,2.5], [-5,0], [-4.33,-2.5], [-2.5,-4.33], [0,-5.0], [2.5,-4.33], [4.33,-2.5], [5.0,0] ], 'layer' : 7}
    m.make('statue path',type='path',xyz=(0, 0, town_height), area=statue_path_area,bbox=[-5,-5,0,5,5,1])

    town_path_area={'points' : [ [50,0], [43.30,25], [25,43.30], [0,50], [-25, 43.30], [-43.30,25], [-50,0], [-43.30,-25], [-25, -43.30], [0, -50], [25,-43.30], [43.30,-25], [50,-0.01], [60,-0.01], [51.96,-30], [30,-51.96], [0, -60], [-30,-51.96], [-51.96,-30], [-60,0], [-51.96,30], [-30,51.96], [0,60], [30,51.96], [51.96,30], [60,0] ], 'layer' : 7}
    m.make('town path',type='path',xyz=(0, 0, town_height), area=town_path_area,bbox=[-38,-62,0,169,154,1])

    m.make('fir',type='fir',xyz=(0,0,10))

    m.make('fir',type='fir',xyz=(64,0,10))
    m.make('fir',type='fir',xyz=(128,0,10))
    m.make('fir',type='fir',xyz=(192,0,10))
    m.make('fir',type='fir',xyz=(256,0,10))
    m.make('fir',type='fir',xyz=(320,0,10))

    #dog = m.make('dog',type='dog',xyz=(5,5,10))
    #m.learn(dog,(il.patrol,"patrol(['t1', 't2'])"))
    #m.know(dog,testknowledge)
    cfire=m.make('campfire',type='campfire',xyz=(3,9,10))
    #m.make('fire',type='fire',xyz=(0,0,0),parent=cfire.id)
    #m.make('mausoleum', type='mausoleum', xyz=(40,40,10))
    
    # tower for the keep
    m.make('tower',type='tower',xyz=(-200,-120,30))
from atlas import *

from world.objects.Thing import Thing
from world.objects.plants.seeds.Seed import *
from common import log,const
#from world.objects.plants import prob
from misc import set_kw
from whrandom import *
from Vector3D import Vector3D

debug_tree = 0

"""
This is a simplified tree for use in acorn. I am going to strip out the
functionality we don't need.
I have stripped out anything to do with energy and growth, but kept
size. There will be some variation in sizes of trees, but they
won't grow.
"""

# This will later be moved into tree specific code #

# This is how fast the tree does it's stuff... use a low number for debugging a high number for real stuff
speed = 20


# This is the size the main goal stops from growing to reporducing #
sizeadult = 5

# Cordinate sizes. These are the left and right mutiples of each cordinates. To work out the actual size its the current codinate + size * the below number
  # X or width
x1 = 0.5
x2 = 0.5
  # Y or long axies  
y1 = 0.5
y2 = 0.5
  # Z or height
z1 = 0.5
z2 = 1.5
# These are the maximums widths between the 2 above cordinates
xMax = 5
yMax = 5
zMax = 40

# Chances
  # The chance for a flower to germinate
init_fruits = 20
fruitchance = 2
minudrop = 0
maxudrop = 2

def cs(a, b, max):
    if a+b > max:
       return a
    else:
       return max

def size_to_name(size):
    if size < 2:
        return "seedling"
    if size < 5:
        return "sapling"
    else:
        return "tree"

class Tree(Thing):
    #This base class for houses, building material is wood#
    def __init__(self, cppthing, **kw):
        self.base_init(cppthing, kw)
        
        # The default height
        set_kw(self,kw,"height",10)

        # Just a name for the client #
        set_kw(self,kw,"sizename",size_to_name(self.height))
        
        # The number of fruit #
        set_kw(self,kw,"fruits",init_fruits)

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
        
        # How big the tree is #
        #set_kw(self,kw,"size",randint(1, 30))
        set_kw(self,kw,"height",10)
        
        # Just a name for the client #
        set_kw(self,kw,"sizename",size_to_name(self.height))
        
        # The number of fruit #
        set_kw(self,kw,"fruits",init_fruits)

	# The type of fruit #
        set_kw(self,kw,"fruitname","Seed")

        # Chance of fruit #
        set_kw(self,kw,"fruitchance",fruitchance)

    def tick_operation(self, op):
        if debug_tree:
            print "I this big ", self.height
            print `self`,"Got tick operation:"
        #in any case send ourself next tick#
        opTick=Operation("tick",to=self)
        opTick.time.sadd=const.basic_tick*speed

        result = atlas.Message(opTick)

        fcount = self.fruits
        result = result + self.drop_fruit()

        if self.height > sizeadult :
            if randint(1, self.fruitchance) == 1:
                if debug_tree:
                    print "I'm growing a fruit!"
                self.fruits = self.fruits + 1
            else:
                if debug_tree:
                    print "I didn't grow a fruit"
        if fcount != self.fruits:
            if fcount == 0:
              newmode = "normal"
            else:
              newmode = "fruit"
            result = result + Operation("set",Entity(self.id,fruits=self.fruits,mode=newmode),to=self)
        return result



    def drop_fruit(self):
        result = atlas.Message()
        if self.fruits > 0:
            rand = randint(minudrop, maxudrop)
            rand = min(rand,self.fruits)
            self.fruits = self.fruits - rand
            for x in range(rand):
                # pick a random spot between the 2 extremes of the plant
                if debug_tree:
                    print "Creating Fruit"
                randx = uniform ( self.location.coordinates.x - cs( x1 * self.height, x2 * self.height, xMax) , self.location.coordinates.x + cs( x2 * self.height, x1 * self.height, xMax) )
                randy = uniform ( self.location.coordinates.y - cs( y1 * self.height, y2 * self.height, yMax) , self.location.coordinates.y + cs( y2 * self.height, y1 * self.height, yMax) )
                #randz = uniform ( self.location.coordinates.z - cs( z1 * self.size, z2 * self.size, zMax) , self.location.coordinates.z + cs( z2 * self.size, z1 * self.size, zMax) )
                randz = 0
                if hasattr(self,"coords_modify"):
                    randx,randy,randz=coords_modify(randx,randy,randz)
                fruit=Entity(name=self.fruitname,location=Location(self.world,Vector3D(randx,randy,randz)),parents=[self.fruitname])
                result = result + Operation("create",fruit,to=self)
                if debug_tree:
                    print fruit
        return result


from atlas import *

from world.objects.Thing import Thing
from common import log,const
#from world.objects.plants import prob
from misc import set_kw
from whrandom import *
from Vector3D import Vector3D

debug_tree = 0

"""
energy per tick durning day = (1/2water) * soil quality * leaf coverage%
energy usage per tick = tree size * (1/2 leaf coverage%)
energy per tick of growth = (tree size * new tree size) 1/2
energy per tick in hibanation = 1/4 tree size
energy to grow flower = flowerusage * 2
energy to grow fruit = fruitusage * 3 - flowerusage
energy to mantain flower = flowerusage * numberofflowers
energy to mantain fruit = fruitusage * numberoffruit
max leaves = amountofleaves * size
"""

# This will later be moved into tree specific code #

# This is how fast the tree does it's stuff... use a low number for debugging a high number for real stuff
speed = 2

# Speed of growth
growthspeed = 6

# This is the size the main goal stops from growing to reporducing #
sizeadult = 10

# Cordinate sizes. These are the left and right mutiples of each cordinates. To work out the actual size its the current codinate + size * the below number
  # X or up axies (tall)
x1 = 1
x2 = 3
  # Y or long axies  
y1 = 1
y2 = 1
  # Z or depth
z1 = 1
z2 = 1
# These are the maximums widths between the 2 above cordinates
xMax = 40
yMax = 10
zMax = 10

#seed/fruit object
fruitname = "Acorn"

#The upkeep costs
  # Size
supkeep = 10
  # Leaf
lupkeep = 2
  # Flower
flupkeep = 10
  # Fruit
fuupkeep = 15

# Maximums
  # Leaves
amountofleaves = 10
  # Fruit
amountoffruits = 2
  # Flowers
amountofflowers = 7

# Energy Usage
  # Growth
growthusage = 100
  # Flower growth
flowerusage = 10
  # Fruit Growth
fruitusage = 2 * flowerusage
  # Leaf growth
leafusage = 0
  # Reaping Cost
repaircost = 10

# Energy Gain
  # Leaf energy produce
leafenergy = 10
  #The amount it cost to remove water/nurtrients from the ground
extraction_cost = 10

# Soil quality
soil_quality = 20

# Season
  # The season flower starts
flowerseason = "winter"
  # The season the tree drops its leaves
fallseason = "autumn"
  # The season the tree dorminates
dormantseason = "summer"

#The maximum amount of leaves to drop, this will be times by 3 for fallseason and times by health% to find out how many drop ( this is per tick )
  # leaves
maxldrop = 10
  # flowers
maxfdrop = 1
  # Fruit
maxudrop = 1

#The minimum amount of stuff that will drop per tick
  # leaves
minldrop = 0
  # flowers
minfdrop = 0
  # Fruit
minudrop = 1

# Chances
  # The chance for a flower to germinate
fruitchance = 1

def cs(a, b, max):
    if a+b > max:
       return a
    else:
       return max

class Tree(Thing):
    #This base class for houses, building material is wood#
    def __init__(self, cppthing, **kw):
        self.base_init(cppthing, kw)
        # How fast the tree will burn #
        set_kw(self,kw,"burn_speed",0.2)
        
        # The material the tree is made out of #
        set_kw(self,kw,"material","green_wood")
        
        # How big the tree is #
        set_kw(self,kw,"size",1)
        
        # Just a name for the client #
        set_kw(self,kw,"sizename","sapling")
        
        # The amount of food the tree has to use #
        set_kw(self,kw,"energy",20)
        
        # The number of flowers #
        set_kw(self,kw,"flowers",0)
        
        # The number of flowers #
        set_kw(self,kw,"fruits",0)
        
        # The health of the tree #
        set_kw(self,kw,"health",100)
        
        # The no of leaves #
        set_kw(self,kw,"leaves",1)
        
        # sets the last growth time #
        set_kw(self,kw,"last_growth",0)
        
        # The amount of water the tree has
        set_kw(self,kw,"water",1)

      
    def tick_operation(self, op):
        if debug_tree:
            print "I this big ", self.size
            print "I have so much energy ", self.energy
            print "I have so many leaves ", self.leaves
            print `self`,"Got tick operation:"
        #in any case send ourself next tick#
        opTick=Operation("tick",to=self)
        opTick.time.sadd=const.basic_tick*speed

        result = Message(opTick)

        #CHEAT!: handle these cases correctly
        #see another "self.healt<0" -test
        print self.health
        if self.health<0:
            #when self.health==-6
            #then-1 == maxldrop * self.health / 100 
            #and this causes exception:
            #(it calls randint(0,-1))
            #new_leaves = randint(minldrop, maxldrop * self.health / 100 )
            return

        if self.health < -100:
            if debug_tree:
                print "Kill me now!"

        elif self.energy < 0:
            if debug_tree:
                print "Tree is dieing", self.health
            self.health = self.health + self.energy
            #CHEAT: see previous "self.health<0" -test
            if self.health<0:
                return
        elif self.health < 100 and self.energy > repaircost:
            if debug_tree:
                print "The tree is damaged, repairing"
            self.energy = self.energy - repaircost
            self.health = self.health + 1
        else:
            if debug_tree:
                print "Tree is all well"


        # add energy #
        if self.world.get_time()=="night":
            if debug_tree:
                print "Its night"
            # Put your night stuff in this operation 
            if hasattr(self,"do_night_things"):
                result = result + self.do_night_things()
        else:
           # Put you day stuff in this operation 
           if debug_tree:
               print "Its day"
           if hasattr(self,"do_day_things"):
                result = result + self.do_day_things()
           if debug_tree:
               print "Water ", self.water, " Soil ", soil_quality, " Leafenergy ", leafenergy
               print "I get his much energy", self.water * soil_quality / extraction_cost * self.leaves * leafenergy
           self.energy = self.energy + self.water * soil_quality / extraction_cost * self.leaves * leafenergy

        # Useage per tick for general stuff..
        # Size upkeep
        if debug_tree:
            print "Size upkeep ", self.size * supkeep
        self.energy = self.energy - self.size * supkeep
        # Leaf upkeep
        if debug_tree:
            print "Leaf upkeep ", self.leaves * lupkeep
        self.energy = self.energy - self.leaves * lupkeep
        # Flower upkeep
        if debug_tree:
            print "Flowers upkeep ", self.flowers * flupkeep
        self.energy = self.energy - self.flowers * flupkeep
        # Fruit upkeep
        if debug_tree:
            print "Fruit upkeep ", self.fruits * fuupkeep
        self.energy = self.energy - self.fruits * fuupkeep

        # Should people be informed of leave changes?
        if debug_tree:
            print self.world.get_time().season, " is the current season"
        if not (self.world.get_time() == dormantseason):
           if self.world.get_time() == fallseason :
              
              #Fall season is when stops the growing of leaves and 
              if self.leaves > 0:
                 if debug_tree:
                     print "Kill lotsa leaves"
                 self.leaves = self.leaves - randint(minldrop, maxldrop * 3 )
              else:
                 if debug_tree:
                     print "I have no leaves left"
                 self.leaves = 0
                 
              if self.flowers > 0:
                 self.flowers = self.flowers - randint(minfdrop, maxfdrop * 10 )
              else:
                 self.flowers = 0

              if self.fruits > 0:
                 rand = randint(minudrop, maxudrop * self.health / 100 )
                 self.fruits = self.fruits - rand
                 for x in range(rand):
                    # pick a random spot between the 2 extremes of the plant
                    if debug_tree:
                        print "Creating Fruit"
                    randx = uniform ( self.location.coordinates.x - cs( x1 * self.size, x2 * self.size, xMax) , self.location.coordinates.x + cs( x2 * self.size, x1 * self.size, xMax) )
                    randy = uniform ( self.location.coordinates.y - cs( y1 * self.size, y2 * self.size, yMax) , self.location.coordinates.y + cs( y2 * self.size, y1 * self.size, yMax) )
                    randz = uniform ( self.location.coordinates.z - cs( z1 * self.size, z2 * self.size, zMax) , self.location.coordinates.z + cs( z2 * self.size, z1 * self.size, zMax) )

                    if hasattr(self,"coords_modify"):
                       randx,randy,randz=coords_modify(randx,randy,randz)
                    
                    fruit=Entity(location=Location(self.world,Vector3D(randx,randy,randz)),type=[fruitname])
                    result = result + Operation("create",fruit,to=self)
                 
                    if debug_tree:
                        print fruit
                 
              else:
                 self.fruits = 0

              
           else:
              if debug_tree:
                  print "It's normal growing time"
              # simulate the falling of leaves and the growing of new ones, aswell it kills of extra flowers        
              if self.leaves > 0:
                 dleaves = randint(minldrop,
                                   int(maxldrop * (1.0 - self.health / 100.0)) )
                 if debug_tree:
                     print dleaves, "are going to die"
                 self.leaves = self.leaves - dleaves
              else:
                 if debug_tree:
                     print "I have no leaves left"
                 self.leaves = 0

              if self.flowers > 0:
                 self.flowers = self.flowers - randint(minfdrop, int(maxldrop * (1.0 - self.health / 100.0)) )
              else:
                 self.flowers = 0

              if self.fruits > 0:
                 if debug_tree:
                     print "Bombs away ",
                 drandf = randint(minudrop, maxudrop * self.health / 100 )
                 self.fruits = self.fruits - drandf
                 if debug_tree:
                     print drandf
                 for x in range(drandf):
                    if debug_tree:
                        print "Creting new fruit"
                    # pick a random spot between the 2 extremes of the plant
                    randx = uniform ( self.location.coordinates.x - cs( x1 * self.size, x2 * self.size, xMax) , self.location.coordinates.x + cs( x2 * self.size, x1 * self.size, xMax) )
                    randy = uniform ( self.location.coordinates.y - cs( y1 * self.size, y2 * self.size, yMax) , self.location.coordinates.y + cs( y2 * self.size, y1 * self.size, yMax) )
                    randz = uniform ( self.location.coordinates.z - cs( z1 * self.size, z2 * self.size, zMax) , self.location.coordinates.z + cs( z2 * self.size, z1 * self.size, zMax) )

                    if hasattr(self,"coords_modify"):
                       randx,randy,randz=coords_modify(randx,randy,randz)
                       
                    fruit=Entity(location=Location(self.world,Vector3D(randx,randy,randz)),type=[fruitname])
                    result = result + Operation("create",fruit,to=self)
                 
                 
                 ### Create a fruit on the ground ###

              else:
                 self.fruits = 0

#              result = result + Operation("set",Entity(self.id,leaves=self.leaves),to=self)
#              result = result + Operation("set",Entity(self.id,flowers=self.flowers),to=self)
#              result = result + Operation("set",Entity(self.id,fruits=self.fruits),to=self)
              
              if debug_tree:
                  print self.leaves
              new_leaves = randint(minldrop, maxldrop * self.health / 100 )
              if self.leaves < ( amountofleaves * self.size ) and self.energy > ( leafusage * new_leaves ) :
                 if debug_tree:
                     print "I'm growing a new leaf", new_leaves,
                 self.leaves = self.leaves + new_leaves
                 if debug_tree:
                     print " ", self.leaves
                 self.energy = self.energy - ( leafusage * new_leaves )

        else:
          pass

        seconds=self.world.get_time().seconds()
        
        if debug_tree:
            print self.last_growth, " time last growth ", seconds, " time now"
            print self.last_growth + growthspeed * self.size * 1000, "till next growth"

        if self.world.get_time() != dormantseason:

           if debug_tree:
               print "Time to check to see if the tree can grow"
           if self.size < sizeadult :
             if seconds > (self.last_growth + growthspeed * self.size * 1000 ):
               if debug_tree:
                   print "I'm only small so i better grow"
                   print " self.energy ", self.energy, " growthusage ", growthusage, " self.size ", self.size, " ",
                   print self.energy > growthusage * self.size
               if self.energy > ( growthusage * self.size ) :

                  if debug_tree:
                      print "I'm going to grow!"
                  self.last_growth = seconds
                  self.energy = self.energy - growthusage
                  result = result + Operation("set",Entity(self.id,size=self.size+1),to=self)

           else:

               if  self.flowers < amountofflowers * self.size and self.world.get_time() == flowerseason and self.energy > flowerusage :
                 if debug_tree:
                     print "I'm going to grow a flower"
                 self.energy = self.energy - flowerusage
                 self.flowers = self.flowers + 1
                                
               else:
                 if debug_tree:
                     print "I already have enough flowers or is isn't spring"

                 if self.flowers > 0 and self.fruits < amountoffruits * self.size and self.energy > fruitusage :
                    if debug_tree:
                        print " I'm going to try and grow a fruit!"
                    if randint(1, fruitchance) == 1 :
                       if debug_tree:
                           print "I'm growing a fruit!"
                       self.flowers = self.flowers - 1
                       self.fruits = self.fruits + 1
                       self.energy = self.energy - fruitusage
                    else:
                        if debug_tree:
                            print "I didn't grow a fruit"

                 else:
                    if debug_tree:
                        print "I already have enough fruit"
                    if self.energy > growthusage and ( seconds > (self.last_growth + growthspeed * self.size * 1000 ) ) :
                    
                       if debug_tree:
                           print "I'm going to grow bigger!"
                       self.last_growth = seconds
                       self.energy = self.energy - growthusage
                       result = result + Operation("set",Entity(self.id,size=self.size+1),to=self)

                       
        print type(result)
        return result                          
              #check for desiese#
                 # reduce health count #
           #check to see if we have dieded because of lack of water etc


    def water_operation(self, op):
        #If somebody tends to us we grow much faster#
        pass
        
    def trim_operation(self, op):
        #If somebody tends to us we grow much faster#
        pass

    def shake_operation(self, op):
        pass
        
    def chop_operation(self, op):
        new_status=self.status-0.2
        ent=Entity(self.id,status=new_status)
        if new_status>=0:
            return Operation("set",ent,to=self)
        return Message(Operation("set",ent,to=self),Operation("create",Entity(name='lumber',type=['lumber'],location=self.location.parent.location.copy()),to=self))

    def rain_operation(self, op):
        pass

    def pick_flower_operation(self, op):
        pass

    def pick_fruit_operation(self, op):
        pass

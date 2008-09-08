#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 1999 Aloril (See the file COPYING for details).

from atlas import *
try:
  from random import *
except ImportError:
  from whrandom import *
from mind.panlingua import interlinguish
il=interlinguish
from cyphesis import probability
from cyphesis.editor import editor
from Quaternion import Quaternion
from Vector3D import Vector3D
import time
from math import *

#goal priority
#1) eating: certain times
#2) market/tavern: certain times, certain probability
#3) sleeping: nights
#4) chop trees: winter (when not doing anything else)
#4) other similar tasks: seasonal (-"-)

#'Breakfast' goal is type of 'eating'.

# Heights are all 0 for now, as entity positions will be adjusted to terrain
# height.
settlement_height=0
forest_height=0
graveyard_height=0


hall_pos=(5,3,settlement_height)
forest_pos=(-20,-60,settlement_height)

pig_sty_pos=(8,8,settlement_height)
butcher_stall_pos=(155,140,settlement_height)
butcher_pos=(153,142,settlement_height)
tool_stall_pos=(150,138,settlement_height)
tool_merchant_pos=(150,140,settlement_height)
tailor_stall_pos=(144,154,settlement_height)
tailor_pos=(142,152,settlement_height)
mausoleum_pos=(-160, 105, graveyard_height)

camp_pos=(100,-50,22)

lake_pos=(-40,-70,0)

knowledge=[('axe','place','smithy'),
           ('forest','location',forest_pos),
           ('hall','location',hall_pos)]
mprices=[('pig','price','5')]
bprices=[('ham','price','2')]
bknowledge=[('market','location',butcher_stall_pos)]
mknowledge=[('market','location',pig_sty_pos)]
sknowledge=[('forest','location',forest_pos),
            ('stash','location',(-98,-97,settlement_height))]
area=[('mausoleum','location', mausoleum_pos),
      ('butcher','location', butcher_stall_pos),
      ('pig','location', 'sty'),
      ('sty','location', pig_sty_pos)]
about=[('acorn','about','Acorns are pigs favorite food.'),
       ('apple','about','Pigs love eating apples, when they can find them.'),
       ('building','about','You can build a house here once you have earned a claim to some land.'),
       ('claim','about','Once you have earned your claim you can use it to make out some land by placing it on the ground.'),
       ('forest','about','The forest is full of wolves and skeletons that are dangerous to your pigs. Be careful.'),
       ('herding','about','You can herd pigs in the forest where the oak trees grow.'),
       ('jetty','about','Ships from the nearest city come into the jetty occassionally.'),
       ('land','about','When you have contributed to our settlement, you will be rewarded with some land.'),
       ('mausoleum','about','The mausoleum was here when we discovered these lands, and seems to be hundreds of years old. No one knows who built it.'),
       ('oak','about','Oak trees drop acorns which pigs love to eat. You can also gather strong wood from oak trees suitable for building.'),
       ('pig','about','You can buy pigs from the sty, and sell them to the butcher once you have helped them grow big enough.'),
       ('pig','food','acorn'),
       ('settlement','about','This settlement is in the frontier of our lands. We need lots of help to get established.'),
       ('sty','about','The pig sty is where the merchant keeps the young pigs he has for sale.'),
       ('skeleton','about','It is said that the undead haunt the area round the ancient mausoleum nearby.'),
       ('wolf','about','Wolves live in the forest, and will eat pigs if they can catch them. However they prefer ham if they can get it.')
      ]
gknowledge=[('m1','location',(-17, -1,    settlement_height)),
            ('m2','location',(-29, -1,    settlement_height)),
            ('m3','location',(-29, -7.5,  settlement_height)),
            ('m4','location',(-38, -10,   settlement_height)),
            ('m5','location',(-43, -15,   settlement_height)),
            ('m6','location',(-43, -14.5, settlement_height))]

wolf_knowledge=[('w1','location',(90,-90,settlement_height)),
                ('w2','location',(110,-90,settlement_height)),
                ('w3','location',(110,90,settlement_height)),
                ('w4','location',(90,90,settlement_height))]

lych_knowledge=[('w1','location',(0,-96,settlement_height)),
                ('w2','location',(-70,-70,settlement_height)),
                ('w3','location',(-100,70,settlement_height)),
                ('w4','location',(-147,-90,settlement_height))]

wander=(il.wander,"wander()")
forage=(il.forage,"forage()")
trade=(il.trade,"trade()")
keep=(il.keep,"keep()")
sell=(il.sell,"sell_trade()")
patrol=(il.patrol,"patrol()")

# Things for Blackum Decker to sell need to be stated here with prices
toolprices = [('bowl', 'price', '3'),
              ('pole', 'price', '4'),
              ('hammer', 'price', '4'),
              ('bottle', 'price', '4'),
              ('cleaver', 'price', '3'),
              ('trowel', 'price', '3'),
              ('axe', 'price', '5'), 
              ('shovel', 'price', '5'),
              ('pickaxe', 'price', '6'),
              ('scythe', 'price', '7'),
              ('tinderbox', 'price', '8'),
              ('bucksaw', 'price', '10')]

toolmerc_knowledge=[('market', 'location', tool_stall_pos)]

tailor_prices = [('shirt', 'price', '5'), 
                 ('trousers', 'price', '5'),
                 ('cloak', 'price', '5')]

tailor_knowledge=[('market', 'location', tailor_stall_pos)]

pig_goals=[(il.avoid,"avoid(['wolf','skeleton','crab'],10.0)"),
           (il.forage,"forage('acorn')"),
           (il.forage,"forage('apple')"),
           (il.forage,"forage('mushroom')"),
           (il.flock,"driven()"),
           (il.peck,"summons('foo')"),
           (il.herd,"herd()")]

deer_goals=[(il.avoid,"avoid(['settler','orc'],30.0)"),
            (il.avoid,"avoid(['fire'],10.0)"),
            (il.forage,"forage('apple')"),
            (il.forage,"forage('mushroom')"),
            (il.browse,"browse('fir', 0.8)"),
            (il.flock,"flock()")]

chicken_goals=[(il.avoid,"avoid(['orc','wolf'],10.0)"),
               (il.avoid,"avoid(['settler'],1.0)"),
               (il.flock,"flock()"),
               (il.peck,"peck()")]

squirrel_goals=[(il.avoid,"avoid(['wolf','crab'],10.0)"),
                (il.forage,"forage('acorn')"),
                (il.forage,"forage('pinekernel')")]

wolf_goals=[(il.forage,"forage('ham')"),
            (il.hunt,"predate('pig',30.0)"),
            (il.hunt,"predate('crab',20.0)"),
            (il.hunt,"predate('squirrel',10.0)"),
            (il.patrol,"patrol(['w1', 'w2', 'w3', 'w4'])")]

crab_goals=[(il.avoid,"avoid('wolf',10.0)"),
            (il.hunt,"predate_small('pig',30.0,10.0)")]

lych_goals=[(il.assemble, "assemble(self, 'skeleton', ['skull', 'ribcage', 'arm', 'pelvis', 'thigh', 'shin'])"),
            (il.patrol,"patrol(['w1', 'w2', 'w3', 'w4'])")]

skeleton_goals=[(il.hunt,"chase(['pig', 'farmer', 'guard', 'settler'])")]


# N, E, S, W, NE, SE, SW, NW in order
directions = [[0,0,0.707,0.707],[0,0,0,1],[0,0,-0.707,0.707],[0,0,1,0],
              [0,0,0.387,0.921],[0,0,-0.387,0.921],[0,0,-0.921,0.387],[0,0,0.921,0.387]]

forests = [
           ('oak', 30, 20, 190, -200, 100, 20),
           ('oak', 20, -200, -100, 0, 190, 20),
           ('fir', 200,  200,  300, -300, 300, 50),
           ('fir', 200, -300, 300,  200,  300, 50)
          ]

tree_styles = {
            'oak' : ['gnarly', 'knotted', 'weathered'], 
            'fir' : ['gnarly', 'knotted', 'weathered']
            }

#observer calls this
def default(mapeditor):
#   general things

    m=editor(mapeditor)

    world=m.look()

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

# a wall around the world

    m.make('boundary',pos=(-500,-321,-20),bbox=[2,642,300])
    m.make('boundary',pos=(-500,-321,-20),bbox=[821,2,300])
    m.make('boundary',pos=(-500, 320,-20),bbox=[821,2,300])
    m.make('boundary',pos=( 320,-321,-20),bbox=[2,642,300])

    m.make('fir',pos=(-10,-0,settlement_height), style = tree_styles['fir'][randint(0,len(tree_styles['fir']) - 1)])
    m.make('fir',pos=(-0,-10,settlement_height), style = tree_styles['fir'][randint(0,len(tree_styles['fir']) - 1)])
    m.make('fir',pos=(0,10,settlement_height), style = tree_styles['fir'][randint(0,len(tree_styles['fir']) - 1)])
    m.make('fir',pos=(10,0,settlement_height), style = tree_styles['fir'][randint(0,len(tree_styles['fir']) - 1)])

    path_area={'points' : [ [-26,-62], [-36,-31], [-26,-14], [2,-1], [22, 40], [132,122], [140,127], [144.5, 146.5], [169, 153], [169,155], [142.5,148.5], [138,129], [130,124], [18,40], [-2, 0], [-28,-12], [-38,-29], [-29,-62] ], 'layer' : 7}
    m.make('path', name='path to village',pos=(10, 20,settlement_height), area=path_area,bbox=[-38,-62,0,169,154,1])

    m.make('tower',pos=(210,210,5))
    m.make('gallows',pos=(185,175,5))

    m.make('house3',pos=(158,150,22),orientation=directions[1])
    m.make('house3',pos=(158,158,22),orientation=directions[4])
    m.make('house3',pos=(150,158,22),orientation=directions[0])
    m.make('house3',pos=(142,158,22),orientation=directions[7])
    m.make('house3',pos=(142,150,22),orientation=directions[3])
    m.make('house3',pos=(142,142,22),orientation=directions[6])
    m.make('house3',pos=(150,125,22),orientation=directions[2])
    m.make('house3',pos=(171,142,22),orientation=directions[5])

    carrotfield_points = [ [0,0], [0,20], [20,20], [20,0] ] # this is so these values can be reused as an argument in plantfield as well as in 'points' in the m.make command
    carrotfield = m.make('ploughed_field', name='field',pos=(120,170,30),status=1.0,area={'points' : carrotfield_points, 'layer' : 8}, bbox=[20,20,0]) # this makes a carrot field storing its identity in carrotfield
    turnipfield_points = [ [0,0], [0,20], [20,20], [20,0] ]
    turnipfield = m.make('ploughed_field', name='field',pos=(142,170,30),status=1.0,area={'points' : turnipfield_points, 'layer' : 8}, bbox=[20,20,0])

    plantfield(m,'carrot',carrotfield,carrotfield_points)# calls the plantfield funtion with (the entity to be planted, the identity of the field to be planted in, the distance between the furrows, the distance between the plants along the furrows, the points of the corners of the field)
    plantfield(m,'turnip',turnipfield,turnipfield_points)

    village_square={'points': [[-10, -14], [15, -11], [13,18], [-8, 11]], 'layer':7 }
    m.make('path', name='village_square', pos=(150, 150, 22), area=village_square, bbox=[-10, -14, 0, 15, 18, 1])

    # m.make('forest', name='sherwood',pos=(-50, 10,settlement_height),bbox=[40,40,40])

    m.make('jetty',pos=(-22,-48,0))
    m.make('boat',pos=(-22,-56,0))

# a camp near the origin

    #cfire=m.make('campfire',pos=(0,4,settlement_height))
    #m.make('fire',pos=(0.7,0.7,0),parent=cfire.id)
    #m.make('tent',pos=(-1,8,settlement_height),bbox=[2.5,2.5,3])
    #m.make('lumber',pos=(-1,3,settlement_height))
    #m.make('lumber',pos=(-1,2.5,settlement_height))

    # hall=m.make('hall',pos=hall_pos)

    # Fire in the centre of the hall
    # cfire=m.make('campfire',pos=(6,6,settlement_height),
                                            # parent=hall.id)
    # m.make('fire',pos=(0.7,0.7,0),parent=cfire.id)

    cfire=m.make('campfire',pos=(3,9,settlement_height))
    m.make('fire',pos=(0,0,0),parent=cfire.id)

    cfire=m.make('campfire',pos=(11,1,settlement_height))
    m.make('fire',pos=(0,0,0),parent=cfire.id)

    for i in range(0, 20):
        m.make('lumber',pos=(uniform(-200,0),uniform(-200,0),settlement_height))

    for i in forests:
        for j in range(0, i[1]):
            m.make(i[0],pos=(uniform(i[2],i[3]),uniform(i[4],i[5]),i[6]), orientation=directions[randint(0,7)], style = tree_styles[i[0]][randint(0,len(tree_styles[i[0]]) - 1)])

    m.make('weather', pos=(0,1,0))

#   bones all over the place
    for i in range(0, 10):
        xpos = uniform(-200,200)
        ypos = uniform(-200,200)
        m.make('skull', pos=(xpos+uniform(-2,2),ypos+uniform(-2,2),settlement_height))
        m.make('pelvis', pos=(xpos+uniform(-2,2),ypos+uniform(-2,2),settlement_height))
        m.make('arm', pos=(xpos+uniform(-2,2),ypos+uniform(-2,2),settlement_height))
        m.make('thigh', pos=(xpos+uniform(-2,2),ypos+uniform(-2,2),settlement_height))
        m.make('shin', pos=(xpos+uniform(-2,2),ypos+uniform(-2,2),settlement_height))
        m.make('ribcage', pos=(xpos+uniform(-2,2),ypos+uniform(-2,2),settlement_height))

#   the lych, who makes bones into skeletons
    lych=m.make('lych', pos=(-21, -89, settlement_height))
    m.learn(lych,lych_goals)
    m.know(lych,lych_knowledge)
    m.tell_importance(lych,il.assemble,'>',il.patrol)

#   animals
    piglet = m.make('pig', pos=(-3,-1,settlement_height))
    m.learn(piglet,pig_goals)

    wolf = m.make('wolf', pos=(90,-90,settlement_height))
    m.learn(wolf,wolf_goals)
    m.know(wolf,wolf_knowledge)
    m.tell_importance(wolf,il.forage,'>',il.hunt)
    m.tell_importance(wolf,il.forage,'>',il.patrol)
    m.tell_importance(wolf,il.hunt,'>',il.patrol)

    crab = m.make('crab', pos=(-90,90,settlement_height))
    m.learn(crab,crab_goals)

    skeleton = m.make('skeleton', pos=(-38,-25,settlement_height))
    m.learn(skeleton,skeleton_goals)

    squirrel = m.make('squirrel', pos=(-32,-15,settlement_height))
    m.know(squirrel,sknowledge)
    m.learn(squirrel,squirrel_goals)

#   villagers
    #directions = [[0,1,0],[1,0,0],[0,-1,0],[-1,0,0],
                  #[0.7,0.7,0],[0.7,-0.7,0],[-0.7,-0.7,0],[-0.7,0.7,0]]

    # An NPC settler
    settler=m.make('settler',pos=(1,1,0))
    axe=m.make('axe',pos=(0,0,0),parent=settler.id)
    m.own(settler,axe)
    m.know(settler,[('forest','location',(30,30,0))])
    m.learn(settler,(il.trade,"harvest_resource('lumber','oak','forest','axe')"))

    # An NPC forester
    settler=m.make('settler', name='forester',pos=(0, 12, 0))
    trowel=m.make('trowel',pos=(0,0,0),parent=settler.id)
    m.own(settler, trowel)
    m.know(settler,[('forest','location',(30,30,0))])
    m.learn(settler,(il.trade,"plant_seeds('acorn','oak','forest','trowel')"))

    home1_pos=(90,-90,settlement_height)
    
    stall=m.make('stall', name='Market Stall',
                 pos=butcher_stall_pos, orientation=directions[5])
    m.make('wall',parent=stall.id,pos=(0,0,0),bbox=(-0.5,-1.5,0,0.5,1.5,0.8))
    m.make('wall',parent=stall.id,pos=(2,0,0),bbox=(0,-1.5,0,0.5,1.5,2))

    # An NPC Butcher
    butcher=m.make('settler', name='Ulad Bargan',desc='the butcher',
                   pos=butcher_pos,age=probability.fertility_age)
    m.learn(butcher,(il.trade,"trade('pig', 'cleaver', 'ham', 'market')"))
    m.learn(butcher,(il.buy_livestock,"buy_livestock('pig', 1)"))
    m.learn(butcher,(il.market,"run_shop('mstall_freshmeat_1_se','open','dawn')"))
    m.learn(butcher,(il.market,"run_shop('mstall_freshmeat_1_se','closed','evening')"))
    m.learn(butcher,(il.help,"add_help(['I need live pigs to restock my butcher stall.','Do you have any pigs for sale?','I prefer pigs that have grown to more than 20kg.'])"))
    m.know(butcher,bknowledge)
    m.know(butcher,bprices)

    
    cleaver=m.make('cleaver', place='market', pos=(0, 0, 0), parent=butcher.id)
    m.own(butcher,cleaver)
    m.own(butcher, stall)
    
    m.learn(butcher,(il.sell,"sell_trade('ham', 'market')"))
    coins=[]
    for i in range(0, 60):
        coins.append(m.make('coin',pos=(0,0,0),parent=butcher.id))
    m.own(butcher,coins)
    
# 	clothing-merchant
    stall = m.make('stall', name='Tailor Stall', pos=tailor_stall_pos, orientation=directions[7])
    m.make('wall',parent=stall.id,pos=(0,0,0),bbox=(-0.5,-1.5,0,0.5,1.5,0.8))
    m.make('wall',parent=stall.id,pos=(2,0,0),bbox=(0,-1.5,0,0.5,1.5,2))

    tailor=m.make('merchant', name='Bok Forgo',desc='the tailor',
                  pos=tailor_pos,age=probability.fertility_age)
    m.learn(tailor,(il.help,"add_help(['Get your clothes here.','Everything to keep you looking your best is here.'])"))
    
    m.know(tailor, tailor_knowledge)
    m.know(tailor, tailor_prices)

    m.own(tailor, stall)

    clothes=[]
    garment_types=['shirt', 'trousers', 'cloak']
    garment_styles=['ragged', 'burlap', 'sun', 'fine']

    for i in range(0, 15):
        gty = garment_types[randint(0, len(garment_types)-1)]
        gsty = garment_styles[randint(0, len(garment_styles)-1)]
        gname = '%s %s' % (gsty, gty)
        clothes.append(m.make(gty,name=gname,pos=(0, uniform(-0.7, 0.7), 0), parent=stall.id))

    m.own(tailor, clothes)

# 	tool-merchant
    stall = m.make('stall', name='Tool Stall', pos=tool_stall_pos, orientation=directions[2])
    m.make('wall',parent=stall.id,pos=(0,0,0),bbox=(-0.5,-1.5,0,0.5,1.5,0.8))
    m.make('wall',parent=stall.id,pos=(2,0,0),bbox=(0,-1.5,0,0.5,1.5,2))

    tmerchant=m.make('merchant', name='Blackun Decker',desc='the tool merchant',
                     pos=tool_merchant_pos,age=probability.fertility_age)
    m.learn(tmerchant,(il.help,"add_help(['Get all your tools here.','Everything a settler needs is available at great prices.'])"))

    m.know(tmerchant, toolmerc_knowledge)
    m.know(tmerchant, toolprices)

    m.own(tmerchant, stall)

    tools=[]
    # the different tools must be stated here \|/
    tooltypes=['shovel', 'axe', 'tinderbox', 'bowl', 'pole', 'bottle',
               'cleaver', 'pickaxe', 'scythe', 'bucksaw','trowel','hammer'] 
    
    for i in range(0, 20):
        tty = tooltypes[randint(0,len(tooltypes)-1)]
        tools.append(m.make(tty,pos=(uniform(-0.7, 0.7),uniform(-0.5,0.5),0.6),parent=stall.id,orientation=directions[randint(0,7)]))
    
    m.own(tmerchant,tools)

    home2_pos=(80,80,settlement_height)
    merchant=m.make('merchant', name='Dyfed Searae',desc='the pig merchant',
                    pos=pig_sty_pos,age=probability.fertility_age,
                    orientation=Quaternion(Vector3D([1,0,0]),Vector3D([0,-1,0])).as_list())
    sty=m.make('sty',pos=pig_sty_pos)
    m.know(merchant,mknowledge)
    m.know(merchant,area)
    m.know(merchant,mprices)
    m.own(merchant,sty)
    m.learn(merchant,(il.keep,"keep_livestock('pig', 'sty', 'sowee')"))
    m.learn(merchant,(il.sell,"sell_trade('pig', 'market', 'morning')"))
    m.learn(merchant,(il.sell,"sell_trade('pig', 'market', 'afternoon')"))
    m.learn(merchant,(il.lunch,"meal('ham','midday', 'inn')"))
    m.learn(merchant,(il.sup,"meal('beer', 'evening', 'inn')"))
    m.learn(merchant,(il.welcome,"welcome('Welcome to this our settlement','settler')"))
    m.learn(merchant,(il.help,"add_help(['Thankyou for joining our remote settlement.','Our first task is to build some shelter, but while we are doing that we still need food.','You can help us out by raising pigs for slaughter.','If you want to buy a piglet to raise, let me know by saying you would like to buy one.','Pigs love to eat acorns from under the oak trees that are abundant in this area.'],['I would like to buy a pig', 'Pehaps I will buy one later'])"))
    piglets=[]
    for i in range(0, 6):
        piglets.append(m.make('pig',pos=(uniform(0,4),uniform(0,4),settlement_height),parent=sty.id,orientation=directions[randint(0,7)]))
    m.learn(piglets,pig_goals)
    m.own(merchant,piglets)

    marshall=m.make('marshall', name='Gorun Iksa',desc='the duke\'s marshall',
                    pos=(14,12,settlement_height))
    m.know(marshall, [('deed','price','50')])
    m.know(marshall, area)
    m.know(marshall, about)
    m.learn(marshall,(il.help,"add_help(['On behalf of the Duke I would like to welcome you to moraf.','If you are new here I suggest talking to the pig seller.','He will tell you what you can do to help out.','If you have decide you would like to settle here I can assign you some land','but you will need to show that you are a useful citizen.','If you can raise 50 coins herding pigs, then a plot of land is yours.'],['I would like to buy a deed','I will come back when I have raised some pigs'])"))
    plots=[]
    for i in range(20, 200, 20):
        for j in range(-100, 100, 20):
            plots.append(m.make('deed',pos=(0,0,0),parent=marshall.id,plot=(i,j)))
    m.own(marshall,plots)

    # Warriors - the more adventurous types

    warriors=[]
    warrior=m.make('mercenary', name='Vonaa Barile',pos=(uniform(-2,2),uniform(-2,2),settlement_height),orientation=directions[randint(0,7)])
    bow=m.make('bow',pos=(0,0,0), parent=warrior.id)
    m.own(warrior,bow)
    for i in range(0, 6):
        arrow=m.make('arrow',pos=(0,0,0), parent=warrior.id)
        m.own(warrior,arrow)
    warriors.append(warrior)

    warrior=m.make('mercenary', name='Lile Birloc',pos=(uniform(-2,2),uniform(-2,2),settlement_height),orientation=directions[randint(0,7)])
    bow=m.make('bow',pos=(0,0,0), parent=warrior.id)
    m.own(warrior,bow)
    for i in range(0, 6):
        arrow=m.make('arrow',pos=(0,0,0), parent=warrior.id)
        m.own(warrior,arrow)
    warriors.append(warrior)

    # Warriors all know where stuff is in the village
    m.know(warriors,area)

    # Warriors enjoy their food and drink
    m.know(warriors, [('services','price','5')])
    m.learn(warriors,(il.help,"add_help(['The forest is a dangerous place.','If you need some help protecting your pigs,','I can help you out for a day or so.','I will need some gold for food and equipment.','For 5 coins I can work for you until sundown.','After sundown you should make sure your pigs are safe,','and get indoors yourself.','If you want to hire my services,','let me know by saying you would like to hire me.'],['I would like to hire your services','I can take care of my pigs alone'])"))
    m.learn(warriors,(il.hire,"hire_trade()"))
    m.learn(warriors,(il.forage,"gather(['boots', 'cloak', 'shirt', 'trousers', 'hat', 'skull', 'coin'])"))
    m.learn(warriors,(il.lunch,"meal('ham','midday', 'inn')"))
    m.learn(warriors,(il.sup,"meal('beer', 'evening', 'inn')"))

    # Goblins

    cfire=m.make('campfire',pos=(100,-50,settlement_height))
    m.make('fire',pos=(0,0,0),parent=cfire.id)

    cfire=m.make('campfire',pos=(90,-50,settlement_height))
    m.make('fire',pos=(0,0,0),parent=cfire.id)

    for i in range(10, 350, 5):
        direction=Vector3D(sin(radians(i)) * uniform(0,2), cos(radians(i)) * uniform(0,2), 10).unit_vector()
        orient=Quaternion(Vector3D(0,0,1), direction)
        m.make('stake',pos=(100 + 14 * sin(radians(i)), -50 + 16 * cos(radians(i)), -1), bbox=[-0.5,-0.5,0,0.5,0.5,5 + uniform(0,2)], orientation=orient.as_list())
        
    camp_area_points=[]
    for i in range(10, 350, 17):
        camp_area_points.append([14 * sin(radians(i)), 16 * cos(radians(i))])
    camp_area={'points': camp_area_points, 'layer':7 }
    m.make('path', name='camp_area', pos=camp_pos, area=camp_area, bbox=[-14, -16, 0, 14, 16, 1])

    goblin_guards=[]
    goblin=m.make('goblin', pos=(102, -33, settlement_height))
    goblin_guards.append(goblin)
    goblin=m.make('goblin', pos=(98, -33, settlement_height))
    goblin_guards.append(goblin)

    m.learn(goblin_guards,(il.defend,"defend('settler', 10)"))

    deers=[]
    xbase = uniform(-180,180)
    ybase = uniform(-180,180)
    for i in range(0, 10):
        xpos = xbase + uniform(-20,20)
        ypos = ybase + uniform(-20,20)
        d=m.make('deer', pos=(xpos, ypos, settlement_height))
        deers.append(d)
    m.learn(deers,deer_goals)
    
    chickens=[]
    xbase = uniform(140,160)
    ybase = uniform(130,150)
    for i in range(0, 10):
        xpos = xbase + uniform(-5,5)
        ypos = ybase + uniform(-5,5)
        d=m.make('chicken', pos=(xpos, ypos, settlement_height))
        chickens.append(d)
    m.learn(chickens,chicken_goals)

    fish=[]
    xbase = lake_pos[0]
    ybase = lake_pos[1]
    for i in range(0, 10):
        xpos = xbase + uniform(-5,5)
        ypos = ybase + uniform(-5,5)
        zpos = uniform(-4,0)
        d=m.make('fish', pos=(xpos, ypos, zpos))
        fish.append(d)
    

    # I am not sure if we need a guard
    #m.learn(guard,(il.patrol,"patrol(['m1', 'm2', 'm3', 'm4', 'm5', 'm6'])"))
    #m.tell_importance(guard,il.defend,'>',il.patrol)

##TODO - plantfield function - by MaxRandor##
# I am not sure about the flexibility of the code in terms of the field not being on a plane, dealing with bumpy ground, but I am not sure the creation of the field can deal with that either.
# Z values for the corners of the field are a bit iffy.
##
def plantfield(m,plant,field,field_points,furrowdist=1,plantspacing=1):
    '''Plant a field.
    
    first argument is for the object to plant the field with string
    second argument is the variable containing the identity of the field that is to be created (field = m.make(....))
    third argument is the space between the furrows
    fourth argument is the space between the plants along the furrows.
    4-3 this diagram shows which corners are which. 1 is the origin corner defiend in pos
    | |
    1-2
    '''
    # corner 1 is no longer needed as parent = field.id makes it obselete.
    # corner 2 obtained from the data in points in the m.make function
    corner2 = [field_points[3][0],field_points[3][1],0]
    # as above but different point
    corner3 = [field_points[2][0],field_points[2][1],0]
    # corner 4 is corner 3 -corner 2
    corner4 = [corner3[0]-corner2[0],corner3[1]-corner2[1],corner3[2]-corner2[2]]
    # the distance along the front of the field pythagoras in three dimensions
    frontfieldist = sqrt(corner2[0]**2 + corner2[1]**2 + corner2[2]**2)
    # the number of furrows is the distance that there is to cover / the distance between each furrow
    numfurrow = int(frontfieldist / furrowdist)
    # the increase in x,y,z of the start position of each furrow
    furrowincr = [corner2[0]/numfurrow,corner2[1]/numfurrow,corner2[2]/numfurrow]
    # the distance along the side of the field
    sidefieldist = sqrt(corner4[0]**2 + corner4[1]**2 + corner4[2]**2)
    # the number of plants in each furrow
    numplants = int(sidefieldist / plantspacing)
    # the increase in x,y,z of the position of the plants along the lenght of the furrow
    plantincr = [corner4[0]/numplants, corner4[1]/numplants, corner4[2]/numplants]
    for i in range(numfurrow):
        # start a new furrow
        furrowstart = [furrowincr[0]*i, furrowincr[1]*i, furrowincr[2]*i]
        for j in range(numplants):
            # calculate the plant position by adding the right number of increments to the furrowstart
            plantpos = [furrowstart[0] + plantincr[0]*j, furrowstart[1] + plantincr[1]*j, furrowstart[2] + plantincr[2]*j]
            # make the plant (note that the plant is randomly facing any direction)
            m.make(plant,pos=plantpos,orientation=directions[randint(0,7)], parent = field.id)

def add_pigs(mapeditor):
#   general things

    m=editor(mapeditor)

    sty = m.look_for(type='sty')
    merchant = m.look_for(name='Dyfed Searae')

    piglets=[]
    for i in range(0, 6):
        piglets.append(m.make('pig',pos=(uniform(0,4),uniform(0,4),settlement_height),parent=sty.id,orientation=directions[randint(0,7)]))
    m.learn(piglets,pig_goals)
    m.own(merchant,piglets)

def add_memtest(mapeditor):
#   general things

    m=editor(mapeditor)

    m.make('settler',pos=(0,5,5))
    m.make('oak',pos=(5,0,5))

def add_village(mapeditor):
#   general things

    m=editor(mapeditor)

    m.make('tower',pos=(210,210,5))
    m.make('gallows',pos=(185,175,5))

    m.make('house3',pos=(158,150,22),orientation=directions[1])
    m.make('house3',pos=(158,158,22),orientation=directions[4])
    m.make('house3',pos=(150,158,22),orientation=directions[0])
    m.make('house3',pos=(142,158,22),orientation=directions[7])
    m.make('house3',pos=(142,150,22),orientation=directions[3])
    m.make('house3',pos=(142,142,22),orientation=directions[6])
    m.make('house3',pos=(150,142,22),orientation=directions[2])
    m.make('house3',pos=(158,142,22),orientation=directions[5])

    m.make('ploughed_field', name='field',pos=(120,170,30),status=1.0,area={'points' : [ [0,0], [0,20], [20,20], [20,0] ], 'layer' : 8})
    m.make('ploughed_field', name='field',pos=(142,170,30),status=1.0,area={'points' : [ [0,0], [0,20], [20,20], [20,0] ], 'layer' : 8})

def test_skeleton(mapeditor):
#   general things

    m=editor(mapeditor)

    skeleton = m.make('skeleton', pos=(-38,-25,settlement_height))
    m.learn(skeleton,skeleton_goals)

def test_pig(mapeditor):
#   general things

    m=editor(mapeditor)
    pig = m.make('pig', pos=(3,3,settlement_height))
    m.learn(pig,pig_goals)
    m.make('acorn', pos=(4,4,settlement_height))

def test_browse(mapeditor):
#   test if browsing works
    
    m=editor(mapeditor)
    deer = m.make('deer', pos=(5, 0, settlement_height))
    m.learn(deer, (il.browse,"browse('fir', 0.8)"))
    m.make('fir',pos=(-10,-0,settlement_height))
    m.make('fir',pos=(-0,-10,settlement_height))
    m.make('fir',pos=(0,10,settlement_height))
    m.make('fir',pos=(10,0,settlement_height))
    
def test_forest(mapeditor):
#   test if browsing works
    
    m=editor(mapeditor)
    for i in forests:
        for j in range(0, i[1]):
            m.make(i[0],pos=(uniform(i[2],i[3]),uniform(i[4],i[5]),i[6]), orientation=directions[randint(0,7)])

   
def modify_terrain(mapeditor):
#   general things

    m=editor(mapeditor)

    world=m.look()
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
        
    m.set(world.id, terrain={'points' : points}, name="moraf", bbox=[minx * 64, miny * 64, minz, maxx * 64, maxy * 64, maxz])

def test_coll(mapeditor):

    m=editor(mapeditor)

    sty=m.make('sty',pos=pig_sty_pos,status=1.0,bbox=[5,5,3], orientation=directions[0])

def test_butcher(mapeditor):

    m=editor(mapeditor)

    butcher=m.make('merchant', name='Ulad Bargan',desc='the butcher',
                 pos=(3,3,0),age=probability.fertility_age)
    cleaver=m.make('cleaver', place='market', pos=(3,2.5,settlement_height))
    m.own(butcher,cleaver)
    m.learn(butcher,(il.trade,"trade('pig', 'cleaver', 'cut', 'ham', 'market')"))
    piglet = m.make('pig', pos=(3,2,0))

def test_pig(mapeditor):

    m=editor(mapeditor)

    piglet = m.make('pig', pos=(-3,-1,settlement_height))
    m.learn(piglet,pig_goals)

def test_settler(mapeditor):

    m=editor(mapeditor)
    settler=m.make('settler',pos=(1,1,0))
    axe=m.make('axe',pos=(0,0,0),parent=settler.id)
    m.own(settler,axe)
    m.know(settler,[('forest','location',(30,30,0))])
    m.learn(settler,(il.trade,"harvest_resource('lumber','oak','forest','axe')"))
    m.make('oak',pos=(32,32,0))

def test_forester(mapeditor):

    m=editor(mapeditor)
    # An NPC forester
    settler=m.make('settler', name='forester',pos=(0, 12, 0))
    trowel=m.make('trowel',pos=(0,0,0),parent=settler.id)
    m.own(settler, trowel)
    m.know(settler,[('forest','location',(30,30,0))])
    m.learn(settler,(il.trade,"plant_seeds('acorn','oak','forest','trowel')"))
    m.make('oak',pos=(32,32,0))

def test_warrior(mapeditor):

    m=editor(mapeditor)
    # An NPC warrior
    warrior=m.make('mercenary',pos=(0, 12, 0))
    m.learn(warrior,(il.trade,"gather(['loaf', 'axe'])"))

def test_chicken(mapeditor):

    m=editor(mapeditor)

    chuck = m.make('chicken', pos=(-3,-1,settlement_height))
    m.learn(chuck,chicken_goals)

def test_path(mapeditor):

    m=editor(mapeditor)
    path_area={'points' : [ [2,0], [22, 40], [132,122], [140,127], [144.5, 146.5], [169, 153], [169,155], [142.5,148.5], [138,129], [130,124], [18,40], [-2,-1] ], 'layer' : 7}
    m.make('path', name='path to village',pos=(10, 20,settlement_height), area=path_area,bbox=[169,154,1])
   
    
lych2_knowledge=[('w1','location',(-140,110,graveyard_height)),
                ('w2','location',(-50,160,graveyard_height)),
                ('w3','location',(-80,40,graveyard_height)),
                ('w4','location',(-180,150,graveyard_height))]

lych2_goals=[(il.assemble, "assemble(self, 'skeleton', ['skull', 'ribcage', 'arm', 'pelvis', 'thigh', 'shin'])"),
            (il.patrol,"patrol(['w1', 'w2', 'w3', 'w4'])")]
   
def test_graveyard(mapeditor):
    m = editor(mapeditor)
    
    m.make('mausoleum', name='mausoleum of harrington', pos=mausoleum_pos, orientation=directions[0])
    m.make('wall', name='steps',pos=(-160,100,graveyard_height),bbox=[4,4,2])
    
    graveyard_area={'points': [[-10, -8], [15, -11], [13,23], [-8, 8]], 'layer':7 }
    m.make('path', name='graveyard', pos=(-150, 110, graveyard_height), area=graveyard_area, bbox=[-10, -11,0, 15, 23, 1])
    
    path_area={'points' : [ [2, 0], [20, 20], [45, 60], [65, 70], [83, 40], [106, -15], [105, -12], [82, 36], [66, 67], [44, 57], [18, 16], [0, -4]], 'layer' : 7}
    m.make('path', name='path to graveyard',pos=(-150, 110, graveyard_height), area=path_area,bbox=[100,8,1])
    gravestone_styles = ['simple', 'rounded', 'stylish']
        

    m.make('gravestone', name='gravestone1', pos=(-145, 107, graveyard_height), orientation=directions[1], style = gravestone_styles[randint(0,2)])
    m.make('gravestone', name='gravestone2', pos=(-147, 104, graveyard_height), orientation=directions[2], style = gravestone_styles[randint(0,2)])
    m.make('gravestone', name='gravestone3', pos=(-148, 109, graveyard_height), orientation=directions[3], style = gravestone_styles[randint(0,2)])
    m.make('gravestone', name='gravestone4', pos=(-150, 117, graveyard_height), orientation=directions[2], style = gravestone_styles[randint(0,2)])
    m.make('gravestone', name='gravestone5', pos=(-143, 111, graveyard_height), orientation=directions[4], style = gravestone_styles[randint(0,2)])
    m.make('gravestone', name='gravestone6', pos=(-149, 110, graveyard_height), orientation=directions[3], style = gravestone_styles[randint(0,2)])
    m.make('gravestone', name='gravestone7', pos=(-147, 116, graveyard_height), orientation=directions[2], style = gravestone_styles[randint(0,2)])
    
    lych=m.make('lych', pos=(-140, 130, graveyard_height))
    m.learn(lych,lych2_goals)
    m.know(lych,lych2_knowledge)
    m.tell_importance(lych,il.assemble,'>',il.patrol)

def test_fire(mapeditor):
    m = editor(mapeditor)

    cfire=m.make('campfire',pos=(3,9,settlement_height))
    # m.make('fire',pos=(0,0,0),parent=cfire.id)

def test_own(mapeditor):

    m=editor(mapeditor)
    settler=m.make('settler',pos=(1,1,0))
    axe=m.make('axe',pos=(0,0,0),parent=settler.id)
    m.own(settler,axe)


def test_goblins(mapeditor):

    m=editor(mapeditor)
    goblin_guards=[]
    goblin=m.make('goblin', pos=(102, -33, settlement_height))
    goblin_guards.append(goblin)
    goblin=m.make('goblin', pos=(98, -33, settlement_height))
    goblin_guards.append(goblin)

    m.learn(goblin_guards,(il.defend,"defend('settler', 10)"))

def test_deer(mapeditor):

    m=editor(mapeditor)

    d=m.make('deer', pos=(5, 0, settlement_height))
    m.learn(d,deer_goals)
    # m.make('settler', pos=(0, 0, settlement_height))

def add_castle(mapeditor):
    
    m=editor(mapeditor)

    # create the peninsula for the castle
    world = m.look()
    points = { }
    points['-5x-2'] = [-5, -2, 20]
    points['-5x-1'] = [-5, -1, 20]
    points['-5x0'] = [-5, 0,   20]
    points['-5x1'] = [-5, 1,   20]
    points['-6x-2'] = [-6, -2, 20]
    points['-6x-1'] = [-6, -1, 20]
    points['-6x0'] = [-6, 0,   20]
    points['-6x1'] = [-6, 1,   20]
    points['-7x-2'] = [-7, -2, 20]
    points['-7x-1'] = [-7, -1, 20]
    points['-7x0'] = [-7, 0,   20]
    points['-7x1'] = [-7, 1,   20]
    points['-8x-2'] = [-8, -2, 20]
    points['-8x-1'] = [-8, -1, 20]
    points['-8x0'] = [-8, -0,  20]
    points['-8x1'] = [-8,  1,  20]

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

    # put the keep on the peninsula
    
    # the foundation looks ugly in sear and does not work in ember, so I left it out. coordinates are probably wrong
    #m.make('castle_foundation', pos=(-415, -70, settlement_height))

    # castle wall
    m.make('castle_outer_wall', pos=(-427.639, -30.2512, settlement_height), orientation=[0,0,0.7,0.7])

    # gateway 
    m.make('gateway', pos=(-380.753, -31.7661, settlement_height), orientation=[0,0,0.7,0.7])

    # the keep
    m.make('keep', pos=(-459.231, -79.5895, settlement_height), orientation=[0,0,0,-1])

    # top right tower
    m.make('bailey1', pos=(-477.46, 41.1592, settlement_height), orientation=[0,0,-1,0.02])

    # bottom left tower
    m.make('bailey2', pos=(-387.035, -98.432, settlement_height), orientation=[0,0,-0.7,-0.7])

    # bottom right tower
    m.make('don_jon', pos=(-382.011, 43.1961, settlement_height), orientation=[0,0,0,-1])

    # armory
    m.make('armory', pos=(-426.873, 45.0104, settlement_height), orientation=[0,0,-0.7,-0.7])
    
    # castle_house 1
    m.make('castle_house', name='castle_house1', pos=(-405.272, -80.1143, settlement_height), orientation=[0,0,0,1])

    # castle_house 2
    m.make('castle_house', name='castle_house2', pos=(-425.233, -79.6156, settlement_height), orientation=[0,0,0,1])
    
    # castle_house 3
    m.make('castle_house', name='castle_house3', pos=(-454.223, -50.518, settlement_height), orientation=[0,0,0.5,-0.868])

    # castle_house 4
    m.make('castle_house', name='castle_house4', pos=(-456.11, -26.3539, settlement_height), orientation=[0,0,0.7,-0.7])

    # inn 
    m.make('inn', pos=(-454.429, 12.7224, settlement_height), orientation=[0,0,0.28,1])

    castle_path_area = {'points': [[-400, -34], [-372, -34], [-350, -42], [-300, -56], [-215, -42], [-180, -5], [-125, -6], [-86, -4],
        [-86, -2], [-125, -4], [-180, -3], [-215, -40], [-300, -54], [-350, -40], [-372, -30], [-400, -30]], 'layer' : 7}
    m.make('path', name='path to castle',pos=(0, 0, settlement_height), area=castle_path_area,bbox=[100,8,1])
 

def kill_world(mapeditor):

    m=editor(mapeditor)

    world=m.look()
    m.set(world.id, status=-1)

def kill_tree(mapeditor):

    m=editor(mapeditor)

    tree=m.make('oak')
    m.set(tree.id, status=-1)

def test_oak(mapeditor):

    m=editor(mapeditor)

    tree=m.make('oak')

def test_attachment(mapeditor):

    m=editor(mapeditor)

    m.make('axe', attachment=23)

def make_shirt(mapeditor):

    m=editor(mapeditor)

    m.make('shirt', worn='chest', attachment=23, mass=2)
    m.make('acorn', worn='chest', pos=(1,1,1), attachment=23, mass=2)

def test_decays(mapeditor):

    m=editor(mapeditor)

    m.make('axe', decays='oak')

def test_device(mapeditor):

    m=editor(mapeditor)
    settler=m.make('settler',pos=(1,1,0))
    axe=m.make('door',pos=(0,0,0))
    m.learn(settler,(il.trade,"activate_device('door')"))

    world=m.look()

def test_tmods(mapeditor):
#   general things
    m=editor(mapeditor)
    
    world=m.look()
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
   
    terrainMod1 = {'type' : 'levelmod', 'pos' : [40,40,15], 'shape' :
        {'type' : 'ball', 'dim' : 2, 'radius' : 10 },
        'height' : 40
    }
    
    terrainMod2 = {'type' : 'cratermod', 'pos' : [0,0,0], 'shape' :
        {'type' : 'ball', 'dim' : 3, 'radius' : 10 },
    }
    
    myterrain = {'points' : points}
    m.set(world.id, terrain=myterrain, name="terrainMod_testLand", bbox=[minx * 64, miny * 64, minz, maxx * 64, maxy * 64, maxz])
    
    m.make('tower',pos=(60,60,5),terrainmod=terrainMod1)
    m.make('tower',pos=(20,20,5), terrainmod=terrainMod2)

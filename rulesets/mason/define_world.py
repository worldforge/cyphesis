#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 1999 Aloril (See the file COPYING for details).

from atlas import *
from whrandom import *
from mind.panlingua import interlinguish
il=interlinguish
from world import probability
from editor import editor
from Quaternion import Quaternion
from Vector3D import Vector3D
import time

#goal priority
#1) eating: certain times
#2) market/tavern: certain times, certain probability
#3) sleeping: nights
#4) chop trees: winter (when not doing anything else)
#4) other similar tasks: seasonal (-"-)

#'Breakfast' goal is type of 'eating'.

# Heights are all 0 for now, as uclient doesn't differentiate
# Once clients and servers can handle terrain properlly, then we
# can start thinking in more ernest about heights
settlement_height=0
forest_height=0

hall_xyz=(5,3,settlement_height)
forest_xyz=(-20,-60,settlement_height)

pig_sty_xyz=(8,8,settlement_height)
butcher_stall_xyz=(-41.5,-6.3,settlement_height)

lake_xyz=(-40,-70,0)

knowledge=[('axe','place','smithy'),
           ('forest','location',forest_xyz),
           ('hall','location',hall_xyz)]
mprices=[('pig','price','5')]
bprices=[('ham','price','2')]
bknowledge=[('market','location',butcher_stall_xyz)]
mknowledge=[('market','location',pig_sty_xyz)]
sknowledge=[('forest','location',forest_xyz),
            ('stash','location',(-98,-97,settlement_height))]
village=[('hall','location', hall_xyz),
         ('butcher','location', butcher_stall_xyz),
         ('pig','location', pig_sty_xyz)]
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

pig_goals=[(il.avoid,"avoid(['wolf','skeleton','crab'],10.0)"),
           (il.forage,"forage(self, 'acorn')"),
           (il.forage,"forage(self, 'apple')"),
           (il.forage,"forage(self, 'mushroom')"),
           (il.herd,"herd()")]

deer_goals=[(il.avoid,"avoid(['settler','orc'],10.0)"),
            (il.forage,"forage(self, 'apple')"),
            (il.forage,"forage(self, 'mushroom')"),
            (il.browse,"browse(self, 'fir', 0.8)"),
            (il.flock,"flock()")]

chicken_goals=[(il.avoid,"avoid(['settler','orc','wolf'],10.0)"),
               (il.flock,"flock()")]

squirrel_goals=[(il.avoid,"avoid(['wolf','crab'],10.0)"),
                (il.forage,"forage(self, 'acorn')"),
                (il.forage,"forage(self, 'pinekernel')")]

wolf_goals=[(il.forage,"forage(self, 'ham')"),
            (il.hunt,"predate(self,'pig',30.0)"),
            (il.hunt,"predate(self,'crab',20.0)"),
            (il.hunt,"predate(self,'squirrel',10.0)"),
            (il.patrol,"patrol(['w1', 'w2', 'w3', 'w4'])")]

crab_goals=[(il.avoid,"avoid('wolf',10.0)"),
            (il.hunt,"predate_small(self,'pig',30.0,10.0)")]

lych_goals=[(il.assemble, "assemble(self, 'skeleton', ['skull', 'ribcage', 'arm', 'pelvis', 'thigh', 'shin'])"),
            (il.patrol,"patrol(['w1', 'w2', 'w3', 'w4'])")]


# N, E, S, W, NE, SE, SW, NW in order
directions = [[0,0,0.707,0.707],[0,0,0,1],[0,0,-0.707,0.707],[0,0,1,0],
              [0,0,0.387,0.921],[0,0,-0.387,0.921],[0,0,-0.921,0.387],[0,0,0.921,0.387]]

forests = [
           ('oak', 30, 20, 190, -200, 100, 20),
           ('oak', 20, -200, -100, 0, 190, 20),
           ('fir', 200,  200,  300, -300, 300, 50),
           ('fir', 200, -300, 300,  200,  300, 50)
          ]

#observer calls this
def default(mapeditor):
#   general things

    m=editor(mapeditor)

    world=m.look()

    points = { }
    for i in range(-6, 7):
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

    m.set(world.id, terrain={'points' : points}, name="moraf")

# a wall around the world

    m.make('boundary',type='boundary',xyz=(-321,-321,-20),bbox=[2,642,300],mode="fixed")
    m.make('boundary',type='boundary',xyz=(-321,-321,-20),bbox=[642,2,300],mode="fixed")
    m.make('boundary',type='boundary',xyz=(-321, 320,-20),bbox=[642,2,300],mode="fixed")
    m.make('boundary',type='boundary',xyz=( 320,-321,-20),bbox=[2,642,300],mode="fixed")

    m.make('fir',type='fir',xyz=(-10,-0,settlement_height))
    m.make('fir',type='fir',xyz=(-0,-10,settlement_height))
    m.make('fir',type='fir',xyz=(0,10,settlement_height))
    m.make('fir',type='fir',xyz=(10,0,settlement_height))

    path_area={'points' : [ [-26,-62], [-36,-31], [-26,-14], [2,-1], [22, 40], [132,122], [140,127], [144.5, 146.5], [169, 153], [169,155], [142.5,148.5], [138,129], [130,124], [18,40], [-2, 0], [-28,-12], [-38,-29], [-29,-62] ], 'layer' : 7}
    m.make('path to village',type='path',xyz=(10, 20,settlement_height), area=path_area,bbox=[-38,-62,0,169,154,1])

    m.make('tower',type='tower',xyz=(210,210,5))
    m.make('gallows',type='gallows',xyz=(185,175,5))

    m.make('house3',type='house3',xyz=(158,150,22),orientation=directions[1])
    m.make('house3',type='house3',xyz=(158,158,22),orientation=directions[4])
    m.make('house3',type='house3',xyz=(150,158,22),orientation=directions[0])
    m.make('house3',type='house3',xyz=(142,158,22),orientation=directions[7])
    m.make('house3',type='house3',xyz=(142,150,22),orientation=directions[3])
    m.make('house3',type='house3',xyz=(142,142,22),orientation=directions[6])
    m.make('house3',type='house3',xyz=(150,142,22),orientation=directions[2])
    m.make('house3',type='house3',xyz=(158,142,22),orientation=directions[5])

    m.make('field',type='ploughed_field',xyz=(120,170,30),status=1.0,area={'points' : [ [0,0], [0,20], [20,20], [20,0] ], 'layer' : 8})
    m.make('field',type='ploughed_field',xyz=(142,170,30),status=1.0,area={'points' : [ [0,0], [0,20], [20,20], [20,0] ], 'layer' : 8})

    chickens=[]
    xbase = uniform(12,20)
    ybase = uniform(12,20)
    for i in range(0, 10):
        xpos = xbase + uniform(-5,5)
        ypos = ybase + uniform(-5,5)
        d=m.make('chicken', type='chicken', xyz=(xpos, ypos, settlement_height))
        chickens.append(d)
    m.learn(chickens,chicken_goals)

    fish=[]
    xbase = lake_xyz[0]
    ybase = lake_xyz[1]
    for i in range(0, 10):
        xpos = xbase + uniform(-5,5)
        ypos = ybase + uniform(-5,5)
        zpos = uniform(-4,0)
        d=m.make('fish', type='fish', xyz=(xpos, ypos, zpos))
        fish.append(d)
    
    # m.make('sherwood',type='forest',xyz=(-50, 10,settlement_height),bbox=[40,40,40])

    m.make('jetty',type='jetty',xyz=(-22,-48,0))
    m.make('boat',type='boat',xyz=(-22,-56,0),mode="floating")

#def dontrunme():
# a camp near the origin

    #cfire=m.make('campfire',type='campfire',xyz=(0,4,settlement_height))
    #m.make('fire',type='fire',xyz=(0.7,0.7,0),parent=cfire.id)
    #m.make('tent',type='tent',xyz=(-1,8,settlement_height),bbox=[2.5,2.5,3])
    #m.make('lumber',type='lumber',xyz=(-1,3,settlement_height))
    #m.make('lumber',type='lumber',xyz=(-1,2.5,settlement_height))

    # hall=m.make('hall',type='hall',xyz=hall_xyz)

    # Fire in the centre of the hall
    # cfire=m.make('campfire',type='campfire',xyz=(6,6,settlement_height),
                                            # parent=hall.id)
    # m.make('fire',type='fire',xyz=(0.7,0.7,0),parent=cfire.id)

    cfire=m.make('campfire',type='campfire',xyz=(3,9,settlement_height))
    m.make('fire',type='fire',xyz=(0,0,0),parent=cfire.id)

    cfire=m.make('campfire',type='campfire',xyz=(11,1,settlement_height))
    m.make('fire',type='fire',xyz=(0,0,0),parent=cfire.id)

    for i in range(0, 20):
        m.make('lumber',type='lumber',xyz=(uniform(-200,0),uniform(-200,0),settlement_height))

    for i in forests:
        for j in range(0, i[1]):
            m.make(i[0],type=i[0],xyz=(uniform(i[2],i[3]),uniform(i[4],i[5]),i[6]), orientation=directions[randint(0,7)])

    m.make('weather',type='weather',desc='object that describes the weather',
           xyz=(0,1,0), rain=0.0)

#   bones all over the place
    for i in range(0, 10):
        xpos = uniform(-200,200)
        ypos = uniform(-200,200)
        m.make('skull', type='skull', xyz=(xpos+uniform(-2,2),ypos+uniform(-2,2),settlement_height))
        m.make('pelvis', type='pelvis', xyz=(xpos+uniform(-2,2),ypos+uniform(-2,2),settlement_height))
        m.make('arm', type='arm', xyz=(xpos+uniform(-2,2),ypos+uniform(-2,2),settlement_height))
        m.make('thigh', type='thigh', xyz=(xpos+uniform(-2,2),ypos+uniform(-2,2),settlement_height))
        m.make('shin', type='shin', xyz=(xpos+uniform(-2,2),ypos+uniform(-2,2),settlement_height))
        m.make('ribcage', type='ribcage', xyz=(xpos+uniform(-2,2),ypos+uniform(-2,2),settlement_height))

#   the lych, who makes bones into skeletons
    lych=m.make('lych', type='lych', xyz=(-21, -89, settlement_height))
    m.learn(lych,lych_goals)
    m.know(lych,lych_knowledge)
    m.tell_importance(lych,il.assemble,'>',il.patrol)

#   animals
    piglet = m.make('pig', type='pig', xyz=(-3,-1,settlement_height))
    m.learn(piglet,pig_goals)

    wolf = m.make('wolf', type='wolf', xyz=(90,-90,settlement_height))
    m.learn(wolf,wolf_goals)
    m.know(wolf,wolf_knowledge)
    m.tell_importance(wolf,il.forage,'>',il.hunt)
    m.tell_importance(wolf,il.forage,'>',il.patrol)
    m.tell_importance(wolf,il.hunt,'>',il.patrol)

    crab = m.make('crab', type='crab', xyz=(-90,90,settlement_height))
    m.learn(crab,crab_goals)

    skeleton = m.make('skeleton', type='skeleton', xyz=(-38,-25,settlement_height))

    squirrel = m.make('squirrel', type='squirrel', desc='test squirrel',
                    xyz=(-32,-15,settlement_height))
    m.know(squirrel,sknowledge)
    m.learn(squirrel,squirrel_goals)

#   villagers
    #directions = [[0,1,0],[1,0,0],[0,-1,0],[-1,0,0],
                  #[0.7,0.7,0],[0.7,-0.7,0],[-0.7,-0.7,0],[-0.7,0.7,0]]

    # An NPC settler
    settler=m.make('settler',xyz=(1,1,0), sex='male')
    axe=m.make('axe',type='axe',xyz=(0,0,0),parent=settler.id)
    m.own(settler,axe)
    m.know(settler,[('forest','location',(30,30,0))])
    m.learn(settler,(il.trade,"harvest_resource(self,'lumber','oak','forest','axe')"))

    home1_xyz=(90,-90,settlement_height)

    butcher=m.make('Ulad Bargan',type='butcher',desc='the butcher',
                 xyz=butcher_stall_xyz,age=probability.fertility_age,sex='male')
    m.learn(butcher,(il.trade,"trade(self, 'pig', 'cleaver', 'ham', 'market')"))
    m.learn(butcher,(il.buy_livestock,"buy_livestock('pig', 1)"))
    m.learn(butcher,(il.market,"run_shop('mstall_freshmeat_1_se','open','dawn')"))
    m.learn(butcher,(il.market,"run_shop('mstall_freshmeat_1_se','closed','evening')"))
    m.know(butcher,bknowledge)
    m.know(butcher,bprices)
    cleaver=m.make('cleaver', type='cleaver', desc='cleaver for cutting meat',
                   place='market', xyz=(0, 0, 0), parent=butcher.id)
    m.own(butcher,cleaver)
    m.learn(butcher,(il.sell,"sell_trade('ham', 'market')"))
    coins=[]
    for i in range(0, 60):
        coins.append(m.make('coin',type='coin',xyz=(0,0,0),parent=butcher.id))
    m.own(butcher,coins)
    

    home2_xyz=(80,80,settlement_height)
    merchant=m.make('Dyfed Searae',type='merchant',desc='the pig merchant',
                    xyz=pig_sty_xyz,age=probability.fertility_age,
                    sex='male',orientation=Quaternion(Vector3D([1,0,0]),Vector3D([0,-1,0])).as_list())
    sty=m.make('sty',type='sty',xyz=pig_sty_xyz)
    m.know(merchant,mknowledge)
    m.know(merchant,village)
    m.know(merchant,mprices)
    m.own(merchant,sty)
    m.learn(merchant,(il.keep,"keep_livestock('pig', 'sty', 'sowee')"))
    m.learn(merchant,(il.sell,"sell_trade('pig', 'market', 'morning')"))
    m.learn(merchant,(il.sell,"sell_trade('pig', 'market', 'afternoon')"))
    m.learn(merchant,(il.lunch,"meal(self, 'ham','midday', 'inn')"))
    m.learn(merchant,(il.sup,"meal(self, 'beer', 'evening', 'inn')"))
    m.learn(merchant,(il.welcome,"welcome('Welcome to this our settlement','settler')"))
    m.learn(merchant,(il.help,"add_help(['Thankyou for joining our remote settlement.','Our first task is to build some shelter, but while we are doing that we still need food.','You can help us out by raising pigs for slaughter.','If you want to buy a piglet to raise, let me know by saying you would like to buy one.','Pigs love to eat acorns from under the oak trees that are abundant in this area.'],['I would like to buy a pig', 'Pehaps I will buy one later'])"))
    piglets=[]
    for i in range(0, 6):
        piglets.append(m.make('pig',type='pig',xyz=(uniform(0,4),uniform(0,4),settlement_height),parent=sty.id,orientation=directions[randint(0,7)]))
    m.learn(piglets,pig_goals)
    m.own(merchant,piglets)

    marshall=m.make('Gorun Iksa',type='marshall',desc='the duke\'s marshall',
                    xyz=(14,12,settlement_height), sex='male')
    m.know(marshall, [('deed','price','50')])
    m.know(marshall, village)
    m.learn(marshall,(il.help,"add_help(['On behalf of the Duke I would like to welcome you to moraf.','If you are new here I suggest talking to the pig seller.','He will tell you what you can do to help out.','If you have decide you would like to settle here I can assign you some land','but you will need to show that you are a useful citizen.','If you can raise 50 coins herding pigs, then a plot of land is yours.'],['I would like to buy a deed','I will come back when I have raised some pigs'])"))
    plots=[]
    for i in range(20, 200, 20):
        for j in range(-100, 100, 20):
            plots.append(m.make('deed',xyz=(0,0,0),parent=marshall.id,plot=(i,j)))
    m.own(marshall,plots)

    stall=m.make('Market Stall',type='stall',xyz=(17,26,settlement_height))
    m.make('wall',type='wall',parent=stall.id,xyz=(0,0,0),bbox=(-0.5,-1.5,0,0.5,1.5,0.8))
    m.make('wall',type='wall',parent=stall.id,xyz=(2,0,0),bbox=(0,-1.5,0,1,1.5,2))

    # Warriors - the more adventurous types

    warriors=[]
    warrior=m.make('Vonaa Barile',type='mercenary',xyz=(uniform(-2,2),uniform(-2,2),settlement_height),sex='female',orientation=directions[randint(0,7)])
    bow=m.make('bow',type='bow',xyz=(0,0,0), parent=warrior.id)
    m.own(warrior,bow)
    for i in range(0, 6):
        arrow=m.make('arrow',type='arrow',xyz=(0,0,0), parent=warrior.id)
        m.own(warrior,arrow)
    warriors.append(warrior)

    warrior=m.make('Lile Birloc', type='mercenary',xyz=(uniform(-2,2),uniform(-2,2),settlement_height),sex='female',orientation=directions[randint(0,7)])
    bow=m.make('bow',type='bow',xyz=(0,0,0), parent=warrior.id)
    m.own(warrior,bow)
    for i in range(0, 6):
        arrow=m.make('arrow',type='arrow',xyz=(0,0,0), parent=warrior.id)
        m.own(warrior,arrow)
    warriors.append(warrior)

    # Warriors all know where stuff is in the village
    m.know(warriors,village)

    # Warriors enjoy their food and drink
    m.know(warriors, [('services','price','5')])
    m.learn(warriors,(il.help,"add_help(['The forest is a dangerous place.','If you need some help protecting your pigs,','I can help you out for a day or so.','I will need some gold for food and equipment.','For 5 coins I can work for you until sundown.','After sundown you should make sure your pigs are safe,','and get indoors yourself.','If you want to hire my services,','let me know by saying you would like to hire me.'],['I would like to hire your services','I can take care of my pigs alone'])"))
    m.learn(warriors,(il.hire,"hire_trade()"))
    m.learn(warriors,(il.lunch,"meal(self, 'ham','midday', 'inn')"))
    m.learn(warriors,(il.sup,"meal(self, 'beer', 'evening', 'inn')"))

    deers=[]
    xbase = uniform(-180,180)
    ybase = uniform(-180,180)
    for i in range(0, 10):
        xpos = xbase + uniform(-20,20)
        ypos = ybase + uniform(-20,20)
        d=m.make('deer', type='deer', xyz=(xpos, ypos, settlement_height))
        deers.append(d)
    m.learn(deers,deer_goals)
    

    # I am not sure if we need a guard
    #m.learn(guard,(il.patrol,"patrol(['m1', 'm2', 'm3', 'm4', 'm5', 'm6'])"))
    #m.tell_importance(guard,il.defend,'>',il.patrol)

def add_pigs(mapeditor):
#   general things

    m=editor(mapeditor)

    sty = m.look_for(type='sty')
    merchant = m.look_for(name='Dyfed Searae')

    piglets=[]
    for i in range(0, 6):
        piglets.append(m.make('pig',type='pig',xyz=(uniform(0,4),uniform(0,4),settlement_height),parent=sty.id,orientation=directions[randint(0,7)]))
    m.learn(piglets,pig_goals)
    m.own(merchant,piglets)

def add_memtest(mapeditor):
#   general things

    m=editor(mapeditor)

    m.make('settler',type='settler',xyz=(0,5,5))
    m.make('oak',type='oak',xyz=(5,0,5))

def add_village(mapeditor):
#   general things

    m=editor(mapeditor)

    m.make('tower',type='tower',xyz=(210,210,5))
    m.make('gallows',type='gallows',xyz=(185,175,5))

    m.make('house3',type='house3',xyz=(158,150,22),orientation=directions[1])
    m.make('house3',type='house3',xyz=(158,158,22),orientation=directions[4])
    m.make('house3',type='house3',xyz=(150,158,22),orientation=directions[0])
    m.make('house3',type='house3',xyz=(142,158,22),orientation=directions[7])
    m.make('house3',type='house3',xyz=(142,150,22),orientation=directions[3])
    m.make('house3',type='house3',xyz=(142,142,22),orientation=directions[6])
    m.make('house3',type='house3',xyz=(150,142,22),orientation=directions[2])
    m.make('house3',type='house3',xyz=(158,142,22),orientation=directions[5])

    m.make('field',type='ploughed_field',xyz=(120,170,30),status=1.0,area={'points' : [ [0,0], [0,20], [20,20], [20,0] ], 'layer' : 8})
    m.make('field',type='ploughed_field',xyz=(142,170,30),status=1.0,area={'points' : [ [0,0], [0,20], [20,20], [20,0] ], 'layer' : 8})
def test_pig(mapeditor):
#   general things

    m=editor(mapeditor)
    pig = m.make('pig', type='pig', xyz=(3,3,settlement_height))
    m.learn(pig,pig_goals)
    m.make('acorn', type='acorn', xyz=(4,4,settlement_height))

def test_browse(mapeditor):
#   test if browsing works
    
    m=editor(mapeditor)
    deer = m.make('deer', type='deer', xyz=(5, 0, settlement_height))
    m.learn(deer, (il.browse,"browse(self, 'fir', 0.8)"))
    m.make('fir',type='fir',xyz=(-10,-0,settlement_height))
    m.make('fir',type='fir',xyz=(-0,-10,settlement_height))
    m.make('fir',type='fir',xyz=(0,10,settlement_height))
    m.make('fir',type='fir',xyz=(10,0,settlement_height))
    
def test_forest(mapeditor):
#   test if browsing works
    
    m=editor(mapeditor)
    for i in forests:
        for j in range(0, i[1]):
            m.make(i[0],type=i[0],xyz=(uniform(i[2],i[3]),uniform(i[4],i[5]),i[6]), orientation=directions[randint(0,7)])

def modify_terrain(mapeditor):
#   general things

    m=editor(mapeditor)

    world=m.look()
    points = { }
    for i in range(-6, 7):
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

    m.set(world.id, terrain={'points' : points})

def test_coll(mapeditor):

    m=editor(mapeditor)

    sty=m.make('sty',type='sty',xyz=pig_sty_xyz,status=1.0,bbox=[5,5,3], orientation=directions[0])

def test_butcher(mapeditor):

    m=editor(mapeditor)

    butcher=m.make('Ulad Bargan',type='butcher',desc='the butcher',
                 xyz=(3,3,0),age=probability.fertility_age,sex='male')
    cleaver=m.make('cleaver', type='cleaver', desc='cleaver for cutting meat',
                   place='market', xyz=(3,2.5,settlement_height))
    m.own(butcher,cleaver)
    m.learn(butcher,(il.trade,"trade(self, 'pig', 'cleaver', 'cut', 'ham', 'market')"))
    piglet = m.make('pig', type='pig', xyz=(3,2,0))

def test_pig(mapeditor):

    m=editor(mapeditor)

    piglet = m.make('pig', type='pig', xyz=(-3,-1,settlement_height))
    m.learn(piglet,pig_goals)

def test_settler(mapeditor):

    m=editor(mapeditor)
    settler=m.make('settler',xyz=(1,1,0), sex='male')
    axe=m.make('axe',type='axe',xyz=(0,0,0),parent=settler.id)
    m.own(settler,axe)
    m.know(settler,[('forest','location',(30,30,0))])
    m.learn(settler,(il.trade,"harvest_resource(self,'lumber','oak','forest','axe')"))
    m.make('oak',xyz=(32,32,0))

def test_path(mapeditor):

    m=editor(mapeditor)
    path_area={'points' : [ [2,0], [22, 40], [132,122], [140,127], [144.5, 146.5], [169, 153], [169,155], [142.5,148.5], [138,129], [130,124], [18,40], [-2,-1] ], 'layer' : 7}
    m.make('path to village',type='path',xyz=(10, 20,settlement_height), area=path_area,bbox=[169,154,1])
   
    
graveyard_height = 0

lych2_knowledge=[('w1','location',(-150,110,graveyard_height)),
                ('w2','location',(-50,160,graveyard_height)),
                ('w3','location',(-80,40,graveyard_height)),
                ('w4','location',(-180,150,graveyard_height))]

   
def test_graveyard(mapeditor):
    m = editor(mapeditor)
    
    m.make('mausoluem of harrington', type='mausoleum', xyz=(-160, 105, graveyard_height), orientation=directions[0])
    graveyard_area={'points': [[-10, -8], [8, -7], [7,9], [-8, 8]], 'layer':7 }
    m.make('graveyard', xyz=(-150, 110, graveyard_height), type='path', area=graveyard_area, bbox=[20, 20, 1])
    
    path_area={'points' : [ [2, 0], [20, 20], [45, 60], [65, 70], [83, 40], [106, -15], [105, -12], [82, 36], [66, 67], [44, 57], [18, 16], [0, -4]], 'layer' : 7}
    m.make('path to graveyard',type='path',xyz=(-150, 110, graveyard_height), area=path_area,bbox=[100,8,1])

    m.make('gravestone1', type='gravestone_rounded', xyz=(-145, 107, graveyard_height), orientation=directions[1])
    m.make('gravestone2', type='gravestone_simple', xyz=(-147, 104, graveyard_height), orientation=directions[2])
    m.make('gravestone3', type='gravestone_stylish', xyz=(-148, 109, graveyard_height), orientation=directions[3])
    m.make('gravestone4', type='gravestone_rounded', xyz=(-150, 117, graveyard_height), orientation=directions[2])
    m.make('gravestone5', type='gravestone_rounded', xyz=(-143, 111, graveyard_height), orientation=directions[4])
    m.make('gravestone6', type='gravestone_stylish', xyz=(-149, 110, graveyard_height), orientation=directions[3])
    m.make('gravestone7', type='gravestone_stylish', xyz=(-147, 116, graveyard_height), orientation=directions[2])
    
    lych=m.make('lych', type='lych', xyz=(-140, 130, graveyard_height))
    m.learn(lych,lych_goals)
    m.know(lych,lych2_knowledge)
    m.tell_importance(lych,il.assemble,'>',il.patrol)

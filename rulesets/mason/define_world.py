#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 1999 Aloril (See the file COPYING for details).

from atlas import *
from whrandom import *
from mind.panlingua import interlinguish
il=interlinguish
from world import probability
from editor import editor
import time

#goal priority
#1) eating: certain times
#2) market/tavern: certain times, certain probability
#3) sleeping: nights
#4) chop trees: winter (when not doing anything else)
#4) other similar tasks: seasonal (-"-)

#'Breakfast' goal is type of 'eating'.

#village_height=0
#forest_height=0
#foo_xyz=(12,-20,village_height)


#knowledge=[('axe','smithy'),
#           ('smithy',smithy_xyz),
#           ('forest',forest_xyz),
#           ('tavern',tavern_xyz),
#           ('market',market_xyz)]
#mprices=[('pig','5')]
#m2prices=[('pig','4')]
#bprices=[('ham','2')]
#bknowledge=[('market',butcher_stall_xyz)]
#mknowledge=[('market',pig_stall_xyz)]
#sknowledge=[('forest',(-30,-116,village_height)),
#            ('stash',(-98,-97,village_height))]
#village=[('inn', inn_xyz),
#         ('butcher', butcher_stall_xyz),
#         ('pig', pig_stall_xyz)]
#gknowledge=[('m1',(-17, -1,    village_height)),
#            ('m2',(-29, -1,    village_height)),
#            ('m3',(-29, -7.5,  village_height)),
#            ('m4',(-38, -10,   village_height)),
#            ('m5',(-43, -15,   village_height)),
#            ('m6',(-43, -14.5, village_height))]
#
#wolf_knowledge=[('w1',(90,-90,village_height)),
#                ('w2',(110,-90,village_height)),
#                ('w3',(110,90,village_height)),
#                ('w4',(90,90,village_height))]
#
#lych_knowledge=[('w1',(0,-96,village_height)),
#                ('w2',(-70,-70,village_height)),
#                ('w3',(-100,70,village_height)),
#                ('w4',(-147,-90,village_height))]
#
#wander=(il.wander,"wander()")
#forage=(il.forage,"forage()")
#trade=(il.trade,"trade()")
#keep=(il.keep,"keep()")
#sell=(il.sell,"sell_trade()")
#patrol=(il.patrol,"patrol()")

#pig_goals=[(il.avoid,"avoid(['wolf','skeleton','crab'],10.0)"),
#           (il.forage,"forage(self, 'acorn')"),
#           (il.forage,"forage(self, 'apple')"),
#           (il.herd,"herd()")]
#
#wolf_goals=[(il.forage,"forage(self, 'ham')"),
#            (il.hunt,"predate(self,'pig',30.0)"),
#            (il.hunt,"predate(self,'crab',20.0)"),
#            (il.patrol,"patrol(['w1', 'w2', 'w3', 'w4'])")]
#
#crab_goals=[(il.avoid,"avoid('wolf',10.0)"),
#            (il.hunt,"predate_small(self,'pig',30.0,10.0)")]
#
#lych_goals=[(il.assemble, "assemble(self, 'skeleton', ['skull', 'ribcage', 'arm', 'pelvis', 'thigh', 'shin'])"),
#            (il.patrol,"patrol(['w1', 'w2', 'w3', 'w4'])")]

#observer calls this
def default(mapeditor):
#   general things

    m=editor(mapeditor)

# a wall around the world

    county=m.make('county',type='area',xyz=(-5000,-5000,0),bbox=[10000,10000,200])
    village=m.make('village',type='area',xyz=(4800,4800,0),bbox=[400,400,100], parent=county.id)
    market=m.make('square',type='area',xyz=(120,170,0),bbox=[40,30,10], parent=village.id)
    m.make('stall',type='stall', xyz=(2,2,0), bbox=[1,0.5,1], parent=market.id)
    m.make('stall',type='stall', xyz=(6,2,0), bbox=[1,0.5,1], parent=market.id)
    m.make('stall',type='stall', xyz=(10,2,0), bbox=[1,0.5,1], parent=market.id)
    m.make('stall',type='stall', xyz=(2,6,0), bbox=[1,0.5,1], parent=market.id)
    m.make('stall',type='stall', xyz=(6,6,0), bbox=[1,0.5,1], parent=market.id)
    m.make('stall',type='stall', xyz=(10,6,0), bbox=[1,0.5,1], parent=market.id)

def dont_run_me(foo):
    m.make('wall',type='wall',xyz=(-151,-101,village_height),bbox=[1,102,2.5])
    m.make('wall',type='wall',xyz=(-151,-101,village_height),bbox=[152,1,2.5])
    m.make('wall',type='wall',xyz=(-151,100,village_height),bbox=[152,1,2.5])
    m.make('wall',type='wall',xyz=(100,-101,village_height),bbox=[1,102,2.5])

# a camp near the origin

    #cfire=m.make('campfire',type='campfire',xyz=(0,4,village_height))
    #m.make('fire',type='fire',xyz=(0.7,0.7,0),parent=cfire.id)
    #m.make('tent',type='tent',xyz=(-1,8,village_height),bbox=[2.5,2.5,3])
    #m.make('lumber',type='lumber',xyz=(-1,3,village_height))
    #m.make('lumber',type='lumber',xyz=(-1,2.5,village_height))

    cfire=m.make('campfire',type='campfire',xyz=(35,54,village_height))
    m.make('fire',type='fire',xyz=(0.7,0.7,0),parent=cfire.id)

    cfire=m.make('campfire',type='campfire',xyz=(42,51,village_height))
    m.make('fire',type='fire',xyz=(0.7,0.7,0),parent=cfire.id)

    cfire=m.make('campfire',type='campfire',xyz=(43,39,village_height))
    m.make('fire',type='fire',xyz=(0.7,0.7,0),parent=cfire.id)

    m.make('sign_market_w',type='sign_market_w',xyz=(-8.7,-21.2,village_height))

    for i in range(0, 20):
        m.make('lumber',type='lumber',xyz=(uniform(-100,0),uniform(-100,-80),village_height))

#   general
    m.make('oak',type='oak',xyz=(-70,-86,village_height), bbox=[0.5,0.5,3])
    m.make('oak',type='oak',xyz=(-6,-77,village_height), bbox=[0.5,0.5,3])
    m.make('oak',type='oak',xyz=(-24,-90,village_height), bbox=[0.5,0.5,3])
    m.make('oak',type='oak',xyz=(-49,-90,village_height), bbox=[0.5,0.5,3])
    m.make('oak',type='oak',xyz=(-86,-81,village_height), bbox=[0.5,0.5,3])
    m.make('oak',type='oak',xyz=(-12,-98,village_height), bbox=[0.5,0.5,3])
    m.make('oak',type='oak',xyz=(-35,-73,village_height), bbox=[0.5,0.5,3])
    m.make('oak',type='oak',xyz=(-83,-66,village_height), bbox=[0.5,0.5,3])
    m.make('oak',type='oak',xyz=(-87,-34,village_height), bbox=[0.5,0.5,3])
    m.make('oak',type='oak',xyz=(-98,-28,village_height), bbox=[0.5,0.5,3])
    m.make('oak',type='oak',xyz=(-75,31,village_height), bbox=[0.5,0.5,3])

    m.make('weather',type='weather',desc='object that describes the weather',
           xyz=(0,1,0), rain=0.0)

#   bones all over the place
    for i in range(0, 10):
        xpos = uniform(-150,100)
        ypos = uniform(-100,100)
        m.make('skull', type='skull', xyz=(xpos+uniform(-2,2),ypos+uniform(-2,2),village_height))
        m.make('pelvis', type='pelvis', xyz=(xpos+uniform(-2,2),ypos+uniform(-2,2),village_height))
        m.make('arm', type='arm', xyz=(xpos+uniform(-2,2),ypos+uniform(-2,2),village_height))
        m.make('thigh', type='thigh', xyz=(xpos+uniform(-2,2),ypos+uniform(-2,2),village_height))
        m.make('shin', type='shin', xyz=(xpos+uniform(-2,2),ypos+uniform(-2,2),village_height))
        m.make('ribcage', type='ribcage', xyz=(xpos+uniform(-2,2),ypos+uniform(-2,2),village_height))

#   the lych, who makes bones into skeletons
    lych=m.make('lych', type='lych', xyz=(-21, -89, village_height))
    m.learn(lych,lych_goals)
    m.know(lych,lych_knowledge)
    m.tell_importance(lych,il.assemble,'>',il.patrol)

#   animals
    piglet = m.make('pig', type='pig', xyz=(-31,-16,village_height))
    m.learn(piglet,pig_goals)

    wolf = m.make('wolf', type='wolf', xyz=(90,-90,village_height))
    m.learn(wolf,wolf_goals)
    m.know(wolf,wolf_knowledge)
    m.tell_importance(wolf,il.forage,'>',il.hunt)
    m.tell_importance(wolf,il.forage,'>',il.patrol)
    m.tell_importance(wolf,il.hunt,'>',il.patrol)

    crab = m.make('crab', type='crab', xyz=(-90,90,village_height))
    m.learn(crab,crab_goals)

    skeleton = m.make('skeleton', type='skeleton', xyz=(-38,-25,village_height))

    #squirrel = m.make('squirrel', type='squirrel', desc='test squirrel',
                    #xyz=(-32,-115,village_height))
    #m.know(squirrel,sknowledge)
    #m.learn(squirrel,(il.transport,"transport_something(self,'acorn','forest','stash')"))

#   villagers

    # Some generic market traders, to operate the market stalls
    stall_list = ['mstall_bakery_2_se', 'mstall_beer_1_se',
                  'mstall_blacksmith_1_sw', 'mstall_blue_1_us',
                  'mstall_books_2_sw', 'mstall_cheese_2_se',
                  'mstall_fish_1_se', 'mstall_freshmeat_1_se',
                  'mstall_fruits_2_se', 'mstall_healer_1_sw',
                  'mstall_household_1_sw', 'mstall_jewels_1_sw',
                  'mstall_magic_items_1_sw', 'mstall_milk_2_se',
                  'mstall_seamstress_1_sw', 'mstall_vegetables_1_se',
                  'mstall_wine_1_se']
    directions = [[0,1,0],[1,0,0],[0,-1,0],[-1,0,0],
                  [0.7,0.7,0],[0.7,-0.7,0],[-0.7,-0.7,0],[-0.7,0.7,0]]
    trader_list = ['merchant', 'maid_brown', 'maid_blond', 'maid_red']
    for stall in stall_list:
        trader=m.make('trader',type=trader_list[randint(0,3)],
                 xyz=(uniform(-36,-56),uniform(-7,-17),village_height),
                 age=probability.fertility_age,face=directions[randint(0,7)])
        m.learn(trader,(il.market,"run_shop('"+stall+"','open','dawn')"))
        m.learn(trader,(il.market,"run_shop('"+stall+"','closed','evening')"))

    #m.make('bstall',type='bstall',xyz=(-41,-5,village_height))

    home1_xyz=(90,-90,village_height)

    butcher=m.make('Ulad Bargan',type='butcher',desc='the butcher',
                 xyz=butcher_stall_xyz,age=probability.fertility_age,sex='male')
    m.learn(butcher,(il.trade,"trade(self, 'pig', 'cleaver', 'cut', 'ham', 'market','day')"))
    m.learn(butcher,(il.buy_livestock,"buy_livestock('pig', 1)"))
    m.learn(butcher,(il.market,"run_shop('mstall_freshmeat_1_se','open','dawn')"))
    m.learn(butcher,(il.market,"run_shop('mstall_freshmeat_1_se','closed','evening')"))
    m.know(butcher,bknowledge)
    m.price(butcher,bprices)
    cleaver=m.make('cleaver', type='cleaver', desc='cleaver for cutting meat',
                   place='market', xyz=(-41,-5,village_height))
    m.own(butcher,cleaver)
    m.learn(butcher,(il.sell,"sell_trade('ham', 'market')"))
    coins=[]
    for i in range(0, 60):
        coins.append(m.make('coin',type='coin',xyz=(0,0,0),parent=butcher.id))
    m.own(butcher,coins)
    

    home2_xyz=(80,80,village_height)
    merchant=m.make('Dyfed Searae',type='merchant',desc='the pig merchant',
         xyz=pig_stall_xyz,age=probability.fertility_age,sex='male',face=[-1,0,0])
    merchant2=m.make('Dylan Searae',type='merchant',desc='the pig merchant',
         xyz=(-28,2,village_height),age=probability.fertility_age,sex='male',face=[0,-1,0])
    merchants=[merchant, merchant2]
    sty=m.make('sty',type='sty',xyz=pig_sty_xyz,status=1.0,bbox=[2.5,2.5,3])
    m.know(merchants,mknowledge)
    m.know(merchants,village)
    m.price(merchant,mprices)
    m.price(merchant2,m2prices)
    m.own(merchant,sty)
    m.learn(merchant,(il.keep,"keep('pig', 'sty')"))
    m.learn(merchant, (il.sell,"sell_trade('pig', 'market', 'morning')"))
    m.learn(merchant2,(il.sell,"sell_trade('pig', 'market', 'afternoon')"))
    m.learn(merchants,(il.lunch,"meal(self, 'ham','midday', 'inn')"))
    m.learn(merchants,(il.sup,"meal(self, 'beer', 'evening', 'inn')"))
    piglets=[]
    for i in range(0, 6):
        piglets.append(m.make('pig',type='pig',xyz=(uniform(0,4),uniform(0,4),village_height),parent=sty.id,face=directions[randint(0,7)]))
    m.learn(piglets,pig_goals)
    m.own(merchants,piglets)

    # Warriors - the more adventurous types

    warriors=[]
    warrior=m.make('Tom Harrowe', type='guard',xyz=(uniform(-1,14),uniform(-18,-27),village_height),sex='male',face=directions[randint(0,7)])
    sword=m.make('sword',type='sword',xyz=(0,0,0), parent=warrior.id)
    m.own(warrior,sword)
    warriors.append(warrior)

    warrior=m.make('Mae Dollor', type='guard',xyz=(uniform(-1,14),uniform(-18,-27),village_height),sex='female',face=directions[randint(0,7)])
    sword=m.make('sword',type='sword',xyz=(0,0,0), parent=warrior.id)
    m.own(warrior,sword)
    warriors.append(warrior)

    warrior=m.make('Covan Dubneal',type='guard',xyz=(uniform(-1,14),uniform(-18,-27),village_height),sex='male',face=directions[randint(0,7)])
    sword=m.make('sword',type='sword',xyz=(0,0,0), parent=warrior.id)
    m.own(warrior,sword)
    warriors.append(warrior)

    warrior=m.make('Roal Guddon', type='guard',xyz=(uniform(-1,14),uniform(-18,-27),village_height),sex='male',face=directions[randint(0,7)])
    sword=m.make('sword',type='sword',xyz=(0,0,0), parent=warrior.id)
    m.own(warrior,sword)
    warriors.append(warrior)

    m.learn(warriors,(il.defend,"defend(self, 'sword', 'skeleton', 10)"))

    warrior=m.make('Vonaa Barile',type='archer',xyz=(uniform(-1,14),uniform(-18,-27),village_height),sex='female',face=directions[randint(0,7)])
    m.learn(warrior,(il.hunt,"hunt(self, 'bow', 'deer', 10)"))
    bow=m.make('bow',type='bow',xyz=(0,0,0), parent=warrior.id)
    m.own(warrior,bow)
    warriors.append(warrior)

    warrior=m.make('Lile Birloc', type='archer',xyz=(-2,-2,village_height),sex='female',face=directions[randint(0,7)])
    m.learn(warrior,(il.hunt,"hunt(self, 'bow', 'deer', 10)"))
    bow=m.make('bow',type='bow',xyz=(0,0,0), parent=warrior.id)
    m.own(warrior,bow)
    for i in range(0, 6):
        arrow=m.make('arrow',type='arrow',xyz=(0,0,0), parent=warrior.id)
        m.own(warrior,arrow)
    warriors.append(warrior)

    # Warriors all know where stuff is in the village
    m.know(warriors,village)

    # Warriors enjoy their food and drink
    m.learn(warriors,(il.lunch,"meal(self, 'ham','midday', 'inn')"))
    m.learn(warriors,(il.sup,"meal(self, 'beer', 'evening', 'inn')"))

    m.make('deer',type='deer',xyz=(2,2,village_height))

    # I am not sure if we need a guard
    #m.learn(guard,(il.patrol,"patrol(['m1', 'm2', 'm3', 'm4', 'm5', 'm6'])"))
    #m.tell_importance(guard,il.defend,'>',il.patrol)



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

village_height=0
forest_height=0
smithy_xyz=(-5,-30,village_height)
forest_xyz=(-20,-60,village_height)
tavern_xyz=(40,-00,village_height)
market_xyz=(-44,-10,village_height)
pig_stall_xyz=(-27,1,village_height)
pig_sty_xyz=(-26,2,village_height)
butcher_stall_xyz=(-41.5,-6.3,village_height)
tree_xyz=(-35,-25,village_height)


knowledge=[('axe','smithy'),
           ('smithy',smithy_xyz),
           ('forest',forest_xyz),
           ('tavern',tavern_xyz),
           ('market',market_xyz)]
mprices=[('pig','5')]
bprices=[('ham','2')]
bknowledge=[('market',butcher_stall_xyz)]
mknowledge=[('market',pig_stall_xyz)]
sknowledge=[('forest',(-30,-116,village_height)),
            ('stash',(-98,-97,village_height))]
gknowledge=[('m1',(-17, -1,    village_height)),
            ('m2',(-29, -1,    village_height)),
            ('m3',(-29, -7.5,  village_height)),
            ('m4',(-38, -10,   village_height)),
            ('m5',(-43, -15,   village_height)),
            ('m6',(-43, -14.5, village_height))]
wknowledge=[('w1',(90,-90,village_height)),
            ('w2',(110,-90,village_height)),
            ('w3',(110,90,village_height)),
            ('w4',(90,90,village_height))]

wander=(il.wander,"wander()")
forage=(il.forage,"forage()")
trade=(il.trade,"trade()")
keep=(il.keep,"keep()")
sell=(il.sell,"sell_trade()")
patrol=(il.patrol,"patrol()")

pig_goals=[(il.avoid,"avoid('wolf',10.0)"),
           (il.forage,"forage(self, 'acorn')"),
           (il.herd,"herd()")]

wolf_goals=[(il.forage,"forage(self, 'ham')"),
            (il.hunt,"predate(self,'pig',30.0)"),
            (il.hunt,"predate(self,'crab',20.0)"),
            (il.patrol,"patrol(['w1', 'w2', 'w3', 'w4'])")]

crab_goals=[(il.avoid,"avoid('wolf',10.0)"),
            (il.hunt,"predate_small(self,'pig',30.0,10.0)")]


#observer calls this
def default(mapeditor):
#   general things

    m=editor(mapeditor)

# a camp near the origin

    cfire=m.make('campfire',type='campfire',xyz=(0,4,village_height))
    m.make('fire',type='fire',xyz=(0.7,0.7,0),parent=cfire.id)
    m.make('tent',type='tent',xyz=(-1,8,village_height))
    m.make('lumber',type='lumber',xyz=(-1,3,village_height))
    m.make('lumber',type='lumber',xyz=(-1,2.5,village_height))

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
    m.make('oak',type='oak',xyz=(-70,-86,village_height))
    m.make('oak',type='oak',xyz=(-6,-77,village_height))
    m.make('oak',type='oak',xyz=(-24,-90,village_height))
    m.make('oak',type='oak',xyz=(-49,-90,village_height))
    m.make('oak',type='oak',xyz=(-86,-81,village_height))
    m.make('oak',type='oak',xyz=(-12,-98,village_height))
    m.make('oak',type='oak',xyz=(-35,-73,village_height))
    m.make('oak',type='oak',xyz=(-83,-66,village_height))
    m.make('oak',type='oak',xyz=(-87,-34,village_height))
    m.make('oak',type='oak',xyz=(-98,-28,village_height))
    m.make('oak',type='oak',xyz=(-75,31,village_height))

    m.make('weather',type='weather',desc='object that describes the weather',
           xyz=(0,1,0), rain=0.0)

#   animals
    for i in range(0, 2):
        m.make('skull', type='skull', xyz=(uniform(-100,100),uniform(-100,100),village_height))
        m.make('pelvis', type='pelvis', xyz=(uniform(-100,100),uniform(-100,100),village_height))
        m.make('arm', type='arm', xyz=(uniform(-100,100),uniform(-100,100),village_height))
        m.make('thigh', type='thigh', xyz=(uniform(-100,100),uniform(-100,100),village_height))
        m.make('shin', type='shin', xyz=(uniform(-100,100),uniform(-100,100),village_height))
        m.make('ribcage', type='ribcage', xyz=(uniform(-100,100),uniform(-100,100),village_height))

    for i in range(0, 30):
        m.make('coin',type='coin',xyz=(uniform(-6,-2),uniform(-2,2),village_height))



    lych=m.make('lych', type='lych', xyz=(2, 3, village_height))
    m.learn(lych,(il.assemble, "assemble(self, 'skeleton', ['skull', 'ribcage', 'arm', 'pelvis', 'thigh', 'shin'])"))
    piglet = m.make('pig', type='pig', xyz=(-31,-16,village_height))
    m.learn(piglet,pig_goals)

    wolf = m.make('wolf', type='wolf', xyz=(90,-90,village_height))
    m.learn(wolf,wolf_goals)
    m.know(wolf,wknowledge)
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

#   farmers

    sty=m.make('sty',type='sty',xyz=pig_sty_xyz,status=1.0)
    m.make('bstall',type='bstall',xyz=(-41,-5,village_height))

    home1_xyz=(90,-90,village_height)

    butcher=m.make('Bill Brondmore',type='butcher',desc='the butcher',
                   xyz=home1_xyz,age=probability.fertility_age,sex='male')
    m.learn(butcher,(il.trade,"trade(self, 'pig', 'cleaver', 'cut', 'ham', 'market')"))
    m.learn(butcher,(il.buy_livestock,"buy_livestock('pig', 1)"))
    m.know(butcher,bknowledge)
    m.price(butcher,bprices)
    cleaver=m.make('cleaver',desc='cleaver for cutting meat',place='market',
                   xyz=(-34,-8,village_height),status=1.0)
    m.own(butcher,cleaver)
    m.learn(butcher,(il.sell,"sell_trade('ham', 'market')"))
    coins=[]
    for i in range(0, 60):
        coins.append(m.make('coin',type='coin',xyz=(0,0,0),parent=butcher.id))
    m.own(butcher,coins)
    

    home2_xyz=(80,80,village_height)
    merchant=m.make('Indor Fergman',type='merchant',desc='the pig merchant',
                   xyz=home2_xyz,age=probability.fertility_age,sex='male')
    m.know(merchant,mknowledge)
    m.price(merchant,mprices)
    m.own(merchant,sty)
    m.learn(merchant,(il.keep,"keep('pig', 'sty')"))
    m.learn(merchant,(il.sell,"sell_trade('pig', 'market')"))
    piglets=[]
    for i in range(0, 6):
        piglets.append(m.make('pig',type='pig',xyz=(uniform(0,4),uniform(0,4),village_height),parent=sty.id))
    m.learn(piglets,pig_goals)
    m.own(merchant,piglets)

    guard=m.make('Pirla Krond', type='guard',desc='the village guard',
                xyz=pig_stall_xyz,age=probability.fertility_age,sex='female')
    m.know(guard,gknowledge)
    m.learn(guard,(il.defend,"defend(self, 'sword', 'skeleton', 10)"))
    m.learn(guard,(il.patrol,"patrol(['m1', 'm2', 'm3', 'm4', 'm5', 'm6'])"))
    m.tell_importance(guard,il.defend,'>',il.patrol)

    sword=m.make('sword', type='sword', xyz=(-35,-8,village_height))
    m.own(guard,sword)

def dont_run_me(foo):
    # generated entries
    # generated by forge/tools/iso_edit_tools/get_client_entities.pl from agrilan.map
    # agrilan.map was created by Pegasus
    m.make('birch_summer_1_us',type='birch_summer_1_us',desc='a dumb birch_summer_1_us object', xyz=(-72,85,village_height))
    m.make('birch_summer_1_us',type='birch_summer_1_us',desc='a dumb birch_summer_1_us object', xyz=(-72,81,village_height))
    m.make('poplar_summer_1_us',type='poplar_summer_1_us',desc='a dumb poplar_summer_1_us object', xyz=(-80,81,village_height))
    m.make('poplar_summer_1_us',type='poplar_summer_1_us',desc='a dumb poplar_summer_1_us object', xyz=(-76,71,village_height))
    m.make('birch_summer_1_us',type='birch_summer_1_us',desc='a dumb birch_summer_1_us object', xyz=(-84,71,village_height))
    m.make('poplar_summer_1_us',type='poplar_summer_1_us',desc='a dumb poplar_summer_1_us object', xyz=(-90,69,village_height))
    m.make('poplar_summer_1_us',type='poplar_summer_1_us',desc='a dumb poplar_summer_1_us object', xyz=(-74,67,village_height))
    m.make('poplar_summer_1_us',type='poplar_summer_1_us',desc='a dumb poplar_summer_1_us object', xyz=(-68,65,village_height))
    m.make('birch_summer_1_us',type='birch_summer_1_us',desc='a dumb birch_summer_1_us object', xyz=(-72,65,village_height))
    m.make('birch_summer_1_us',type='birch_summer_1_us',desc='a dumb birch_summer_1_us object', xyz=(-66,61,village_height))
    m.make('poplar_summer_1_us',type='poplar_summer_1_us',desc='a dumb poplar_summer_1_us object', xyz=(19,55,village_height))
    m.make('poplar_summer_1_us',type='poplar_summer_1_us',desc='a dumb poplar_summer_1_us object', xyz=(-70,55,village_height))
    m.make('fern_1_us',type='fern_1_us',desc='a dumb fern_1_us object', xyz=(-84,51,village_height))
    m.make('poplar_summer_1_us',type='poplar_summer_1_us',desc='a dumb poplar_summer_1_us object', xyz=(-75,41,village_height))
    m.make('poplar_summer_1_us',type='poplar_summer_1_us',desc='a dumb poplar_summer_1_us object', xyz=(-97,31,village_height))
    #m.make('oak_summer_1_us',type='oak_summer_1_us',desc='a dumb oak_summer_1_us object', xyz=(-87,25,village_height))
    m.make('roses_flowering_2_us',type='roses_flowering_2_us',desc='a dumb roses_flowering_2_us object', xyz=(83,23,village_height))
    m.make('roses_flowering_2_us',type='roses_flowering_2_us',desc='a dumb roses_flowering_2_us object', xyz=(79,23,village_height))
    m.make('roses_flowering_2_us',type='roses_flowering_2_us',desc='a dumb roses_flowering_2_us object', xyz=(75,23,village_height))
    m.make('roses_flowering_2_us',type='roses_flowering_2_us',desc='a dumb roses_flowering_2_us object', xyz=(59,23,village_height))
    m.make('roses_flowering_2_us',type='roses_flowering_2_us',desc='a dumb roses_flowering_2_us object', xyz=(56,23,village_height))
    m.make('roses_flowering_2_us',type='roses_flowering_2_us',desc='a dumb roses_flowering_2_us object', xyz=(53,23,village_height))
    m.make('roses_flowering_2_us',type='roses_flowering_2_us',desc='a dumb roses_flowering_2_us object', xyz=(49,23,village_height))
    m.make('birch_summer_1_us',type='birch_summer_1_us',desc='a dumb birch_summer_1_us object', xyz=(-95,21,village_height))
    m.make('roses_flowering_2_us',type='roses_flowering_2_us',desc='a dumb roses_flowering_2_us object', xyz=(49,19,village_height))
    m.make('birch_summer_1_us',type='birch_summer_1_us',desc='a dumb birch_summer_1_us object', xyz=(35,17,village_height))
    m.make('birch_summer_1_us',type='birch_summer_1_us',desc='a dumb birch_summer_1_us object', xyz=(-79,17,village_height))
    m.make('roses_flowering_2_us',type='roses_flowering_2_us',desc='a dumb roses_flowering_2_us object', xyz=(49,15,village_height))
    m.make('birch_summer_1_us',type='birch_summer_1_us',desc='a dumb birch_summer_1_us object', xyz=(31,15,village_height))
    m.make('poplar_summer_1_us',type='poplar_summer_1_us',desc='a dumb poplar_summer_1_us object', xyz=(-95,15,village_height))
    m.make('birch_summer_1_us',type='birch_summer_1_us',desc='a dumb birch_summer_1_us object', xyz=(35,13,village_height))
    m.make('house_deco_1_us',type='house_deco_1_us',desc='a dumb house_deco_1_us object', xyz=(53,11,village_height))
    m.make('roses_flowering_2_us',type='roses_flowering_2_us',desc='a dumb roses_flowering_2_us object', xyz=(49,11,village_height))
    m.make('birch_summer_1_us',type='birch_summer_1_us',desc='a dumb birch_summer_1_us object', xyz=(97,7,village_height))
    m.make('birch_summer_1_us',type='birch_summer_1_us',desc='a dumb birch_summer_1_us object', xyz=(-95,6,village_height))
    m.make('house_deco_4_us',type='house_deco_4_us',desc='a dumb house_deco_4_us object', xyz=(-38,3,village_height))
    m.make('house_deco_4_us',type='house_deco_4_us',desc='a dumb house_deco_4_us object', xyz=(-57,3,village_height))
    m.make('roses_flowering_2_us',type='roses_flowering_2_us',desc='a dumb roses_flowering_2_us object', xyz=(27,-1,village_height))
    m.make('roses_flowering_2_us',type='roses_flowering_2_us',desc='a dumb roses_flowering_2_us object', xyz=(-2,-2,village_height))
    #m.make('fence_study_ta_1',type='fence_study_ta_1',desc='a dumb fence_study_ta_1 object', xyz=(-27,-2,village_height))
    m.make('barrel_2_ta',type='barrel_2_ta',desc='a dumb barrel_2_ta object', xyz=(13,-3,village_height))
    m.make('roses_flowering_2_us',type='roses_flowering_2_us',desc='a dumb roses_flowering_2_us object', xyz=(27,-4,village_height))
    m.make('poplar_summer_1_us',type='poplar_summer_1_us',desc='a dumb poplar_summer_1_us object', xyz=(35,-5,village_height))
    m.make('barrel_2_ta',type='barrel_2_ta',desc='a dumb barrel_2_ta object', xyz=(13,-5,village_height))
    m.make('barrel_2_ta',type='barrel_2_ta',desc='a dumb barrel_2_ta object', xyz=(11,-5,village_height))
    m.make('box_wood_small_1_ta',type='box_wood_small_1_ta',desc='a dumb box_wood_small_1_ta object', xyz=(3,-5,village_height))
    m.make('barrel_2_ta',type='barrel_2_ta',desc='a dumb barrel_2_ta object', xyz=(1,-5,village_height))
    m.make('roses_flowering_2_us',type='roses_flowering_2_us',desc='a dumb roses_flowering_2_us object', xyz=(-2,-5,village_height))
    m.make('barrel_2_ta',type='barrel_2_ta',desc='a dumb barrel_2_ta object', xyz=(-29,-6,village_height))
    m.make('box_wood_small_1_ta',type='box_wood_small_1_ta',desc='a dumb box_wood_small_1_ta object', xyz=(-46,-6,village_height))
    m.make('box_wood_small_1_ta',type='box_wood_small_1_ta',desc='a dumb box_wood_small_1_ta object', xyz=(-47,-6,village_height))
    m.make('house_deco_1_us',type='house_deco_1_us',desc='a dumb house_deco_1_us object', xyz=(-75,-6,village_height))
    m.make('birch_summer_1_us',type='birch_summer_1_us',desc='a dumb birch_summer_1_us object', xyz=(-97,-6,village_height))
    m.make('roses_flowering_2_us',type='roses_flowering_2_us',desc='a dumb roses_flowering_2_us object', xyz=(27,-7,village_height))
    m.make('barrel_2_ta',type='barrel_2_ta',desc='a dumb barrel_2_ta object', xyz=(1,-7,village_height))
    m.make('barrel_2_ta',type='barrel_2_ta',desc='a dumb barrel_2_ta object', xyz=(-29,-7,village_height))
    m.make('barrel_2_ta',type='barrel_2_ta',desc='a dumb barrel_2_ta object', xyz=(-30,-7,village_height))
    m.make('jar_ornate_tall_med_1_ta',type='jar_ornate_tall_med_1_ta',desc='a dumb jar_ornate_tall_med_1_ta object', xyz=(-48,-7,village_height))
    m.make('maid_brown_sw_kl',type='maid_brown_sw_kl',desc='a dumb maid_brown_sw_kl object', xyz=(-49,-7,village_height))
    m.make('roses_flowering_2_us',type='roses_flowering_2_us',desc='a dumb roses_flowering_2_us object', xyz=(-2,-8,village_height))
    m.make('barrel_2_ta',type='barrel_2_ta',desc='a dumb barrel_2_ta object', xyz=(-29,-8,village_height))
    m.make('barrel_2_ta',type='barrel_2_ta',desc='a dumb barrel_2_ta object', xyz=(-30,-8,village_height))
    m.make('box_wood_small_1_ta',type='box_wood_small_1_ta',desc='a dumb box_wood_small_1_ta object', xyz=(-34,-8,village_height))
    m.make('box_wood_small_1_ta',type='box_wood_small_1_ta',desc='a dumb box_wood_small_1_ta object', xyz=(-36,-8,village_height))
    m.make('box_wood_small_1_ta',type='box_wood_small_1_ta',desc='a dumb box_wood_small_1_ta object', xyz=(-47,-8,village_height))
    m.make('jar_ornate_tall_med_1_ta',type='jar_ornate_tall_med_1_ta',desc='a dumb jar_ornate_tall_med_1_ta object', xyz=(-48,-8,village_height))
    m.make('jar_ornate_tall_med_1_ta',type='jar_ornate_tall_med_1_ta',desc='a dumb jar_ornate_tall_med_1_ta object', xyz=(-49,-8,village_height))
    m.make('poplar_summer_1_us',type='poplar_summer_1_us',desc='a dumb poplar_summer_1_us object', xyz=(35,-9,village_height))
    m.make('house_deco_4_us',type='house_deco_4_us',desc='a dumb house_deco_4_us object', xyz=(15,-9,village_height))
    m.make('chair_ne_wooden_dbrown_1_us',type='chair_ne_wooden_dbrown_1_us',desc='a dumb chair_ne_wooden_dbrown_1_us object', xyz=(5,-9,village_height))
    m.make('barrel_2_ta',type='barrel_2_ta',desc='a dumb barrel_2_ta object', xyz=(-29,-9,village_height))
    m.make('box_wood_small_1_ta',type='box_wood_small_1_ta',desc='a dumb box_wood_small_1_ta object', xyz=(-34,-9,village_height))
    m.make('box_wood_small_1_ta',type='box_wood_small_1_ta',desc='a dumb box_wood_small_1_ta object', xyz=(-35,-9,village_height))
    m.make('box_wood_small_1_ta',type='box_wood_small_1_ta',desc='a dumb box_wood_small_1_ta object', xyz=(-36,-9,village_height))
    m.make('mstall_blue_1_us',type='mstall_blue_1_us',desc='a dumb mstall_blue_1_us object', xyz=(-40,-9,village_height))
    m.make('barrel_2_ta',type='barrel_2_ta',desc='a dumb barrel_2_ta object', xyz=(-41,-9,village_height))
    m.make('mstall_blue_1_us',type='mstall_blue_1_us',desc='a dumb mstall_blue_1_us object', xyz=(-45,-9,village_height))
    m.make('barrel_2_ta',type='barrel_2_ta',desc='a dumb barrel_2_ta object', xyz=(-46,-9,village_height))
    m.make('box_wood_small_1_ta',type='box_wood_small_1_ta',desc='a dumb box_wood_small_1_ta object', xyz=(-47,-9,village_height))
    m.make('roses_flowering_2_us',type='roses_flowering_2_us',desc='a dumb roses_flowering_2_us object', xyz=(27,-10,village_height))
    m.make('box_wood_small_1_ta',type='box_wood_small_1_ta',desc='a dumb box_wood_small_1_ta object', xyz=(-36,-10,village_height))
    m.make('maid_red_se_kl',type='maid_red_se_kl',desc='a dumb maid_red_se_kl object', xyz=(-41,-11,village_height))
    m.make('barrel_2_ta',type='barrel_2_ta',desc='a dumb barrel_2_ta object', xyz=(-40,-10,village_height))
    m.make('barrel_2_ta',type='barrel_2_ta',desc='a dumb barrel_2_ta object', xyz=(-41,-10,village_height))
    m.make('barrel_2_ta',type='barrel_2_ta',desc='a dumb barrel_2_ta object', xyz=(-42,-10,village_height))
    m.make('barrel_2_ta',type='barrel_2_ta',desc='a dumb barrel_2_ta object', xyz=(-46,-10,village_height))
    m.make('barrel_2_ta',type='barrel_2_ta',desc='a dumb barrel_2_ta object', xyz=(-47,-10,village_height))
    m.make('house_deco_3_us',type='house_deco_3_us',desc='a dumb house_deco_3_us object', xyz=(57,-11,village_height))
    m.make('table_ne_wooden_dbrown_1_us',type='table_ne_wooden_dbrown_1_us',desc='a dumb table_ne_wooden_dbrown_1_us object', xyz=(5,-11,village_height))
    m.make('roses_flowering_2_us',type='roses_flowering_2_us',desc='a dumb roses_flowering_2_us object', xyz=(-2,-11,village_height))
    m.make('barrel_2_ta',type='barrel_2_ta',desc='a dumb barrel_2_ta object', xyz=(-29,-11,village_height))
    m.make('barrel_2_ta',type='barrel_2_ta',desc='a dumb barrel_2_ta object', xyz=(-30,-11,village_height))
    m.make('house_deco_3_us',type='house_deco_3_us',desc='a dumb house_deco_3_us object', xyz=(-27,-12,village_height))
    m.make('barrel_2_ta',type='barrel_2_ta',desc='a dumb barrel_2_ta object', xyz=(-29,-12,village_height))
    m.make('barrel_2_ta',type='barrel_2_ta',desc='a dumb barrel_2_ta object', xyz=(-30,-12,village_height))
    m.make('poplar_summer_1_us',type='poplar_summer_1_us',desc='a dumb poplar_summer_1_us object', xyz=(35,-13,village_height))
    m.make('roses_flowering_2_us',type='roses_flowering_2_us',desc='a dumb roses_flowering_2_us object', xyz=(27,-13,village_height))
    m.make('birch_summer_1_us',type='birch_summer_1_us',desc='a dumb birch_summer_1_us object', xyz=(-93,-15,village_height))
    m.make('roses_flowering_2_us',type='roses_flowering_2_us',desc='a dumb roses_flowering_2_us object', xyz=(27,-16,village_height))
    m.make('poplar_summer_1_us',type='poplar_summer_1_us',desc='a dumb poplar_summer_1_us object', xyz=(35,-17,village_height))
    m.make('box_wood_small_1_ta',type='box_wood_small_1_ta',desc='a dumb box_wood_small_1_ta object', xyz=(1,-17,village_height))
    m.make('birch_summer_1_us',type='birch_summer_1_us',desc='a dumb birch_summer_1_us object', xyz=(-88,-17,village_height))
    m.make('roses_flowering_2_us',type='roses_flowering_2_us',desc='a dumb roses_flowering_2_us object', xyz=(-2,-18,village_height))
    m.make('box_wood_small_1_ta',type='box_wood_small_1_ta',desc='a dumb box_wood_small_1_ta object', xyz=(-62,-19,village_height))
    m.make('box_wood_small_1_ta',type='box_wood_small_1_ta',desc='a dumb box_wood_small_1_ta object', xyz=(-63,-19,village_height))
    m.make('box_wood_small_1_ta',type='box_wood_small_1_ta',desc='a dumb box_wood_small_1_ta object', xyz=(-64,-19,village_height))
    m.make('box_wood_small_1_ta',type='box_wood_small_1_ta',desc='a dumb box_wood_small_1_ta object', xyz=(-66,-19,village_height))
    m.make('roses_flowering_2_us',type='roses_flowering_2_us',desc='a dumb roses_flowering_2_us object', xyz=(27,-20,village_height))
    m.make('box_wood_small_1_ta',type='box_wood_small_1_ta',desc='a dumb box_wood_small_1_ta object', xyz=(-65,-20,village_height))
    m.make('poplar_summer_1_us',type='poplar_summer_1_us',desc='a dumb poplar_summer_1_us object', xyz=(35,-21,village_height))
    m.make('roses_flowering_2_us',type='roses_flowering_2_us',desc='a dumb roses_flowering_2_us object', xyz=(-2,-21,village_height))
    m.make('box_wood_small_1_ta',type='box_wood_small_1_ta',desc='a dumb box_wood_small_1_ta object', xyz=(-63,-21,village_height))
    m.make('box_wood_small_1_ta',type='box_wood_small_1_ta',desc='a dumb box_wood_small_1_ta object', xyz=(-65,-21,village_height))
    m.make('house_deco_2_us',type='house_deco_2_us',desc='a dumb house_deco_2_us object', xyz=(6,-22,village_height))
    m.make('roses_flowering_2_us',type='roses_flowering_2_us',desc='a dumb roses_flowering_2_us object', xyz=(27,-23,village_height))
    m.make('roses_flowering_2_us',type='roses_flowering_2_us',desc='a dumb roses_flowering_2_us object', xyz=(-2,-24,village_height))
    m.make('house_deco_3_us',type='house_deco_3_us',desc='a dumb house_deco_3_us object', xyz=(57,-25,village_height))
    m.make('poplar_summer_1_us',type='poplar_summer_1_us',desc='a dumb poplar_summer_1_us object', xyz=(35,-25,village_height))
    m.make('roses_flowering_2_us',type='roses_flowering_2_us',desc='a dumb roses_flowering_2_us object', xyz=(-2,-27,village_height))
    #m.make('oak_summer_1_us',type='oak_summer_1_us',desc='a dumb oak_summer_1_us object', xyz=(-98,-28,village_height))
    m.make('poplar_summer_1_us',type='poplar_summer_1_us',desc='a dumb poplar_summer_1_us object', xyz=(35,-29,village_height))
    m.make('box_wood_small_1_ta',type='box_wood_small_1_ta',desc='a dumb box_wood_small_1_ta object', xyz=(-37,-30,village_height))
    m.make('box_wood_small_1_ta',type='box_wood_small_1_ta',desc='a dumb box_wood_small_1_ta object', xyz=(-38,-30,village_height))
    m.make('jar_ornate_tall_med_1_ta',type='jar_ornate_tall_med_1_ta',desc='a dumb jar_ornate_tall_med_1_ta object', xyz=(-40,-30,village_height))
    m.make('box_wood_small_1_ta',type='box_wood_small_1_ta',desc='a dumb box_wood_small_1_ta object', xyz=(-53,-30,village_height))
    m.make('box_wood_small_1_ta',type='box_wood_small_1_ta',desc='a dumb box_wood_small_1_ta object', xyz=(-54,-30,village_height))
    m.make('house_deco_1_us',type='house_deco_1_us',desc='a dumb house_deco_1_us object', xyz=(-63,-30,village_height))
    m.make('barrel_2_ta',type='barrel_2_ta',desc='a dumb barrel_2_ta object', xyz=(-30,-31,village_height))
    m.make('box_wood_small_1_ta',type='box_wood_small_1_ta',desc='a dumb box_wood_small_1_ta object', xyz=(-36,-31,village_height))
    m.make('box_wood_small_1_ta',type='box_wood_small_1_ta',desc='a dumb box_wood_small_1_ta object', xyz=(-37,-31,village_height))
    m.make('box_wood_small_1_ta',type='box_wood_small_1_ta',desc='a dumb box_wood_small_1_ta object', xyz=(-38,-31,village_height))
    m.make('barrel_2_ta',type='barrel_2_ta',desc='a dumb barrel_2_ta object', xyz=(-39,-31,village_height))
    m.make('barrel_2_ta',type='barrel_2_ta',desc='a dumb barrel_2_ta object', xyz=(-40,-31,village_height))
    m.make('jar_ornate_tall_med_1_ta',type='jar_ornate_tall_med_1_ta',desc='a dumb jar_ornate_tall_med_1_ta object', xyz=(-41,-31,village_height))
    m.make('box_wood_small_1_ta',type='box_wood_small_1_ta',desc='a dumb box_wood_small_1_ta object', xyz=(-48,-31,village_height))
    m.make('box_wood_small_1_ta',type='box_wood_small_1_ta',desc='a dumb box_wood_small_1_ta object', xyz=(-54,-31,village_height))
    m.make('barrel_2_ta',type='barrel_2_ta',desc='a dumb barrel_2_ta object', xyz=(-29,-32,village_height))
    m.make('barrel_2_ta',type='barrel_2_ta',desc='a dumb barrel_2_ta object', xyz=(-30,-32,village_height))
    m.make('box_wood_small_1_ta',type='box_wood_small_1_ta',desc='a dumb box_wood_small_1_ta object', xyz=(-37,-32,village_height))
    m.make('box_wood_small_1_ta',type='box_wood_small_1_ta',desc='a dumb box_wood_small_1_ta object', xyz=(-38,-32,village_height))
    m.make('barrel_2_ta',type='barrel_2_ta',desc='a dumb barrel_2_ta object', xyz=(-40,-32,village_height))
    m.make('jar_ornate_tall_med_1_ta',type='jar_ornate_tall_med_1_ta',desc='a dumb jar_ornate_tall_med_1_ta object', xyz=(-41,-32,village_height))
    m.make('mstall_blue_1_us',type='mstall_blue_1_us',desc='a dumb mstall_blue_1_us object', xyz=(-45,-32,village_height))
    m.make('barrel_2_ta',type='barrel_2_ta',desc='a dumb barrel_2_ta object', xyz=(-46,-32,village_height))
    m.make('box_wood_small_1_ta',type='box_wood_small_1_ta',desc='a dumb box_wood_small_1_ta object', xyz=(-47,-32,village_height))
    m.make('box_wood_small_1_ta',type='box_wood_small_1_ta',desc='a dumb box_wood_small_1_ta object', xyz=(-48,-32,village_height))
    m.make('poplar_summer_1_us',type='poplar_summer_1_us',desc='a dumb poplar_summer_1_us object', xyz=(35,-33,village_height))
    m.make('barrel_2_ta',type='barrel_2_ta',desc='a dumb barrel_2_ta object', xyz=(-29,-33,village_height))
    m.make('barrel_2_ta',type='barrel_2_ta',desc='a dumb barrel_2_ta object', xyz=(-30,-33,village_height))
    m.make('barrel_2_ta',type='barrel_2_ta',desc='a dumb barrel_2_ta object', xyz=(-31,-33,village_height))
    m.make('barrel_2_ta',type='barrel_2_ta',desc='a dumb barrel_2_ta object', xyz=(-46,-33,village_height))
    m.make('box_wood_small_1_ta',type='box_wood_small_1_ta',desc='a dumb box_wood_small_1_ta object', xyz=(-48,-33,village_height))
    m.make('barrel_2_ta',type='barrel_2_ta',desc='a dumb barrel_2_ta object', xyz=(-29,-34,village_height))
    m.make('barrel_2_ta',type='barrel_2_ta',desc='a dumb barrel_2_ta object', xyz=(-30,-34,village_height))
    m.make('barrel_2_ta',type='barrel_2_ta',desc='a dumb barrel_2_ta object', xyz=(-31,-34,village_height))
    m.make('maid_red_sw_kl',type='maid_red_sw_kl',desc='a dumb maid_red_sw_kl object', xyz=(-42,-34,village_height))
    #m.make('oak_summer_1_us',type='oak_summer_1_us',desc='a dumb oak_summer_1_us object', xyz=(-87,-34,village_height))
    m.make('barrel_2_ta',type='barrel_2_ta',desc='a dumb barrel_2_ta object', xyz=(-29,-35,village_height))
    m.make('barrel_2_ta',type='barrel_2_ta',desc='a dumb barrel_2_ta object', xyz=(-30,-35,village_height))
    m.make('house_deco_3_us',type='house_deco_3_us',desc='a dumb house_deco_3_us object', xyz=(-27,-36,village_height))
    m.make('barrel_2_ta',type='barrel_2_ta',desc='a dumb barrel_2_ta object', xyz=(-29,-36,village_height))
    m.make('poplar_summer_1_us',type='poplar_summer_1_us',desc='a dumb poplar_summer_1_us object', xyz=(35,-37,village_height))
    m.make('house_deco_3_us',type='house_deco_3_us',desc='a dumb house_deco_3_us object', xyz=(57,-39,village_height))
    m.make('poplar_summer_1_us',type='poplar_summer_1_us',desc='a dumb poplar_summer_1_us object', xyz=(35,-41,village_height))
    m.make('poplar_summer_1_us',type='poplar_summer_1_us',desc='a dumb poplar_summer_1_us object', xyz=(-97,-41,village_height))
    m.make('barrel_2_ta',type='barrel_2_ta',desc='a dumb barrel_2_ta object', xyz=(73,-45,village_height))
    m.make('poplar_summer_1_us',type='poplar_summer_1_us',desc='a dumb poplar_summer_1_us object', xyz=(35,-45,village_height))
    m.make('box_wood_small_1_ta',type='box_wood_small_1_ta',desc='a dumb box_wood_small_1_ta object', xyz=(55,-47,village_height))
    m.make('box_wood_small_1_ta',type='box_wood_small_1_ta',desc='a dumb box_wood_small_1_ta object', xyz=(55,-49,village_height))
    m.make('poplar_summer_1_us',type='poplar_summer_1_us',desc='a dumb poplar_summer_1_us object', xyz=(35,-49,village_height))
    m.make('house_deco_1_us',type='house_deco_1_us',desc='a dumb house_deco_1_us object', xyz=(-34,-50,village_height))
    m.make('house_deco_1_us',type='house_deco_1_us',desc='a dumb house_deco_1_us object', xyz=(-45,-50,village_height))
    m.make('house_deco_1_us',type='house_deco_1_us',desc='a dumb house_deco_1_us object', xyz=(-55,-50,village_height))
    m.make('barrel_2_ta',type='barrel_2_ta',desc='a dumb barrel_2_ta object', xyz=(55,-51,village_height))
    m.make('barrel_2_ta',type='barrel_2_ta',desc='a dumb barrel_2_ta object', xyz=(53,-51,village_height))
    m.make('house_deco_3_us',type='house_deco_3_us',desc='a dumb house_deco_3_us object', xyz=(57,-53,village_height))
    m.make('barrel_2_ta',type='barrel_2_ta',desc='a dumb barrel_2_ta object', xyz=(53,-53,village_height))
    m.make('poplar_summer_1_us',type='poplar_summer_1_us',desc='a dumb poplar_summer_1_us object', xyz=(35,-53,village_height))
    m.make('poplar_summer_1_us',type='poplar_summer_1_us',desc='a dumb poplar_summer_1_us object', xyz=(35,-57,village_height))
    m.make('poplar_summer_1_us',type='poplar_summer_1_us',desc='a dumb poplar_summer_1_us object', xyz=(35,-61,village_height))
    m.make('birch_summer_1_us',type='birch_summer_1_us',desc='a dumb birch_summer_1_us object', xyz=(9,-64,village_height))
    m.make('poplar_summer_1_us',type='poplar_summer_1_us',desc='a dumb poplar_summer_1_us object', xyz=(35,-65,village_height))
    m.make('stall_1_ta',type='stall_1_ta',desc='a dumb stall_1_ta object', xyz=(24,-65,village_height))
    m.make('birch_summer_1_us',type='birch_summer_1_us',desc='a dumb birch_summer_1_us object', xyz=(18,-65,village_height))
    m.make('birch_summer_1_us',type='birch_summer_1_us',desc='a dumb birch_summer_1_us object', xyz=(-98,-66,village_height))
    m.make('house_deco_4_us',type='house_deco_4_us',desc='a dumb house_deco_4_us object', xyz=(57,-67,village_height))
    m.make('chair_ne_wooden_dbrown_1_us',type='chair_ne_wooden_dbrown_1_us',desc='a dumb chair_ne_wooden_dbrown_1_us object', xyz=(51,-67,village_height))
    m.make('chair_ne_wooden_dbrown_1_us',type='chair_ne_wooden_dbrown_1_us',desc='a dumb chair_ne_wooden_dbrown_1_us object', xyz=(49,-67,village_height))
    m.make('birch_summer_1_us',type='birch_summer_1_us',desc='a dumb birch_summer_1_us object', xyz=(-93,-67,village_height))
    m.make('birch_summer_1_us',type='birch_summer_1_us',desc='a dumb birch_summer_1_us object', xyz=(9,-68,village_height))
    m.make('table_ne_wooden_dbrown_1_us',type='table_ne_wooden_dbrown_1_us',desc='a dumb table_ne_wooden_dbrown_1_us object', xyz=(49,-69,village_height))
    m.make('poplar_summer_1_us',type='poplar_summer_1_us',desc='a dumb poplar_summer_1_us object', xyz=(35,-69,village_height))
    m.make('birch_summer_1_us',type='birch_summer_1_us',desc='a dumb birch_summer_1_us object', xyz=(-97,-69,village_height))
    m.make('birch_summer_1_us',type='birch_summer_1_us',desc='a dumb birch_summer_1_us object', xyz=(25,-71,village_height))
    m.make('birch_summer_1_us',type='birch_summer_1_us',desc='a dumb birch_summer_1_us object', xyz=(9,-72,village_height))
    m.make('birch_summer_1_us',type='birch_summer_1_us',desc='a dumb birch_summer_1_us object', xyz=(-67,-72,village_height))
    m.make('poplar_summer_1_us',type='poplar_summer_1_us',desc='a dumb poplar_summer_1_us object', xyz=(35,-73,village_height))
    m.make('birch_summer_1_us',type='birch_summer_1_us',desc='a dumb birch_summer_1_us object', xyz=(12,-73,village_height))
    m.make('birch_summer_1_us',type='birch_summer_1_us',desc='a dumb birch_summer_1_us object', xyz=(25,-75,village_height))
    m.make('poplar_summer_1_us',type='poplar_summer_1_us',desc='a dumb poplar_summer_1_us object', xyz=(-97,-75,village_height))
    m.make('birch_summer_1_us',type='birch_summer_1_us',desc='a dumb birch_summer_1_us object', xyz=(-68,-76,village_height))
    m.make('poplar_summer_1_us',type='poplar_summer_1_us',desc='a dumb poplar_summer_1_us object', xyz=(-90,-77,village_height))
    m.make('birch_summer_1_us',type='birch_summer_1_us',desc='a dumb birch_summer_1_us object', xyz=(-98,-77,village_height))
    m.make('stall_1_ta',type='stall_1_ta',desc='a dumb stall_1_ta object', xyz=(16,-79,village_height))
    m.make('stall_1_ta',type='stall_1_ta',desc='a dumb stall_1_ta object', xyz=(13,-79,village_height))
    #m.make('oak_summer_1_us',type='oak_summer_1_us',desc='a dumb oak_summer_1_us object', xyz=(-97,-80,village_height))
    m.make('house_deco_2_us',type='house_deco_2_us',desc='a dumb house_deco_2_us object', xyz=(51,-81,village_height))
    #m.make('oak_summer_1_us',type='oak_summer_1_us',desc='a dumb oak_summer_1_us object', xyz=(-86,-81,village_height))
    m.make('birch_summer_1_us',type='birch_summer_1_us',desc='a dumb birch_summer_1_us object', xyz=(25,-82,village_height))
    m.make('birch_summer_1_us',type='birch_summer_1_us',desc='a dumb birch_summer_1_us object', xyz=(22,-82,village_height))
    m.make('birch_summer_1_us',type='birch_summer_1_us',desc='a dumb birch_summer_1_us object', xyz=(-21,-82,village_height))
    m.make('birch_summer_1_us',type='birch_summer_1_us',desc='a dumb birch_summer_1_us object', xyz=(-78,-82,village_height))
    m.make('birch_summer_1_us',type='birch_summer_1_us',desc='a dumb birch_summer_1_us object', xyz=(-13,-83,village_height))
    m.make('birch_summer_1_us',type='birch_summer_1_us',desc='a dumb birch_summer_1_us object', xyz=(-85,-83,village_height))
    m.make('birch_summer_1_us',type='birch_summer_1_us',desc='a dumb birch_summer_1_us object', xyz=(-34,-84,village_height))
    m.make('birch_summer_1_us',type='birch_summer_1_us',desc='a dumb birch_summer_1_us object', xyz=(-53,-84,village_height))
    m.make('poplar_summer_1_us',type='poplar_summer_1_us',desc='a dumb poplar_summer_1_us object', xyz=(-93,-84,village_height))
    m.make('birch_summer_1_us',type='birch_summer_1_us',desc='a dumb birch_summer_1_us object', xyz=(-98,-84,village_height))
    m.make('birch_summer_1_us',type='birch_summer_1_us',desc='a dumb birch_summer_1_us object', xyz=(1,-85,village_height))
    m.make('birch_summer_1_us',type='birch_summer_1_us',desc='a dumb birch_summer_1_us object', xyz=(-75,-85,village_height))
    m.make('birch_summer_1_us',type='birch_summer_1_us',desc='a dumb birch_summer_1_us object', xyz=(28,-86,village_height))
    m.make('birch_summer_1_us',type='birch_summer_1_us',desc='a dumb birch_summer_1_us object', xyz=(-60,-86,village_height))
    #m.make('oak_summer_1_us',type='oak_summer_1_us',desc='a dumb oak_summer_1_us object', xyz=(-70,-86,village_height))
    m.make('poplar_summer_1_us',type='poplar_summer_1_us',desc='a dumb poplar_summer_1_us object', xyz=(-4,-87,village_height))
    #m.make('oak_summer_1_us',type='oak_summer_1_us',desc='a dumb oak_summer_1_us object', xyz=(-10,-87,village_height))
    m.make('poplar_summer_1_us',type='poplar_summer_1_us',desc='a dumb poplar_summer_1_us object', xyz=(-28,-87,village_height))
    m.make('birch_summer_1_us',type='birch_summer_1_us',desc='a dumb birch_summer_1_us object', xyz=(-34,-88,village_height))
    m.make('birch_summer_1_us',type='birch_summer_1_us',desc='a dumb birch_summer_1_us object', xyz=(-69,-88,village_height))
    m.make('birch_summer_1_us',type='birch_summer_1_us',desc='a dumb birch_summer_1_us object', xyz=(-98,-88,village_height))
    m.make('barrel_2_ta',type='barrel_2_ta',desc='a dumb barrel_2_ta object', xyz=(81,-89,village_height))
    m.make('roses_flowering_2_us',type='roses_flowering_2_us',desc='a dumb roses_flowering_2_us object', xyz=(79,-89,village_height))
    m.make('birch_summer_1_us',type='birch_summer_1_us',desc='a dumb birch_summer_1_us object', xyz=(-28,-89,village_height))
    m.make('poplar_summer_1_us',type='poplar_summer_1_us',desc='a dumb poplar_summer_1_us object', xyz=(-66,-89,village_height))
    m.make('birch_summer_1_us',type='birch_summer_1_us',desc='a dumb birch_summer_1_us object', xyz=(-74,-89,village_height))
    m.make('barrel_2_ta',type='barrel_2_ta',desc='a dumb barrel_2_ta object', xyz=(36,-90,village_height))
    #m.make('oak_summer_1_us',type='oak_summer_1_us',desc='a dumb oak_summer_1_us object', xyz=(-24,-90,village_height))
    #m.make('oak_summer_1_us',type='oak_summer_1_us',desc='a dumb oak_summer_1_us object', xyz=(-49,-90,village_height))
    m.make('poplar_summer_1_us',type='poplar_summer_1_us',desc='a dumb poplar_summer_1_us object', xyz=(-83,-90,village_height))
    m.make('birch_summer_1_us',type='birch_summer_1_us',desc='a dumb birch_summer_1_us object', xyz=(1,-91,village_height))
    m.make('poplar_summer_1_us',type='poplar_summer_1_us',desc='a dumb poplar_summer_1_us object', xyz=(-12,-91,village_height))
    m.make('birch_summer_1_us',type='birch_summer_1_us',desc='a dumb birch_summer_1_us object', xyz=(-92,-91,village_height))
    m.make('stall_1_ta',type='stall_1_ta',desc='a dumb stall_1_ta object', xyz=(38,-92,village_height))
    m.make('birch_summer_1_us',type='birch_summer_1_us',desc='a dumb birch_summer_1_us object', xyz=(30,-92,village_height))
    m.make('poplar_summer_1_us',type='poplar_summer_1_us',desc='a dumb poplar_summer_1_us object', xyz=(-55,-92,village_height))
    m.make('poplar_summer_1_us',type='poplar_summer_1_us',desc='a dumb poplar_summer_1_us object', xyz=(-98,-92,village_height))
    m.make('roses_flowering_2_us',type='roses_flowering_2_us',desc='a dumb roses_flowering_2_us object', xyz=(79,-93,village_height))
    m.make('birch_summer_1_us',type='birch_summer_1_us',desc='a dumb birch_summer_1_us object', xyz=(23,-93,village_height))
    m.make('birch_summer_1_us',type='birch_summer_1_us',desc='a dumb birch_summer_1_us object', xyz=(-62,-93,village_height))
    m.make('birch_summer_1_us',type='birch_summer_1_us',desc='a dumb birch_summer_1_us object', xyz=(-77,-93,village_height))
    m.make('birch_summer_1_us',type='birch_summer_1_us',desc='a dumb birch_summer_1_us object', xyz=(41,-94,village_height))
    m.make('birch_summer_1_us',type='birch_summer_1_us',desc='a dumb birch_summer_1_us object', xyz=(12,-94,village_height))
    m.make('poplar_summer_1_us',type='poplar_summer_1_us',desc='a dumb poplar_summer_1_us object', xyz=(4,-94,village_height))
    m.make('birch_summer_1_us',type='birch_summer_1_us',desc='a dumb birch_summer_1_us object', xyz=(-46,-94,village_height))
    m.make('poplar_summer_1_us',type='poplar_summer_1_us',desc='a dumb poplar_summer_1_us object', xyz=(-60,-94,village_height))
    m.make('birch_summer_1_us',type='birch_summer_1_us',desc='a dumb birch_summer_1_us object', xyz=(-15,-95,village_height))
    m.make('birch_summer_1_us',type='birch_summer_1_us',desc='a dumb birch_summer_1_us object', xyz=(-26,-95,village_height))
    m.make('poplar_summer_1_us',type='poplar_summer_1_us',desc='a dumb poplar_summer_1_us object', xyz=(-89,-95,village_height))
    m.make('poplar_summer_1_us',type='poplar_summer_1_us',desc='a dumb poplar_summer_1_us object', xyz=(-1,-96,village_height))
    m.make('birch_summer_1_us',type='birch_summer_1_us',desc='a dumb birch_summer_1_us object', xyz=(-74,-96,village_height))
    m.make('poplar_summer_1_us',type='poplar_summer_1_us',desc='a dumb poplar_summer_1_us object', xyz=(-94,-96,village_height))
    m.make('roses_flowering_2_us',type='roses_flowering_2_us',desc='a dumb roses_flowering_2_us object', xyz=(79,-97,village_height))
    m.make('house_deco_1_us',type='house_deco_1_us',desc='a dumb house_deco_1_us object', xyz=(67,-97,village_height))
    m.make('roses_flowering_2_us',type='roses_flowering_2_us',desc='a dumb roses_flowering_2_us object', xyz=(61,-97,village_height))
    m.make('roses_flowering_2_us',type='roses_flowering_2_us',desc='a dumb roses_flowering_2_us object', xyz=(57,-97,village_height))
    m.make('birch_summer_1_us',type='birch_summer_1_us',desc='a dumb birch_summer_1_us object', xyz=(49,-97,village_height))
    m.make('birch_summer_1_us',type='birch_summer_1_us',desc='a dumb birch_summer_1_us object', xyz=(29,-97,village_height))
    m.make('poplar_summer_1_us',type='poplar_summer_1_us',desc='a dumb poplar_summer_1_us object', xyz=(-22,-97,village_height))
    m.make('birch_summer_1_us',type='birch_summer_1_us',desc='a dumb birch_summer_1_us object', xyz=(-25,-97,village_height))
    m.make('poplar_summer_1_us',type='poplar_summer_1_us',desc='a dumb poplar_summer_1_us object', xyz=(-45,-97,village_height))
    m.make('birch_summer_1_us',type='birch_summer_1_us',desc='a dumb birch_summer_1_us object', xyz=(-89,-97,village_height))
    m.make('birch_summer_1_us',type='birch_summer_1_us',desc='a dumb birch_summer_1_us object', xyz=(-98,-97,village_height))
    m.make('stall_1_ta',type='stall_1_ta',desc='a dumb stall_1_ta object', xyz=(12,-98,village_height))
    m.make('poplar_summer_1_us',type='poplar_summer_1_us',desc='a dumb poplar_summer_1_us object', xyz=(5,-98,village_height))
    m.make('birch_summer_1_us',type='birch_summer_1_us',desc='a dumb birch_summer_1_us object', xyz=(2,-98,village_height))
    m.make('birch_summer_1_us',type='birch_summer_1_us',desc='a dumb birch_summer_1_us object', xyz=(-3,-98,village_height))
    #m.make('oak_summer_1_us',type='oak_summer_1_us',desc='a dumb oak_summer_1_us object', xyz=(-12,-98,village_height))
    m.make('poplar_summer_1_us',type='poplar_summer_1_us',desc='a dumb poplar_summer_1_us object', xyz=(-33,-98,village_height))
    m.make('birch_summer_1_us',type='birch_summer_1_us',desc='a dumb birch_summer_1_us object', xyz=(-36,-98,village_height))
    #m.make('oak_summer_1_us',type='oak_summer_1_us',desc='a dumb oak_summer_1_us object', xyz=(-41,-98,village_height))
    m.make('poplar_summer_1_us',type='poplar_summer_1_us',desc='a dumb poplar_summer_1_us object', xyz=(-53,-98,village_height))
    m.make('birch_summer_1_us',type='birch_summer_1_us',desc='a dumb birch_summer_1_us object', xyz=(-65,-98,village_height))
    m.make('poplar_summer_1_us',type='poplar_summer_1_us',desc='a dumb poplar_summer_1_us object', xyz=(-84,-98,village_height))
    m.make('birch_summer_1_us',type='birch_summer_1_us',desc='a dumb birch_summer_1_us object', xyz=(45,-99,village_height))
    m.make('birch_summer_1_us',type='birch_summer_1_us',desc='a dumb birch_summer_1_us object', xyz=(-15,-99,village_height))
    m.make('birch_summer_1_us',type='birch_summer_1_us',desc='a dumb birch_summer_1_us object', xyz=(-44,-99,village_height))
    m.make('birch_summer_1_us',type='birch_summer_1_us',desc='a dumb birch_summer_1_us object', xyz=(-55,-99,village_height))

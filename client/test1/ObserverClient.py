#!/usr/bin/env python
#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 1999 Aloril (See the file COPYING for details).

import init
import pdb
import sys

from tools.cmd_utilities import *
from CreatorClient import CreatorClient
from CommClient import CommClient
import define_world
from common import persistence,const
from random import randint
import time,string

def sc(x,min,max,s):
    """Convert real-world coordinates to screen coordinates
    
       x: real word x (for example 10.5)
       min: left side of screen in real world (for example -300,0)
       max: right side of screen in real world (for example 300.0)
       screen width (for example 49)"""
    res=int(s/(max-min)*(x-min))
    if res<0: res=0
    if res>=s: res=s-1
    return res

class ObserverClient(CommClient):
    def __init__(self, argv, log_file, display = 1):
        self.display = display
        self.connection=self.connect(argv)
        if const.debug_level>=1:
            print "const.debug_level>=1, logging to",log_file
            self.connection.log_file=open(log_file,"w")
        adminAccount=persistence.load_admin_account()
        self.player=self.create_player(adminAccount.id,adminAccount.password)
        self.character=self.create_character("creator",CreatorClient)
    def load_default(self):
        define_world.default(self.character)
    def idle(self):
        time.sleep(0.1)
        if not self.display: return
        self.map=self.character.map
        wait=0.0
        xmin=-300.0
        xmax=-xmin
        ymin=-150.0
        ymax=-ymin
        sizex=49
        sizey=20
        goal_width=30
        def scx(x, min=xmin, max=xmax, s=sizex): return sc(x,min,max,s)
        def scy(y, min=ymin, max=ymax, s=sizey): return sc(-y,min,max,s)
        screen = [None]*sizex
        for i in range(sizex):
            screen[i] = ['.'] * sizey
        house_list=[]
        mind_list=[]
        fire_list=[]
        for t in self.map.things.values():
            if len(t.type)>=1:
                typething = string.split(t.type[0], ".")
                if string.find(typething[-1], "house")>=0: 
                    house_list.append(t)
                elif string.find(typething[-1], "farmer")>=0 or string.find(typething[-1], "smith")>=0 or string.find(typething[-1], "character")>=0 or string.find(typething[-1], "creator")>=0:
                    mind_list.append(t)
                elif string.find(typething[-1], "fire"):
                    fire_list.append(t)
            else:
                print "CHEAT!: somewhere is entity without type!:",t
        self.time=str(self.character.time)
        status_str=self.time+" Count of minds: "+`len(mind_list)`
        print chr(27)+"[H",status_str+" "*(sizex-len(status_str)-1)
        for t in house_list:
            (x,y,z)=t.get_xyz()
            screen[scx(x)][scy(y)]=t.name[0]
        yind=1
        goal_txt={}
        for m in mind_list:
            (x,y,z)=m.get_xyz()
            x,y=scx(x),scy(y)
            screen[x][y]=m.name[0]
            screen[x+1][y]=m.name[-1]
            if hasattr(m,"goal"):
                goal_txt[yind]=(`m`+":"+m.goal+" "*goal_width)[:goal_width]
                yind=yind+1
        for t in fire_list:
            (x,y,z)=t.get_xyz()
            screen[scx(x)][scy(y)]='F'
            wait=0.2
        out=[]
        for y in range(sizey):
            for x in range(sizex):
                out.append(screen[x][y])
            out.append(goal_txt.get(y," "*goal_width))
            out.append('\n')
        print string.join(out,'')

if __name__=="__main__":
    try:
        if init.display:
            print "Use --nodisplay to hide 'text graphics'"
        observer=ObserverClient(sys.argv,"observer.log", init.display)
        observer.load_default()
        observer.run()
    except:
        import pdb,sys,traceback
        traceback.print_exc()
        info=sys.exc_info()
        pdb.post_mortem(info[2])


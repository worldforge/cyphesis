#!/usr/bin/env python
#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 1999 Aloril (See the file COPYING for details).

import init
import sys,time,string
from atlas import *
from transport import *
from ObserverClient import ObserverClient
from tools.cmd_utilities import *
from random import randint

class TestException(Exception): pass
def deb(): import pdb; pdb.pm()

#Test client, will be later replaced with actual client

## if len(sys.argv)>1:
##     server=sys.argv[1]
## else:
##     server="localhost"
## if len(sys.argv)>2:
##     port=sys.argv[2]
## else:
##     port=6767

## try:
##     c=connect(server,port)
## except:
##     print "Could not connect to",server,port
##     raise

## c.log_file=open("code.log","w")

def send_code(code, type):
    global rop,r
    rop=c.call(Operation("code",Entity(code=code,code_type=type),from_=player.id))
    r=rop[0][0].result
    r=string.replace(r,"[newline]","\n")
    if r or type=="eval": print r

def ex(code):
    send_code(code,"exec")

def e(code):
    send_code(code,"eval")

def createplayer ():
	playerName="player"+`randint(0,100)`
	player=Entity(playerName,password="player_password")
	createAccountOp=Operation("create",player)
	res = c.call(createAccountOp)
	return res[0][0]



def createchar (player):
	character=Entity(type=["farmer"], auto=1)
	createOp=Operation("create", character, from_=player.id)
	res = c.call(createOp)
	return res[0][0]

def login(player):
        return c.call(Operation("login", Entity(player, password="player_password")))

def start():
    global player,char,observer,c
    #player=createplayer()
    #char=createchar(player)
    observer=ObserverClient(sys.argv,"code.log")
    player=observer.player
    char=observer.character
    c=observer.connection
    #c.send(Operation("save",Entity("world.xml"),to="world_0"))

def move(coordinates=1.0, velocity=0.0):
    if type(coordinates)==FloatType: coordinates=[coordinates,0.0,0.0]
    if type(velocity)==FloatType: velocity=[velocity,0.0,0.0]
    loc = char.location
    loc.coordinates = coordinates
    loc.velocity = velocity
    op=Operation("move",char,from_=char.id)
    c.send(op)
    global res
    res=c.pop()
    print "Received",len(res),"messages"
    #print res

def menu():
    print "l: list all objects"
    while 1:
        choice=raw_input()
        if choice=='l':
            e("self.objects")

start()
c.send(Operation("load",Entity("foo.xml"),to="server"))
e("self.server.id_dict")

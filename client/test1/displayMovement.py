#!/usr/bin/env python
#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 1999 Aloril (See the file COPYING for details).
#this client displays movement...

import init
import sys,time
from atlas import *
from transport import *

from random import randint

class TestException(Exception): pass
def deb(): import pdb; pdb.pm()

#Test client, will be later replaced with actual client

if len(sys.argv)>1:
    server=sys.argv[1]
else:
    server="localhost"
if len(sys.argv)>2:
    port=sys.argv[2]
else:
    port=6767

try:
    tr=connect(server,port)
except:
    print "Could not connect to",server,port
    raise

playerName="player"+`randint(0,100)`
player=Entity(playerName,password="player_password")
createAccountOp=Operation("create",player)
res=tr.call(createAccountOp)
op=res[0]
if op.id!="info":
    print "account creation failed, trying to login:"
    #player=Entity("admin",password="admin_password")
    player=Entity(playerName,password="player_password")
    loginOp=Operation("login",player)
    res=tr.call(loginOp)
    op=res[0]
    if op.id!="info":
        print "Error in login:\n",res
        sys.exit()
ent=op[0]
print "Player id:",playerName
if hasattr(ent,"characters"):
    print "Characters:",ent.characters

character=Entity(type=["farmer"],auto=1)
createOp=Operation("create",character,from_=player.id)
res=tr.call(createOp)
print "Our character:\n",res

prev_time=0.0
while 1: #just observe moves...
    res=tr.pop()
    if res:
        for op in res:
            if op.id=="sight":
                op2=op[0]
                if op2.get_name()=="op":
                    ent=op2[0]
                    if op2.id=="create":
                        print "Created: %s: %s" % (ent.type[0],ent.id)
                    elif op2.id=="move":
                        cur_time=op2.time.s
                        print "Moved:",ent.id,cur_time,cur_time-prev_time,\
                              `ent.location`,"Velocity:",ent.location.velocity
                        prev_time=cur_time
    time.sleep(0.1)


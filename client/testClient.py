#!/usr/bin/env python
#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 1999 Aloril (See the file COPYING for details).

import init
import sys,time
from atlas import *
from transport import *

from random import randint

class TestException(Exception): pass
def deb(): import pdb; pdb.pm()

def deep(id, depth=60):
    talkOp=Operation("talk",Entity(say="CRASH?"),
                     from_=id)
    op=op0=talkOp
    for i in range(depth):
        op=Operation("deep")
        op0.append(op)
        op0=op
    return talkOp

#Test client, will be later replaced with actual client

if len(sys.argv)>1:
    server=sys.argv[1]
else:
    server="localhost"
if len(sys.argv)>2:
    port=int(sys.argv[2])
else:
    port=6767

try:
    tr=connect(server,port)
except:
    print "Could not connect to",server,port
    raise

tr.log_file=open("test.log","w")

#playerName="player"+`randint(0,100)`
playerName="player"
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

for i in range(1):
    character=Entity(name="test client",type=["farmer"],auto=1)
    createOp=Operation("create",character,from_=player.id)
    res=tr.call(createOp)
    print "Our "+`i`+":th character:\n",res

while 0: #just observe...
    res=tr.pop()
    if res: print "Received:\n",res
    time.sleep(0.1)

character=res[0][0]
oldLoc=loc=character.location
loc.coordinates=[1.0,2.0,3.0]
moveOp=Operation("move",character,from_=character.id)
print moveOp
#del character.drunkness

#talkOp=Operation("talk",Entity(say="Hello world!"),from_=character.id)
#talkOp=Operation("talk",Entity(say="Eating is more important than sleeping. Really?"),
#                 from_=character.id)
talkOp=Operation("talk",Entity(say="Jowes Harrowe, Eating is more important than sleeping."),
                 from_=character.id)
print talkOp
tr.send(talkOp)

ent=Entity(name="home",type=["house"])
op=Operation("create",ent,from_=character.id)
tr.send(op)
#dop=Operation("delete",from_=character.id)

ent=Entity(name="vodka",type=["vodka"])
op=Operation("create",ent,from_=character.id)
vodka_no=op.no
vodka=None
tr.send(op)

time_dict={}
while 1:
    timeUsed=0.0
    if not loc:
        loc=oldLoc
        #raise TestException, "move failed"
    #loc.coordinates=[randint(0,9),randint(0,4),0.0]
    loc.coordinates=[randint(-2,2),randint(-2,2),0.0]
    moveOp[0].location=loc
    print "try to move into:\n",loc
    time_dict[world_info.operation_no]=time.time()
    tr.send(moveOp)
    if vodka:
        #print vodka
        vodka.location.parent=character.id
        tr.send(Operation("move",
                          Entity(vodka.id,location=vodka.location),
                          from_=character.id,
                          to=vodka.id))
    loc=None
    while timeUsed<10.0:
        res=tr.pop()
        if res:
            for op in res:
                key=op.refno
                prevTime=time_dict.get(key)
                if prevTime:
                    print "Reply time:",time.time()-prevTime
                    del time_dict[key]
                #who?
                if op.to==character.id:
                    if op.id=='sight':
                        op2=op[0]
                        if op2.id=='move':
                            ent=op2[0]
                            if ent.id==character.id:
                                print "moved into:\n",ent.location
                                loc=ent.location
                            else:
                                print ent.id,"has moved into:\n",ent.location
                        else:
                            print "received sight about:\n",op2
                            if op2.get_name()=="op" and op2.refno==vodka_no:
                                vodka=op2[0]
                    else:
                        print "received:\n",op
        time.sleep(0.1)
        timeUsed=timeUsed+0.1

def r():
    global res
    res=tr.pop()
    print res

def say(what, char=character):
    talkOp=Operation("talk",Entity(say=what),
                     from_=char.id)
    tr.send(talkOp)


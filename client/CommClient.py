#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 1999 Aloril (See the file COPYING for details).

from atlas import *
from world.physics.Vector3D import Vector3D
from transport import *
from CharacterClient import CharacterClient
from common.BaseEntity import BaseEntity
import time
from server.WorldTime import WorldTime,seconds2string

#initialize atlas world_info
world_info.coordinates_conversion_class=Vector3D
world_info.seconds2string=seconds2string
world_info.string2DateTime=WorldTime

class CommClient(BaseEntity):
    def __init__(self, argv):
        self.connection=self.connect(argv)
        playerName="player"+`randint(0,100)`
        self.player=self.create_player(playerName,password)
        self.character=self.create_character("character",CharacterClient)
    def connect(self, argv):
        if len(argv)>1:
            server=argv[1]
        else:
            server="localhost"
        if len(argv)>2:
            port=int(argv[2])
        else:
            port=6767
        try:
            return connect(server,port)
        except:
            print "Could not connect to",server,port
            raise
    def send(self, op):
        self.connection.send(op)
    def pop(self):
        return self.connection.pop()
    def create_player(self, name, password):
        player=Entity(name,password=password,type=["player"])
        createAccountOp=Operation("create",player)
        print "Create create op"
        res=self.connection.call(createAccountOp)
        print "Sent create op"
        op=res[0]
        print "Decoded reply to create op"
        if op.id!="info":
            print "account creation failed, trying to login:"
            player=Entity(name,password=password)
            loginOp=Operation("login",player)
            res=self.connection.call(loginOp)
            op=res[0]
            if op.id!="info":
                print "Error in login:\n",res
                sys.exit()
        ent=op[0]
        print "Player id:",name
        if hasattr(ent,"characters"):
            print "Characters:",ent.characters
        return ent
    def create_character(self, type, type_class):
        character=Entity(name=self.player.id,type=[type])
        createOp=Operation("create",character,from_=self.player.id)
        res=self.connection.call(createOp)
        op=res[0]
        if op.id!="info":
            raise ValueError, "creation of character of type '%s' failed" % type
        body=op[0]
        print "our character:\n",body
        obj=object_from_entity(type_class,body)
        obj.connection=self
        #CHEAT! something more elegant?
        obj.location.parent=obj.map.add(Entity(obj.location.parent,
                                               type=["thing"]))
        obj.location.coordinates=Vector3D(obj.location.coordinates)
        return obj
    def idle(self):
        time.sleep(0.1)
    def handle_net(self):
        while 1:
            input=self.connection.pop()
            if input:
                #print "Received:\n",input
                result=self.character.message(input)
                self.connection.send(result)
            else:
                break
    def run(self, loop=1):
        while 1:
            self.handle_net()
            self.idle()
            if not loop: break


#!/usr/bin/env python
#calling:
#irc_server:port channel nickname - atlas_server port
#all parts are optional, except you need to use "-" before atlas_server part(s)

#general arguments, like --ruleset, --security, etc... should become
#before any of above

import init
from atlas import *
import irclib
import string
import time
import sys

from ObserverClient import ObserverClient

server = "irc.worldforge.org"
port = 6667
channel = "#test"
#channel = "#cyphesis"
#channel = "#forge"
gateway_nick = "atlas_gateway2"
if channel=="#test":
    sleep_after_talk = 0
else:
    sleep_after_talk = 1

class IRC_Connection:
    def __init__(self, gateway, irc, irc_args, character):
        nickname = self.get_nick(character)
        self.gateway = gateway
        self.server, self.port, self.channel = irc_args
        print nickname, "trying to connect..."
        c = irc.server().connect(self.server, self.port,
                                 nickname, nickname, nickname)
        self.connection = c
        self.character = character
        self.joined = []
        #register
        c.add_global_handler("welcome", self.on_connect)
        c.add_global_handler("join", self.on_join)
        c.add_global_handler("disconnect", self.on_disconnect)
        if nickname == gateway_nick:
            #CHEAT!: no local handler: fix irclib.py
            self.previous_talk = None
            c.add_global_handler("pubmsg", self.on_pubmsg)
            c.add_global_handler("namreply", self.existing_names)
    def get_nick(self, character=None):
        if character:
            if character.type[0]=="creator":
                return gateway_nick
            return character.id
        return self.connection.get_nick_name()
    def talk(self, say):
        print self.get_nick(), "talk:", say
        self.connection.privmsg(self.channel, say)
    def on_pubmsg(self, connection, event):
        print self.get_nick(), "on_pubmsg!", event.__dict__, event
        nick = irclib.nick_from_nickmask(event.source())
        say = event.arguments()[0]
        if self.previous_talk == (nick,say):
            print "REPEAT"
            return
        self.previous_talk = (nick,say)
        print nick, say
        self.gateway.irc_talk(self, nick, say)
    def existing_names(self, connection, event):
        print self.get_nick(), "existing_names!", event.__dict__
        lst = event.arguments()[2]
        names=[]
        for name in string.split(lst):
            if name[0]=='@': name=name[1:]
            names.append(name)
        self.gateway.create_irc_characters(names)
    def on_connect(self, connection, event):
        print self.get_nick(), "on_connect!", event.__dict__
        print connection.join(self.channel)
    def on_join(self, connection, event):
        print self.get_nick(), "on_join!", event.__dict__
        nick = irclib.nick_from_nickmask(event.source())
        if nick==self.get_nick():
            self.joined.append(event.target())
        self.gateway.create_irc_characters([nick])
    def on_disconnect(self, connection, event):
        print self.get_nick(), "on_disconnect!", event.__dict__
        if connection.is_connected():
            connection.exit()
    def has_joined(self):
        return self.joined


class AtlasIRC_GatewayClient(ObserverClient):
    def __init__(self, argv, log_file, irc_args):
        ObserverClient.__init__(self, argv, log_file)

        self.irc_args = irc_args
        self.irc = irclib.IRC()
        self.connections = {}
        self.nick2id = {}

        map=self.character.map
        map.add_hooks.append(self.add_map)
        map.update_hooks.append(self.update_map)
        map.delete_hooks.append(self.delete_map)
        self.character.add_trigger("sound_talk",self.atlas_talk)
        self.character.send(Operation("look"))

        self.talk_queue=[]
        self.previous_talk=None
    ########## Map updates
    def add_map(self, obj):
        print "add_map:", obj, obj.type[0], obj.name
        if obj.type[0] in ["farmer", "smith", "creator"] and \
           not self.nick2id.get(obj.name):
            print "add_map: adding connection..."
            conn = IRC_Connection(self, self.irc, self.irc_args, obj)
            time_begin = time.time()
            #while time.time() - time_begin < 30.0:
            #    #print time.time() - time_begin
            #    self.idle()
            self.connections[obj.id] = conn
            self.nick2id[conn.get_nick()]=obj.id
            print "nick2id:",self.nick2id
    def update_map(self, obj):
        #print "update_map:", obj
        conn = self.connections.get(obj.id)
        if conn:
            #print "update_map: conn:", conn
            pass
        else:
            print "update_map calling add_map:", obj
            self.add_map(obj)
    def delete_map(self, obj):
        pass
    ########## Talk
    def atlas_talk(self, op, sub_op):
        id = op.from_.id
        say = sub_op[0].say
        conn = self.connections.get(id)
        print "atlas_talk:", conn, id, say
        if self.previous_talk==(id,say):
            return
        self.previous_talk=(id,say)
        if conn and conn.get_nick()!=gateway_nick:
            if conn.has_joined():
                conn.talk(say)
                time.sleep(sleep_after_talk) #avoid flooding, just in case
            else:
                self.talk_queue.append((conn,say))
    def irc_talk(self, connection, nick, say):
        id = self.nick2id.get(nick)
        if id:
            print "irc_talk:", id, connection, nick, say
            conn = self.connections.get(nick)
            if not conn: #talk originates from irc character
                print "sending talk operation..."
                self.send(Operation("talk",
                                    Entity(say=say),
                                    to=id,
                                    from_=self.character))
        print "irc_talk:", connection, nick, say
    def create_irc_characters(self, names):
        print "create_irc_characters:", names, self.nick2id
        for name in names:
            id = self.nick2id.get(name)
            if not id:
                print "creating",name
                self.nick2id[name] = name #reserver this nick
                char = self.character.make(Entity(name=name, type=["farmer"]))
                self.nick2id[name] = char.id
                print "...done:", char.id, self.nick2id
    ########## Main loop
    def idle(self):
        if self.talk_queue:
            conn,say = self.talk_queue[0]
            if conn.has_joined():
                conn.talk(say)
                time.sleep(sleep_after_talk) #avoid flooding, just in case
                del self.talk_queue[0]
        self.irc.process_once()

if __name__=="__main__":
    #print sys.argv
    if len(sys.argv)>1 and sys.argv[1]!="-":
        s = string.split(sys.argv[1], ":", 1)
        server = s[0]
        if len(s) == 2:
            try:
                port = int(s[1])
            except ValueError:
                print "Error: Erroneous port."
                sys.exit(1)
        del sys.argv[1]
    if len(sys.argv)>1 and sys.argv[1]!="-":
        channel = sys.argv[1]
        del sys.argv[1]
    if len(sys.argv)>1 and sys.argv[1]!="-":
        gateway_nick = sys.argv[1]
        del sys.argv[1]
    
    if len(sys.argv)>1 and sys.argv[1]=="-":
        del sys.argv[1]

    #print sys.argv

    print "Arguments (and defaults):", server, port, channel, gateway_nick
    gateway = AtlasIRC_GatewayClient(sys.argv,"gateway.log",
                                     (server, port, channel))
    gateway.load_default()
    gateway.run()

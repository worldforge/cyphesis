#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 1999 Aloril (See the file COPYING for details).

from atlas import *
from mind.NPCMind import NPCMind

class ExternalMind(NPCMind):
    """NPC that is running somewhere else and not inside server process
       handles PC connections too"""
    def __init__(self, connection=None, **kw):
        self.base_init(kw)
        self.connection=connection
    def message(self, msg):
        """send Atlas message to (N)PC"""
        #print "emind",self.id,"got message:\n",msg
        self.connection.send(msg)

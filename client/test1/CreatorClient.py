#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 1999 Aloril (See the file COPYING for details).

from atlas import *
from CharacterClient import CharacterClient
from common import log

class CreatorClient(CharacterClient):
    def make(self, entity):
        op=Operation("create",entity,from_=self)
        result=self.send_and_wait_reply(op)
        if result.id!="sight" or result[0].id!="create":
            log.inform("creation failed",result)
        #print "Made it!",result
        obj=self.map[result[0][0].id]
        print 'Created:',obj
        return obj


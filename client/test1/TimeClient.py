#!/usr/bin/env python
#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 2000 Aloril (See the file COPYING for details).

import sys
from time import time
from ObserverClient import ObserverClient
from editor import editor
import define_world

class TimeClient(ObserverClient):
    def load_default(self):
        begin_time = time()
        define_world.default(self.character)
        #make lumbers one more time, use this as end of time marker 
        #(all talk must be handled first)
        m=editor(self.character)
        m.make('lumber',place='forest', xyz=define_world.forest_xyz)
        end_time = time()
        print "Creation and learning time:", end_time-begin_time

if __name__=="__main__":
    print "Timing..."
    observer=TimeClient(sys.argv,"observer.log", display=0)
    observer.load_default()


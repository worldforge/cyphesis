#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 2000 Aloril (See the file COPYING for details).

#Using http://www.worldforge.org/images/maps/map_agrilan_4_us.jpg
#as background: made by Uta Szymanek <szymanek@student.uni-kl.de>
#55 pixels==20m
#0.4m/pixel
#0m, 0m: 200 pixels, 100 pixels

import init
import pdb
import sys,string,os
from random import randint
from time import time

from common import const
from tools.cmd_utilities import *
from ObserverClient import ObserverClient,sc
import media

if "acorn" in init.rulesets:
    pixel_size=0.5
    map_file_name='media/map_agrilan_5_us.jpg'
else:
    pixel_size=0.4
    map_file_name='media/map_agrilan_4_us.jpg'

def test_file_existence(file_name):
    try:
        os.stat(file_name)
    except os.error:
        print file_name,"not found! Aborting..."
        sys.exit(1)

known_types = media.media_order_value.keys()

class BackgroundMap:
    def __init__(self, createImg):
        test_file_existence(map_file_name)
        self.map_image = createImg(map_file_name)
        self.sizex=self.map_image.GetWidth()
        self.sizey=self.map_image.GetHeight()
        self.xcenter=200
        if "acorn" in init.rulesets:
            self.ycenter=200
        else:
            self.ycenter=100
        self.xmin=-self.xcenter*pixel_size
        self.xmax=(self.sizex-self.xcenter)*pixel_size
        self.ymin=-self.ycenter*pixel_size
        self.ymax=(self.sizey-self.ycenter)*pixel_size
    def __getattr__(self, name):
        """try first self.map_image, then normal attribute search"""
        return getattr(self.__dict__['map_image'], name)
    def scx(self, x): 
        return sc(x, self.xmin, self.xmax, self.sizex)
    def scy(self, y):
        return sc(-y, self.ymin, self.ymax, self.sizey)


        
class GraphicalClient(ObserverClient):
    def __init__(self, argv, log_file, canvas):
        ObserverClient.__init__(self, argv, log_file)
        self.canvas=canvas
        map=self.character.map
        map.add_hooks.append(self.add_map)
        map.update_hooks.append(self.update_map)
        map.delete_hooks.append(self.delete_map)
    ########## Map updates
    def add_map(self, obj):
        self.canvas.add_object(obj)
    def update_map(self, obj):
        self.canvas.update_object(obj)
    def delete_map(self, obj):
        self.canvas.delete_object(obj)
    def idle(self):
        self.time=str(self.character.time)
        self.canvas.SetStatusText(self.time)


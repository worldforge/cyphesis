#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 1999 Aloril (See the file COPYING for details).

from atlas import *

from world.objects.animals.Animal import Animal
from mind.WolfMind import WolfMind
from common.misc import set_kw

class Wolf(Animal):
    def __init__(self, **kw):
        self.base_init(kw)
        set_kw(self,kw,"weight",20.0)
        self.maxweight=30.0
    def setup_operation(self, op):
        """do once first after character creation"""
        if hasattr(op,"sub_to"): return None #meant for mind
        self.mind=WolfMind(id=self.id, body=self)
        opMindSetup=Operation("setup",to=self,sub_to=self.mind)
        return opMindSetup

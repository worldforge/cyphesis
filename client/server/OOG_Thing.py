#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 1999 Aloril (See the file COPYING for details).

from common.BaseEntity import BaseEntity

class OOG_Thing(BaseEntity):
    """Base entity for all Out Of Game entities, like Server"""
    def __init__(self, **kw):
        self.base_init(kw)
    def undefined_operation(self, op):
        """don't ignore undefined operations"""
        return self.error(op,"Unknown operation in "+`self`)

#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 1999 Aloril (See the file COPYING for details).

from atlas import *
from Account import Account
import string,traceback

class File:
    def __init__(self):
        self.str=""
    def write(self, str):
        self.str=self.str+str

class Admin(Account):
    """admin accounts"""
    def save_operation (self, op):
        filename = op[0].id
        if self.server.id_dict.has_key(op.to.id):
            self.server.id_dict[op.to.id].save(filename)
    def load_operation (self, op):
        filename = op[0].id
        if self.server.id_dict.has_key(op.to.id):
            self.server.id_dict[op.to.id].load(filename)
    def code_operation(self, op):
        #CHEAT!: move to Player.py and check for privileged admin account
        #or maybe make it part of creator class
        ent=op[0]
        try:
            if ent.code_type=='eval':
                res=`self.server.world.eval_code(ent.code)`
            else:
                self.server.world.execute_code(ent.code)
                res=''
        except:
            f=File()
            traceback.print_exc(None,f)
            res=f.str
        res=string.replace(res,"\n","[newline]")
        return Operation("info",Entity(code=ent.code,result=res),refno=op.no)
    def character_error(self, op, ent):
        """admin can create anything"""
        pass

#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 1999 Aloril (See the file COPYING for details).

import const

debug_fp=None
def debug(level, msg, op=None):
    """used for debugging"""
    if const.debug_level>=level:
        if debug_fp:
            debug_fp.write(msg+"\n")
            if op!=None:
                debug_fp.write(str(op)+"\n")
            debug_fp.flush()
        else:
            print msg
            if op!=None:
                print op

inform_fp=None
def inform(msg,op):
    """place to handle informative message logging etc..:
       used like: log.inform(op,"player account creation OK: "+ent.id)"""
    if const.debug_level:
        if inform_fp:
            inform_fp.write(msg+"\n"+str(op)+"\n")
            inform_fp.flush()
        else:
            print msg

thinking_fp=None
def thinking(msg):
    if const.debug_thinking:
        if thinking_fp:
            thinking_fp.write(msg+"\n")
            thinking_fp.flush()
        else:
            print msg

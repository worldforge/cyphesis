#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 1999 Aloril (See the file COPYING for details).

#load first rule specific consts
try:
    from rule_const import *
except ImportError:
    #no rulespecific changes/additions
    pass

def set(name,value):
    """create variable if it doesn't already exist"""
    if globals().has_key(name): return
    globals()[name]=value

#0: no output
#1: only basic inform messages
#2: more info about what is happening
#3: all debug messages
set("debug_level", 1)
#0: no output
#1: print thinking
set("debug_thinking", 0)

#everything
set("fzero", 1e-13) #float zero

#how many times as fast as real time
set("time_multiplier", 1000.0)
#how many times objects asses their situation
set("basic_tick", time_multiplier*3.0) #or in Real time how many seconds between ticks
set("base_velocity_coefficient", 5.0)
set("base_velocity", base_velocity_coefficient/time_multiplier)
#how mny seconds in day
set("day_in_seconds", 3600*24.0)


#ranges
set("sight_range", 100.0)
set("hearing_range", 10.0)
set("collision_range", 1.0)
set("enable_ranges", 0)
set("world_range", 100.0)

#flag to indicate that the main server is running in python
set("server_python", 1)

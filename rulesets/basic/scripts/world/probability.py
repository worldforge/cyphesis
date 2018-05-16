#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 1999 Aloril (See the file COPYING for details).

from common import log,const

import random

fertility_age=const.basic_tick*30

#probablity that fire happens in basic_tick
fire_probability=0.005

def does_it_happen(prob,tick_length=const.basic_tick):
    """calculate random number and see whether  """
    #return random.random()<prob*tick_length/const.day_in_seconds
    now=prob*tick_length/const.basic_tick
    rand=random.random()
    log.debug(3,"does_it_happen? "+str(rand)+" "+str(now))
    return rand<now

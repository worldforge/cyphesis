#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 2008 Alistair Riddoch (See the file COPYING for details).

import time
from atlas import *
from cyphesis.editor import create_editor
from math import *
from random import *


def default(host='', account='', password='', **args):
    # Spray the world with rubbish

    m=create_editor(host, account, password)

    for i in range(10000):
        m.make('oak', pos=(uniform(-500, 300), uniform(-500, 300), 0))
        m.delete(m.look_for(type='oak').id)
        time.sleep(0.1)

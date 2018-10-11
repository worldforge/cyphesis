# -*- coding: utf-8 -*-
# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 2013 Erik Ogenvik (See the file COPYING for details).

from atlas import *
from random import *
from mind.panlingua import interlinguish

il = interlinguish
from cyphesis import probability
from cyphesis.editor import editor, create_editor
from physics import Quaternion
from physics import Vector3D
from math import *

import server


# observer calls this
def default(host='', account='', password='', **args):
    print("You must specify a command. The commands available are 'suspend' and 'resume'.")


# Suspends the world, allowing authors to alter it while not worrying about entities moving or changing.
def suspend(host='', account='', password='', **args):
    m = create_editor(host, account, password)

    world = m.look()
    m.set("0", suspended=1)


# Resumes a previously suspended world.
def resume(host='', account='', password='', **args):
    m = create_editor(host, account, password)

    world = m.look()
    m.set("0", suspended=0)

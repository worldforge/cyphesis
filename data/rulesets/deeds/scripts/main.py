# -*- coding: utf-8 -*-
# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 2013 Erik Ogenvik (See the file COPYING for details).

from atlas import *

from mind.panlingua import interlinguish

il = interlinguish
from cyphesis.editor import create_editor


# observer calls this
def default(host='', account='', password='', **args):
    print("You must specify a command. The commands available are 'suspend' and 'resume'.")


# Suspends the world, allowing authors to alter it while not worrying about entities moving or changing.
def suspend(client):
    m = create_editor(client)

    m.look()
    m.set("0", suspended=1)


# Resumes a previously suspended world.
def resume(client):
    m = create_editor(client)

    m.look()
    m.set("0", suspended=0)

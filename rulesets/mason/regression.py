#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 2009 Alistair Riddoch

from atlas import *
from cyphesis.editor import editor
from types import StringType

def default(mapeditor):
    m = editor(mapeditor)

    world=m.look()

    # _test_task(m, 'cultivate', 'acorn', 'trowel', 'sow')
    _test_task(m, 'delve', world, 'pickaxe', 'cut')
    _test_task(m, 'dig', world, 'shovel', 'cut')
    # _test_task(m, 'fishing', 'ocean', 'fishingrod', 'sow')
    _test_task(m, 'logging', 'oak', 'axe', 'cut')
    _test_task(m, 'raise', 'stake', 'pole', 'lever')
    _test_task(m, 'reap', world, 'scythe', 'cut')
    _test_task(m, 'ram', 'stake', 'hammer', 'strike')

def _test_task(m, task, target, tool, op, avatar = 'settler'):

    if type(avatar)== StringType:
        c = _create_character(m, avatar, (10, 5, 0))
    else:
        c = avatar

    if type(tool)==StringType:
        t = m.make(tool, pos=(0,0,0), parent=c.id)
    else:
        t = tool

    if type(target)==StringType:
        o = m.make(target, pos=(12, 5, 0))
    else:
        o = target

    m.m.send(Operation('wield', Entity(t.id), to=c))
    m.m.send(Operation('use', Operation(op, Entity(o.id)), to=c))

    c = m.look(c.id)

    if not hasattr(c, 'tasks') or len(c.tasks) < 1:
        raise AssertionError, 'Task \'%s\' failed to start' % task

    if c.tasks[0].name != task:
        raise AssertionError, 'Task \'%s\' started ' \
                              'instead of expected \"%s\"' % (c.tasks[0].name,
                                                              task)


def _create_character(m, type, pos):
    return m.make(type, pos=pos)

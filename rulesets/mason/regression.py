#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 2009 Alistair Riddoch

from atlas import *
from cyphesis.editor import create_editor
from types import StringType

ALL_CLASSES=["acorn", "annelid", "apple", "appletree", "area", "arm", "armory", "arrow", "axe", "barrel", "birch", "block_house", "blueprint", "board", "boat", "boots", "bottle", "boulder", "boundary", "bow", "bowl", "bucksaw", "bunny", "butcher_house", "campfire", "carrot", "castle_foundation", "castle_house", "castle_outer_wall", "chicken", "cleaver", "cloak", "coin", "construction", "cow", "crab", "curtain_wall", "deed", "deer", "dog", "don_jon", "door", "feature", "fern", "fir", "fircone", "fire", "fish", "fishingrod", "float", "flower", "forest", "gallows", "garment", "gateway", "goblin", "grass", "gravestone", "hall", "ham", "hammer", "hat", "hook", "horse", "house", "inn", "jetty", "keep", "knife", "larva", "leaf", "loaf", "longtable", "lumber", "lych", "maggot", "marshall", "material", "mausoleum", "mercenary", "merchant", "mobile", "mushroom", "oak", "ocean", "path", "pelvis", "pickaxe", "pig", "pile", "ploughed_field", "pole", "poplar", "ribcage", "rope", "scythe", "seed", "settler", "shin", "shirt", "shovel", "sieve", "skeleton", "skull", "spider", "squirrel", "stake", "stall", "statue", "stone", "stonehouse", "structure", "sty", "sword", "tent", "theodolite", "thigh", "tinderbox", "torch", "tower", "tree", "trousers", "trowel", "tuber", "turnip", "twobyfour", "venison", "wall", "weather", "willow", "wolf", "wood"]

class RegressionTester:
    def __init__(self, editor, sx = 0, sy = 0, width = 64):
        self.editor = editor
        self.x = sx
        self.y = sy
        self.width = width
    def get_pos(self):
        "Get the next position on a spaced out grid"
        pos = (self.x, self.y, 0)
        self.x += 2
        if self.x > self.width:
            self.x = 0
            self.y += 2
        return pos

    def create_all(self, types):
        "Create an instance of every type given"
        for type in types:
            self.editor.make(type, pos=self.get_pos())

    def create_character(self, type, pos):
        "Create a character entity"
        return self.editor.make(type, pos=pos)

    def test_task(self, task, target, tool, op, avatar = 'settler'):
        "Test activating a task using a tool on a target"
        if type(avatar)== StringType:
            c = self.create_character(avatar, self.get_pos())
        else:
            c = avatar

        if type(tool)==StringType:
            t = self.editor.make(tool, pos=(0,0,0), parent=c.id)
        else:
            t = tool

        if type(target)==StringType:
            o = self.editor.make(target, pos=self.get_pos())
        else:
            o = target

        self.editor.m.send(Operation('wield', Entity(t.id), to=c))
        self.editor.m.send(Operation('use', Operation(op, Entity(o.id)), to=c))

        c = self.editor.look(c.id)

        if not hasattr(c, 'tasks') or len(c.tasks) < 1:
            raise AssertionError, 'Task \'%s\' failed to start' % task

        if c.tasks[0].name != task:
            raise AssertionError, 'Task \'%s\' started instead of ' \
                                  'expected \"%s\"' % (c.tasks[0].name, task)



def default(host='', account='', password='', **args):
    m = create_editor(host, account, password)

    world=m.look()

    p=RegressionTester(m)

    p.create_all(ALL_CLASSES)

    # p.test_task('cultivate', 'acorn', 'trowel', 'sow')
    p.test_task('delve', world, 'pickaxe', 'cut')
    p.test_task('dig', world, 'shovel', 'cut')
    # p.test_task(m, 'fishing', 'ocean', 'fishingrod', 'sow')
    p.test_task('logging', 'oak', 'axe', 'cut')
    p.test_task('raise', 'stake', 'pole', 'lever')
    p.test_task('reap', world, 'scythe', 'cut')
    p.test_task('ram', 'stake', 'hammer', 'strike')


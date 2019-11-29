# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 2006 Al Riddoch (See the file COPYING for details).


class Statistics(object):
    """A very simple Statistics example."""

    def __init__(self, entity=None):
        pass

    # Set it up or sumink

    def calc_attack(self):
        # print "Request for attack"
        return 1

    def calc_defence(self):
        # print "Request for defence"
        return 1

    def calc_strength(self):
        # print "Request for strength"
        if hasattr(self, 'character') and hasattr(self.character, 'mass'):
            return self.character.mass
        else:
            return 0

    def set_strength(self, val):
        pass

    # print "Setting strength to %d" % val

    attack = property(calc_attack)
    defence = property(calc_defence)
    strength = property(calc_strength, set_strength)

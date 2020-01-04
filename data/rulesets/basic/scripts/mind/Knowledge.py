# This file is distributed under the terms of the GNU General Public license.
# Copyright (C) 1999 Aloril (See the file COPYING for details).


class Knowledge:
    def __init__(self):
        self.knowings = {}

    def add(self, what, key, value):
        if what not in self.knowings:
            self.knowings[what] = {}
        self.knowings[what][key] = value

    def remove(self, what, key):
        if what in self.knowings:
            del self.knowings[what][key]

    def get(self, what, key=None):
        if not key:
            return self.knowings.get(what, {})
        else:
            if what not in self.knowings:
                return None
            # Return None if value does not exist.
            return self.knowings[what].get(key)

    def __str__(self):
        s = "<know: " + str(self.knowings)
        return s + ">\n"

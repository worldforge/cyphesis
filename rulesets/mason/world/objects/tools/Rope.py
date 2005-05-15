#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 2005 Al Riddoch (See the file COPYING for details).

from world.objects.Thing import Thing

class Rope(Thing):
    def attach_operation(self, op): pass
        # Set out attribute to be attached to the other thing
    def pull_operation(self, op): pass
        # Move whatever we are attached to.
        # Lift? Drag? Rotate?

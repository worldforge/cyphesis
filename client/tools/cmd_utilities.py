#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 1999 Aloril (See the file COPYING for details).
"""command line utilities"""

from pprint import pprint

def deb(): import pdb; pdb.pm()

#def pr(arg): print arg
pr=pprint


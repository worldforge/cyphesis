import sys,string
import getopt

#modules
try:
    from avl_tree import avl_tree
except ImportError:
    print "avl_tree is not in PYTHONPATH or in any other standard place"
    print """Adding "../modules" to sys.path"""
    sys.path.append("../modules")
    try:
        from avl_tree import avl_tree
        print "Found avl_tree from ../modules !"
    except ImportError:
        print "avl_tree not found, see INSTALL for more info"
        sys.exit()

from common import ruleset_import_hooks

#print "CONFIG:",sys.argv

optlist, args = getopt.getopt(sys.argv[1:], '', ['ruleset=','security','robust','nodisplay'])
sys.argv=[sys.argv[0]]+args

#nodisplay for ObserverClient.py
display = ('--nodisplay','') not in optlist

#security
security_flag= ('--security','') in optlist
if security_flag:
    print "Security on!"

#robust
robust_flag= ('--robust','') in optlist
if robust_flag:
    print "Robust behavior on!"

#rulesets
rulesets=[]
for opt in optlist:
    if opt[0]=="--ruleset":
        rulesets.append(opt[1])
        print "Using ruleset",opt[1]

rulesets.append("basic")
for ruleset in rulesets:
    sys.path.append("../rulesets/"+ruleset)
ruleset_import_hooks.install(rulesets)

#atlas.py:
try:
    from atlas import *
except ImportError:
    print "atlas is not in PYTHONPATH, "
    print """Adding "../../atlas/libatlas/libAtlasPy" to sys.path"""
    sys.path.append("../../atlas/libatlas/libAtlasPy")
    try:
        from atlas import *
        print "Found atlas from ../../atlas/libatlas/libAtlasPy !"
    except ImportError:
        print """Not found, retrying with "../../../protocols/atlas/libatlas/libAtlasPy" instead"""
        sys.path[-1]="../../../protocols/atlas/libatlas/libAtlasPy"
        try:
            from atlas import *
            print "Found atlas from ../../../protocols/atlas/libatlas/libAtlasPy !"
        except ImportError:
            print "I didn't found atlas.py, "
            print "maybe you should add it yourself to PYTHONPATH?"
            print "(see INSTALL for more info)"
            sys.exit()


#print sys.path

#print "->CONFIG:",sys.argv

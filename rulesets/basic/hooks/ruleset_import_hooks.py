import ihooks
#this is needed because python v1.5.1 doesn't include enough recent ihooks
#ihooks = module_ihooks

def debug(str):
    fp=open("import.log","a")
    fp.write(str)
    fp.close()

class RulesetModuleLoader(ihooks.ModuleLoader):
    """now rulesets can have sub modules
       (directories) and it searchs modules in given order: for example
       first from acorn and then basic even when they are at
       world.objects.animals modules (with standard python importing
       mechanism search works only for top level modules and when
       world.objects resides at both rulesets/acorn and
       rulesets/basic: sub modules are only searched where
       world.objects was found)
    """
    def __init__(self, rulesets):
        ihooks.ModuleLoader.__init__(self)
        self.rulesets = rulesets
        self.ruleset_paths = []
        for ruleset in rulesets:
            self.ruleset_paths.append("PREFIX/rulesets/"+ruleset)
        #print self.ruleset_paths
    def ihooks_find_module_in_dir(self, name, dir, allow_packages=1):
        return ihooks.ModuleLoader.find_module_in_dir(self, name, dir,
                                                      allow_packages)
    def find_module_in_dir(self, name, dir, allow_packages=1):
        """search 'duplicate' main modules """
        #debug("!!!!! %s %s %s\n" % (name, dir, allow_packages))
        res = self.ihooks_find_module_in_dir(name, dir, allow_packages)
        #debug("-> %s\n" % (res,))
        if not res and dir and allow_packages:
            for i in range(0,len(self.ruleset_paths)-1):
                ruleset_path = self.ruleset_paths[i]
                if dir[:len(ruleset_path)]==ruleset_path:
                    new_dir = self.ruleset_paths[i+1] + dir[len(ruleset_path):]
                    #debug("RETRYING with %s\n" % (new_dir,))
                    return self.find_module_in_dir(name, new_dir, allow_packages)
        return res

def install(rulesets):
    ihooks.install(ihooks.ModuleImporter(RulesetModuleLoader(rulesets)))

from atlas import *

from world.objects.plants.seeds.Seed import Seed

class Acorn(Seed):
    """An acorn which germinates."""
    def germinate_operation(self, op):
        set_ent=Entity(self.id,status=-1)
        create_ent=Entity(name='oak',parent='oak',location=Location(self.location.parent, self.location.position),mass=self.props.mass,scale=[0.1])
        res = Operation("set",set_ent,to=self)
        res = res + Operation("create",create_ent,to=self)
        return res

#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 2012 Anthony Pesce <timetopat@gmail.com> (See the file COPYING for details).

from atlas import *
from physics import *
from physics import Quaternion
from physics import Point3D
from physics import Vector3D

import server
import weakref

class Furnishings(server.Task):
    """A task for creating a Wooden structures such as A Frames with lumber and rope but for now a hammer""" 

    materials = "lumber"
    def furnishing_operation(self, op):
        """ Op handler for Furnishings op which activates this task """
        
        if len(op) < 1:
            sys.stderr.write("Furnishings  task has no target in op")

        self.target = server.world.get_object_ref(op[0].id)
        self.tool = op.to

        self.pos = Point3D(op[0].pos)
    def info_operation(self,op):
        print "Furnishings info"
        item = server.world.get_object_ref(op[0].id)
        chunk_loc = Location(item())
        chunk_loc.coordinates =Point3D([0,0,0])
        res=Oplist()
        target=self.target()
        raw_materials=[]
        raw_materials1=[]#holders campfire
        lcount=0
        wcount=0
        bcount=0
        ccount=0
        for item in self.character.contains:
            if item.type[0] == "lumber":
                raw_materials.append(item)
                lcount = lcount + 1
            if item.type[0] == "wood":
                raw_materials.append(item)
                wcount = wcount + 1
            if item.type[0] == "campfire":
                raw_materials1.append(item)
                ccount = ccount + 1
            if item.type[0] == "boulder":
                raw_materials.append(item)
                bcount = bcount + 1
            if lcount == 1 and wcount==3 :
                print "Enough material for table"
                break
            if lcount == 4 and wcount==2 :
                print "Enough material for chair"
                break
            if lcount == 0 and wcount==5 :
                print "Enough material for floor"
                break
            if ccount == 1 and bcount==4 :
                print "Enough material for fireplace"
                break
        else:
            print "No materials in inventory for Furnishings 2"
            self.irrelevant()
            return


        count=lcount+wcount+bcount
        print str(count)
        print self.fname
        if self.fname=="Table":
            #Making table
            while (count > 0) : 
                tar = raw_materials.pop()
                set = Operation("set", Entity(tar.id, status = -1), to = tar)
                res.append(set)
                count = count - 1
            #create the table
            #Table base
            lbbox=[-.2,-.2,-.5,.2,.2,.5]#local bbox
            create=Operation("create", Entity(name = "lumber", type = "lumber", location = chunk_loc,bbox=lbbox,mode="fixed"), to = target)
            res.append(create)
            #create the table top
            offset=Vector3D(0,0,1)
            chunk_loc.coordinates=chunk_loc.coordinates+offset
            lbbox=[-.7,-.7,-.1,.7,.7,.1]
            create=Operation("create", Entity(name = "wood", type = "wood", location = chunk_loc,bbox=lbbox,mode="fixed"), to = target)
            res.append(create)
        if self.fname=="Chair":
            #Making chair
            print "Making Chair 1"
            while (count > 0) : 
                tar = raw_materials.pop()
                set = Operation("set", Entity(tar.id, status = -1), to = tar)
                res.append(set)
                count = count - 1
            #create the legs
            #leg 1
            chunk_loc.coordinates =Point3D([0,0,0])
            offset=Vector3D(.2,.2,0)
            chunk_loc.coordinates=chunk_loc.coordinates+offset
            create=Operation("create", Entity(name = "lumber", type = "lumber", location = chunk_loc,mode="fixed"), to = target)
            res.append(create)


            #leg 2
            chunk_loc.coordinates =Point3D([0,0,0])
            offset=Vector3D(-.2,.2,0)
            chunk_loc.coordinates=chunk_loc.coordinates+offset
            create=Operation("create", Entity(name = "lumber", type = "lumber", location = chunk_loc,mode="fixed"), to = target)
            res.append(create)

            #leg 3
            chunk_loc.coordinates =Point3D([0,0,0])
            offset=Vector3D(-.2,-.2,0)
            chunk_loc.coordinates=chunk_loc.coordinates+offset
            create=Operation("create", Entity(name = "lumber", type = "lumber", location = chunk_loc,mode="fixed"), to = target)
            res.append(create)

            #leg 4
            chunk_loc.coordinates =Point3D([0,0,0])
            offset=Vector3D(.2,-.2,0)
            chunk_loc.coordinates=chunk_loc.coordinates+offset
            create=Operation("create", Entity(name = "lumber", type = "lumber", location = chunk_loc,mode="fixed"), to = target)
            res.append(create)


            
            #create the seet
            chunk_loc.coordinates =Point3D([0,0,0])
            offset=Vector3D(0,0,.5)
            lbbox=[-.3,-.3,-.1,.3,.3,.1]
            chunk_loc.coordinates=chunk_loc.coordinates+offset
            create=Operation("create", Entity(name = "wood", type = "wood", location = chunk_loc,bbox=lbbox,mode="fixed"), to = target)
            res.append(create)

            #create the back of the seat
            chunk_loc.coordinates =Point3D([0,0,0])
            offset=Vector3D(0,.3,.75)
            lbbox=[-.3,-.1,-.4,.3,.1,.4]
            chunk_loc.coordinates=chunk_loc.coordinates+offset
            create=Operation("create", Entity(name = "wood", type = "wood", location = chunk_loc,bbox=lbbox,mode="fixed"), to = target)
            res.append(create)
            
            #create the table
        
        if self.fname=="Floor":
            #Making Floor
            while (count > 0) : 
                tar = raw_materials.pop()
                set = Operation("set", Entity(tar.id, status = -1), to = tar)
                res.append(set)
                count = count - 1
            #create the Floor, it is one large wood
            #Table base
            lbbox=[-2,-2,-1,2,2,.1]#local bbox
            create=Operation("create", Entity(name = "wood", type = "wood", location = chunk_loc,bbox=lbbox,mode="fixed"), to = target)
            res.append(create)

        if self.fname=="Fireplace":
            #Making Fireplace
            while (count > 0) : 
                tar = raw_materials.pop()
                set = Operation("set", Entity(tar.id, status = -1), to = tar)
                res.append(set)
                count = count - 1
            #create the Fireplace, it is 1 campfire and 4 boulders
            #place campfire
            #lbbox=[-2,-2,-1,2,2,.1]#local bbox
            tar=raw_materials1.pop()
            create=Operation("move", Entity(tar.id, location = chunk_loc,mode="fixed"), to = tar)
            res.append(create)
            #make floor of fireplace
            lbbox=[-2,-1,-.1,2,1,.1]#local bbox
            create=Operation("create", Entity(name = "boulder", type = "boulder", location = chunk_loc,bbox=lbbox,mode="fixed"), to = target)
            res.append(create)
            #make wall 1 of fireplace
            chunk_loc.coordinates =Point3D([0,0,0])
            offset=Vector3D(-1.6,0,0)
            lbbox=[-.1,-1,-.1,.1,1,1.5]
            chunk_loc.coordinates=chunk_loc.coordinates+offset
            create=Operation("create", Entity(name = "boulder", type = "boulder", location = chunk_loc,bbox=lbbox,mode="fixed"), to = target)
            res.append(create)
            #make wall 2 of fireplace
            chunk_loc.coordinates =Point3D([0,0,0])
            offset=Vector3D(1.6,0,0)
            lbbox=[-.1,-1,-.1,.1,1,1.5]
            chunk_loc.coordinates=chunk_loc.coordinates+offset
            create=Operation("create", Entity(name = "boulder", type = "boulder", location = chunk_loc,bbox=lbbox,mode="fixed"), to = target)
            res.append(create)
            #make back of fireplace
            chunk_loc.coordinates =Point3D([0,0,0])
            offset=Vector3D(0,-.6,0)
            lbbox=[-2,-.1,-.1,2,.1,1.5]
            chunk_loc.coordinates=chunk_loc.coordinates+offset
            create=Operation("create", Entity(name = "boulder", type = "boulder", location = chunk_loc,bbox=lbbox,mode="fixed"), to = target)
            res.append(create) 

        #aframe().bbox=[-self.globalll/2,-self.globalll/2,-self.globallh/2,self.globalll/2,self.globalll,self.globallh/2]
        #item().orientation=Quaternion(1,1,1,1)#self.character.location.orientation
        print "Rotate to characters rotation"
        self.progress =1
        self.irrelevant()
        return res
            
    def tick_operation(self, op):

        """ Op handler for regular tick op """
        target=self.target()
        if not target:
            # print "Target is no more"m
            self.irrelevant()
            return

        self.rate = 0.5 / 0.75
        self.progress += 1

        if not target:
            print "Target is no more"
            self.irrelevant()
            return

        

        if self.progress < 1:
            # print "Not done yet"
            return self.next_tick(0.75)

        self.progress = 0
       

        chunk_loc = Location(self.character.location.parent)
        chunk_loc.coordinates = self.pos
        chunk_loc.orientation=self.character.location.orientation
        res=Oplist()
        lcount=0#lumber count
        wcount=0#wood count
        ccount=0#campfire count
        bcount=0#boulder count
        self.fname=""#furnishing name
        #makes sure we have the right amount of material
        for item in self.character.contains:
            if item.type[0] == "lumber":
                lcount = lcount + 1
                #print "ADDING"
            if item.type[0] == "wood":
                wcount = wcount + 1
                #print "ADDING"
            if item.type[0] == "campfire":
                ccount = ccount + 1
                #print "ADDING"
            if item.type[0] == "boulder":
                bcount = bcount + 1
                #print "ADDING"
            if lcount == 1 and wcount==3 :
                self.fname="Table"
                break
            if lcount == 4 and wcount ==2:
                print "Making chair"
                self.fname= "Chair"
                break
            if lcount== 0 and wcount==5:
                self.fname="Floor"
                break
            if ccount== 1 and bcount==4:
                self.fname="Fireplace"
                break
        else:
            print "No materials in inventory for Furnishings 1"
            self.irrelevant()
            return
        
        bbox1=[-3,-3,-3,3,3,3]   
        create=Operation("create", Entity(name = self.fname, type = "construction", location = chunk_loc), to = target)
        create.setSerialno(0)
        res.append(create)
        res.append(self.next_tick(1.75))    
        return res

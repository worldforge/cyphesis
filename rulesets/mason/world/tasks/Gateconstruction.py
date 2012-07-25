#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 2012 Anthony Pesce <timetopat@gmail.com> (See the file COPYING for details).

from atlas import *
from physics import *
from physics import Quaternion
from physics import Point3D
from physics import Vector3D

import server
import weakref

class Gateconstruction(server.Task):
    """A task for creating a Wooden structures such as A Frames with lumber and rope but for now a hammer""" 

    materials = "lumber"
    def gates_operation(self, op):
        """ Op handler for Pioneeringconstruction op which activates this task """
        
        if len(op) < 1:
            sys.stderr.write("Pioneeringconstruction  task has no target in op")

        self.target = server.world.get_object_ref(op[0].id)
        self.tool = op.to

        self.pos = Point3D(op[0].pos)
    def info_operation(self,op):
        print "Gate info"
        gate = server.world.get_object_ref(op[0].id)
        #self.pos=aframe().pos#   Point3D(op[0].pos)       
        #print op[0].id
        self.lcount = 0#needs 1 lumber for basic gate
        self.acount=0#needs 2 a frames for basic gate
        raw_materials = []
        raw_materials1 = []
        for item in self.character.contains:
            if item.type[0] == str("construction"):
                raw_materials.append(item)
                self.acount = self.acount + 1
                print "ADDING A frames"
            if item.type[0] == str("lumber"):
                raw_materials1.append(item)
                self.lcount=self.lcount+1
                print "ADDING LUMBER"                
            if self.acount == 2 and self.lcount==1:
                print "DONE Gate"
                break
        else:
            print "No materials in inventory for Gate"
            self.irrelevant()
            return


        print str(len(raw_materials))
        print str(len(raw_materials1))
        chunk_loc = Location(gate())
        #chunk_loc = Location(self.character.location.parent)
        chunk_loc.coordinates =Point3D([0,0,0]) #self.pos
        res=Oplist()
        count1=self.lcount
        while (count1 > 0):
            tar = raw_materials1.pop()
            self.lumber_length=tar.location.bbox.far_point[2]-tar.location.bbox.near_point[2]
            offset=Vector3D(0,self.lumber_length/3,self.lumber_length*.65)
            chunk_loc.orientation=Quaternion([.707,0,0,.707])
            chunk_loc.coordinates=chunk_loc.coordinates+offset
            move1=Operation("move", Entity(tar.id,location=chunk_loc,mode="fixed"), to=tar)
            res.append(move1)
            #print str(tar.location.bbox.far_point[2]-tar.location.bbox.near_point[2])
            #print str(tar.location.bbox.far_point[1]-tar.location.bbox.near_point[1])
            #print str(tar.location.bbox.far_point[0]-tar.location.bbox.near_point[0])
            count1=count1-1

        
        count = self.acount
        chunk_loc = Location(gate())
        #chunk_loc = Location(self.character.location.parent)
        chunk_loc.coordinates =Point3D([0,0,0]) #self.pos        
        #loops through raw_materials and places 3 lumber in inventory infront of user
        offset=Vector3D(0,0,0)
        while (count > 0) : 
            tar = raw_materials.pop()
          
            if count == 2 :
                #left component
                chunk_loc.coordinates =Point3D([0,0,0])
                offset=Vector3D(0,0,self.lumber_length*.7)
                chunk_loc.orientation=Quaternion([.707,0,0,.707])
                chunk_loc.coordinates=chunk_loc.coordinates+offset
                print "LEFT"
            if count == 1 :
                #right component
                chunk_loc.coordinates =Point3D([0,0,0])
                offset=Vector3D(0,-(self.lumber_length/2),self.lumber_length*.7)
                chunk_loc.orientation=Quaternion([.707,0,0,.707])
                chunk_loc.coordinates=chunk_loc.coordinates+offset
                print "RIGHT"
                
            print "MOVING"
            move=Operation("move", Entity(tar.id,location=chunk_loc,mode="fixed"), to=tar)
            res.append(move)
            count = count - 1

        
        #gate().bbox=[-self.lumber_length/2,-self.lumber_length/2,-.1,self.lumber_length/2,self.lumber_length/2,.1] 
        print "RETURNING"    
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
            
        create=Operation("create", Entity(name = "Basic_Gate", type = "construction", location = chunk_loc), to = target)
        create.setSerialno(0)
        #print create.id
        res.append(create)
        res.append(self.next_tick(1.75))    
        #res.append(create)
        #self.progress = 1
        #self.irrelevant()
        return res

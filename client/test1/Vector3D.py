#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 1999 Kosh (See the file COPYING for details).

from types import * #for TypeXXX
from operator import * #for add, etc..
from math import *

class Vector3D:
        """Make a vector from a list or from all three values, if the lenght
        of the list is less then 3 then the rest of the values are filled 
        with zeros."""
        def __init__(self, x, y=None, z=None):
                "Check to see if x is a list"
                if type(x)==ListType:
                        #If the lenght of the list is less then 3 fill with 0"
                        if len(x)<3:
                                x=x+[0.0]*(3-len(x))
                        self.__dict__['vector']=x[:3]
                else:
                        #Make a vector with x,y,z from above"
                        self.__dict__['vector']=[x,y,z]

        def __cmp__(self, other):
                "Check if two vector are equal"
                return cmp(self.vector,other.vector)

        def __add__(self, other):
                "Add two vectors"
                return Vector3D(map(add,self.vector,other.vector))

        def __sub__(self, other):
                "Subtract two vectors"
                return Vector3D(map(sub,self.vector,other.vector))

        def __mul__(self, other):
                "Multpiply one vector by another vector or by a number"
                if type(other) == InstanceType:
                        o_vector=other.vector
                else:
                        o_vector=[other]*3
                return Vector3D(map(mul,self.vector,o_vector))

        def __div__(self, other):
                "Divide a vector by a number"
                return Vector3D(map(div,self.vector,[other]*3))

        def __getitem__(self, index):
                "Get a value from a vector given by index"
                return self.vector[index]

        def __setitem__(self, index,value):  
                "Set a value of a vector given by index"
                self.vector[index]=value

        def __len__(self):
                "Define the length of a vector"
                return 3

        def __getslice__(self, i, j):
                "Get a slice of a vector"
                return self.vector[i:j]

        def __getattr__(self, name):
                "Get a value from a vector by name"
                if name=="x": return self.vector[0]
                if name=="y": return self.vector[1]
                if name=="z": return self.vector[2]
                raise AttributeError, name

        def __setattr__(self, name, value):
                "Set the value of a vector by name"
                if name=="x": self.vector[0]=value
                if name=="y": self.vector[1]=value
                if name=="z": self.vector[2]=value
                self.__dict__[name]=value

        def __repr__(self):
                "String representation of a vector"
                return 'Vector3D' + `(self.x, self.y, self.z)`

        def __neg__(self):
                "The negative of a vector"
                return Vector3D(map(neg,self.vector))

        def __nonzero__(self):
                "Check for nonzero vector"
                return not (self.x==self.y==self.y==0.0)
 
        def dot(self, v):
                "Dot product of a vector"
                return self.x * v.x + self.y * v.y + self.z * v.z
  
        def cross(self, v):
                "Cross product of a vector"
                return Vector3D(self.y * v.z - self.z * v.x, self.z * v.x - self.x * v.z, self.x * v.y - self.y * v.z)
  
        def rotatey(self, angle):
                "Rotate a vector around y axis by angle in radians"	
                self.y = cos(angle) * self.y + sin(angle) * self.z
                self.z = sin(angle) * self.y + cos(angle) * self.z
                return self
               
        def rotatex(self, angle):
                "Rotate a vector around x axis by angle in radians" 
                self.z = cos(angle) * self.z + sin(angle) * self.x
                self.x = sin(angle) * self.z + cos(angle) * self.x
                return self
               
        def rotatez(self, angle):
                "Rotate a vector around z axis by angle in radians" 
                self.x = cos(angle) * self.x + sin(angle) * self.y
                self.y = sin(angle) * self.x + cos(angle) * self.y
                return self

        def angle(self, v):
                "Find the angle between two vectors"
                d = v.x * self.x + v.y * self.y + v.z * self.z
                return d / (v.mag() * u.mag())

        def unit_vector(self):
        	"return the unit vector of a vector"
        	try:
			return self/self.mag()
		except ZeroDivisionError:
			return Vector3D([0.0,0.0,0.0])
        	
        def unit_vector_to_another_vector(self, v):
        	"return the unit vector in the direction of another vector"
        	difference_vector = v - self
        	return difference_vector.unit_vector()

        def mag(self):
                "Find the magnitude of a vector"
                return sqrt(self.x**2 + self.y**2 + self.z**2)
        
        def distance(self, v):
                "Find the distance between two vectors"
                return sqrt((self.x - v.x)**2 + (self.y - v.y)**2 + (self.z - v.z)**2)

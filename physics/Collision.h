// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#ifndef PHYSICS_COLLISION_H
#define PHYSICS_COLLISION_H

#include "physics/Vector3D.h"

#include <map>

class Location;

typedef std::multimap<int, WFMath::Vector<3> > NormalSet;

////////////////////////// COLLISION //////////////////////////

// Predict collision between a point and a plane.
// Returns true if p is infront of plane before collision
bool getCollisionTime(const Vector3D & p,     // Position of point
                      const Vector3D & u,     // Velocity of point
                      // float point_time,   // Time since position set
                      const Vector3D & l,     // Position on plane
                      const Vector3D & n,     // Plane normal
                      const Vector3D & v,     // Velocity of plane
                      // float plane_time,   // Time since position set
                      float & time);         // Returned collision time

// Predict collision between two sets of meshes, defined by vertices
// and surface normals
// Returns whether the collision will occur
bool predictCollision(const CoordList & l,    // Vertices of this mesh
                      const NormalSet & ln,   // Normals of this mesh
                      const Vector3D & u,     // Velocity of this mesh
                      const CoordList & o,    // Vertices of other mesh
                      const NormalSet & on,   // Normals of other mesh
                      const Vector3D & v,     // Velocity of other mesh
                      float & time,           // Returned time to collision
                      Vector3D & normal);     // Returned collision normal

// Predict collision between 2 entity locations
// Returns whether the collision will occur
bool predictCollision(const Location & l,     // Location data of this object
                      const Location & o,     // Location data of other object
                      float & time,           // Returned time to collision
                      Vector3D & normal);     // Returned collision normal

////////////////////////// EMERGENCE //////////////////////////

// Predict collision between a point and a plane.
// Returns true if p is behind plane before collision
bool getEmergenceTime(const Vector3D & p,     // Position of point
                      const Vector3D & u,     // Velocity of point
                      // float point_time,   // Time since position set
                      const Vector3D & l,     // Position on plane
                      const Vector3D & n,     // Plane normal
                      const Vector3D & v,     // Velocity of plane
                      // float plane_time,   // Time since position set
                      float & time);         // Returned collision time

// Predict collision between two sets of meshes, defined by vertices
// and surface normals
// Returns whether the collision will occur
bool predictEmergence(const CoordList & l,    // Vertices of this mesh
                      const NormalSet & ln,   // Normals of this mesh
                      const Vector3D & u,     // Velocity of this mesh
                      const CoordList & o,    // Vertices of other mesh
                      const NormalSet & on,   // Normals of other mesh
                      const Vector3D & v,     // Velocity of other mesh
                      float & time,           // Returned time to collision
                      Vector3D & normal);     // Returned collision normal

// Predict when entity emerges from inside other entity
// Returns whether the emergence will occur
bool predictEmergence(const Location & l,     // Location data of this object
                      const Location & o,     // Location data of other object
                      float & time,          // Returned time to collision
                      Vector3D & normal);     // Returned collision normal

#endif // PHYSICS_COLLISION_H

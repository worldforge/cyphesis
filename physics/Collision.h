// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#ifndef PHYSICS_COLLISION_H
#define PHYSICS_COLLISION_H

#include "physics/Vector3D.h"

#include <map>

class Location;

typedef std::multimap<int, WFMath::Vector<3> > NormalSet;

// Predict collision between a point and a plane.
// Returns whether the point is already behind the plane
bool predictCollision(const Vector3D & p,     // Position of point
                      const Vector3D & u,     // Velocity of point
                      // double point_time,   // Time since position set
                      const Vector3D & l,     // Position on plane
                      const Vector3D & n,     // Plane normal
                      const Vector3D & v,     // Velocity of plane
                      // double plane_time,   // Time since position set
                      double & time,          // Returned collision time
                      Vector3D & normal);     // Returned collision normal


// Predict collision between two sets of meshes, defined by vertices
// and surface normals
// Returns whether the collision will occur
bool predictCollision(const CoordList & l,    // Vertices of this mesh
                      const NormalSet & ln,   // Normals of this mesh
                      const Vector3D & u,     // Velocity of this mesh
                      const CoordList & o,    // Vertices of other mesh
                      const NormalSet & on,   // Normals of other mesh
                      const Vector3D & v,     // Velocity of other mesh
                      double & time,          // Returned time to collision
                      Vector3D & normal);     // Returned collision normal


// Predict collision between 2 entity locations
// Returns whether the collision will occur
bool predictCollision(const Location & l,     // Location data of this object
                      const Location & o,     // Location data of other object
                      double & time,          // Returned time to collision
                      Vector3D & normal);     // Returned collision normal

#endif // PHYSICS_COLLISION_H

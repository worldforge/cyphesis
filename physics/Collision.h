// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#ifndef PHYSICS_COLLISION_H
#define PHYSICS_COLLISION_H

#include "physics/Vector3D.h"

#include <wfmath/axisbox.h>

#include <map>

class Location;

typedef std::multimap<int, WFMath::Vector<3> > NormalSet;

////////////////////////// COLLISION //////////////////////////

/// \brief Predict collision between a point and a plane.
///
/// @return true if p is infront of plane before collision
bool getCollisionTime(const Vector3D & p,     // Position of point
                      const Vector3D & u,     // Velocity of point
                      // float point_time,   // Time since position set
                      const Vector3D & l,     // Position on plane
                      const Vector3D & n,     // Plane normal
                      const Vector3D & v,     // Velocity of plane
                      // float plane_time,   // Time since position set
                      float & time);         // Returned collision time

/// \brief Predict collision between two sets of meshes, defined by vertices
/// and surface normals.
///
/// @return true if a collision will occur, false otherwise.
bool predictCollision(const CoordList & l,    // Vertices of this mesh
                      const NormalSet & ln,   // Normals of this mesh
                      const Vector3D & u,     // Velocity of this mesh
                      const CoordList & o,    // Vertices of other mesh
                      const NormalSet & on,   // Normals of other mesh
                      const Vector3D & v,     // Velocity of other mesh
                      float & time,           // Returned time to collision
                      Vector3D & normal);     // Returned collision normal

/// \brief Predict collision between 2 entity locations.
///
/// @return true if a collision will occur.
bool predictCollision(const Location & l,     // Location data of this object
                      const Location & o,     // Location data of other object
                      float & time,           // Returned time to collision
                      Vector3D & normal);     // Returned collision normal

////////////////////////// EMERGENCE //////////////////////////

/// \brief Predict collision between a point and a plane.
///
/// @return true if p is behind plane before collision
bool getEmergenceTime(const Vector3D & p,     // Position of point
                      const Vector3D & u,     // Velocity of point
                      // float point_time,   // Time since position set
                      const Vector3D & l,     // Position on plane
                      const Vector3D & n,     // Plane normal
                      const Vector3D & v,     // Velocity of plane
                      // float plane_time,   // Time since position set
                      float & time);         // Returned collision time

/// \brief Predict emergence of one mesh from inside another, the two meshes
/// defined by vertices and surface normals.
///
/// @return true if the collision will occur within the timeframe,
/// false otherwise.
bool predictEmergence(const CoordList & l,      // Vertices of this mesh
                      const Vector3D & u,       // Velocity of this mesh
                      const WFMath::AxisBox<3>&,// Bounding box of container
                      float & time);            // Returned time to collision
// FIXME Maybe another function like the above is required to handle if the
// container is more complex and has a set of meshes?

/// \brief Predict when entity emerges from inside other entity.
///
/// @return true if the collision will occur within the timeframe,
/// false otherwise.
bool predictEmergence(const Location & l,     // Location data of this object
                      const Location & o,     // Location data of container
                      float & time);          // Returned time to collision

#endif // PHYSICS_COLLISION_H

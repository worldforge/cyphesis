// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#include "modules/Location.h"

#include "Collision.h"


bool predictCollision(const Vector3D & p,     // Position of point
                      const Vector3D & u,     // Velocity of point
                      // double point_time,   // Time since position set
                      const Vector3D & l,     // Position on plane
                      const Vector3D & n,     // Plane normal
                      const Vector3D & v,     // Velocity of plane
                      // double plane_time,   // Time since position set
                      double & time,            // Collision time return
                      Vector3D & normal)      // Collision normal return
//
//
//                            |     \ n
//                   p ___ u  |  v __\ l
//                     \      |      /
//                      \     |     /
//                       \    |    /
//                        \   |   /
//                         \  |  /
//                          \ | /
//  _________________________\|/___________________________
//
//  The time when point hits plane is as follows:
//
//  ( (p + u * t) - (l + v * t) ) . n = 0
//
//  dot product ( . ) is x*x + y*y + z*z
//
//  (p.x + u.x * t - l.x - v.x * t) * n.x +
//  (p.y + u.y * t - l.y - v.y * t) * n.y +
//  (p.z + u.z * t - l.z - v.z * t) * n.z = 0
//
//  p.x * n.x + u.x * n.x * t - l.x * n.x - v.x * n.x * t +
//  p.y * n.y + u.y * n.y * t - l.y * n.y - v.y * n.y * t +
//  p.z * n.z + u.z * n.z * t - l.z * n.z - v.z * n.z * t = 0
//
//
// ( v.x * n.x + v.y * n.y + v.z * n.z - u.x * n.x - u.y * n.y - u.z * n.z ) * t
// = ( p.x * n.x - l.x * n.x + p.y * n.y - l.y * n.y + p.z * n.z - l.z * n.z )
//
// t =
// ( p.x * n.x - l.x * n.x + p.y * n.y - l.y * n.y + p.z * n.z - l.z * n.z ) /
// ( v.x * n.x + v.y * n.y + v.z * n.z - u.x * n.x - u.y * n.y - u.z * n.z )
//
// return value should indicate whether we are infront of or behind the
// plane. There is math in common, but I'm not sure how much it will help
//
{
    time = (  p.x() * n.x() - l.x() * n.x()
            + p.y() * n.y() - l.y() * n.y()
            + p.z() * n.z() - l.z() * n.z() ) /
           (  v.x() * n.x() + v.y() * n.y()
            + v.z() * n.z() - u.x() * n.x()
            - u.y() * n.y() - u.z() * n.z() );
    return (Dot(p - l, n) > 0.);
}

void predictEntryExit(const CoordList & c,          // Vertices of this mesh
                      const Vector3D & u,           // Velocity of this mesh
                      const CoordList & o,          // Vertices of other mesh
                      const NormalSet & n,          // Normals of other mesh
                      const Vector3D & v,           // Velocity of other mesh
                      double & latest_entry_time,   // Time last vertex enters
                      double & earliest_exit_time,  // Time last vertex leaves
                      Vector3D & normal)            // Returned collision normal
{
    // Check l vertices against o surfaces
    // FIXME AJR 2003-10-17
    // I think I have these the wrong way around. Need to check a vertex
    // against all the surfaces - not the other way around
    // Also there are two issues, not one. Vertex collision is detected
    // by the last plane a vertex moves behind. Object collision is
    // detected by the first vertex to collide with the other object.
    // Trickier than I thought.
    CoordList::const_iterator I = c.begin();
    NormalSet::const_iterator J = n.begin();
    for (; J != n.end(); ++J) {
        const Vector3D & s_pos = o[J->first];
        const Vector3D & s_norm = J->second;
        for (; I != c.end(); ++I) {
            double time;
            Vector3D entry_normal;
            if (predictCollision(*I, u, s_pos, s_norm, v, time, entry_normal)) {
                // We are in front of the plane, so time is time to hit
                if (time > 0) {
                    if (time > latest_entry_time) {
                        latest_entry_time = time;
                        normal = entry_normal;
                    }
                // or time since we exited
                } else {
                    // Irrelevant case I think
                    if (time < earliest_exit_time) {
                        earliest_exit_time = time;
                    }
                }
            } else {
                // We are behind the plane, so time is time to exit
                if (time > 0) {
                    if (time < earliest_exit_time) {
                        earliest_exit_time = time;
                    }
                // or time since we hit
                } else {
                    // Irrelevant case I think
                    if (time > latest_entry_time) {
                        latest_entry_time = time;
                    }
                }
            }
                
            // FIXME Now what?
        }
    }
}

bool predictCollision(const CoordList & l,    // Vertices of this mesh
                      const NormalSet & ln,   // Normals of this mesh
                      const Vector3D & u,     // Velocity of this mesh
                      const CoordList & o,    // Vertices of other mesh
                      const NormalSet & on,   // Normals of other mesh
                      const Vector3D & v,     // Velocity of other mesh
                      double & time,          // Returned time to collision
                      Vector3D & n)      // Returned collision normal
{
    double latest_entry_time = 0, earliest_exit_time = -0.1;
    predictEntryExit(l, u, o, on, v, latest_entry_time, earliest_exit_time, n);
    predictEntryExit(o, v, l, ln, u, latest_entry_time, earliest_exit_time, n);
    return (earliest_exit_time > latest_entry_time);
}

//
// This is the vertex layout used by the 3Dbox functions.
//
//
//                                   6
//
//
//                         7                    5
//
//
//                                   4
//
//
//
//                                   2
//
//
//                         3                    1
//          z
//          |
//       y\ | /x                     0
//         \|/


bool predictCollision(const Location & l,
                      const Location & o,
                      double & time,
                      Vector3D & normal)
{
    const WFMath::Point<3> & ln = l.m_bBox.lowCorner();
    const WFMath::Point<3> & lf = l.m_bBox.highCorner();
    const WFMath::Point<3> & on = o.m_bBox.lowCorner();
    const WFMath::Point<3> & of = o.m_bBox.highCorner();

    // Create a set of vertices representing the box corners
    CoordList lbox(8), obox(8);

    lbox[0] = WFMath::Vector<3>(ln.x(), ln.y(), ln.z());
    lbox[1] = WFMath::Vector<3>(lf.x(), ln.y(), ln.z());
    lbox[2] = WFMath::Vector<3>(lf.x(), lf.y(), ln.z());
    lbox[3] = WFMath::Vector<3>(ln.x(), lf.y(), ln.z());
    lbox[4] = WFMath::Vector<3>(ln.x(), ln.y(), lf.z());
    lbox[5] = WFMath::Vector<3>(lf.x(), ln.y(), lf.z());
    lbox[6] = WFMath::Vector<3>(lf.x(), lf.y(), lf.z());
    lbox[7] = WFMath::Vector<3>(lf.x(), ln.y(), lf.z());

    obox[0] = WFMath::Vector<3>(on.x(), on.y(), on.z());
    obox[1] = WFMath::Vector<3>(of.x(), on.y(), on.z());
    obox[2] = WFMath::Vector<3>(of.x(), of.y(), on.z());
    obox[3] = WFMath::Vector<3>(on.x(), of.y(), on.z());
    obox[4] = WFMath::Vector<3>(on.x(), on.y(), of.z());
    obox[5] = WFMath::Vector<3>(of.x(), on.y(), of.z());
    obox[6] = WFMath::Vector<3>(of.x(), of.y(), of.z());
    obox[7] = WFMath::Vector<3>(of.x(), on.y(), of.z());

    // Orient the box corners
    for(int i = 0; i < 8; ++i) {
        lbox[i].rotate(l.m_orientation);
        obox[i].rotate(o.m_orientation);
    }

    // Set up a set of surface normals, each with an assoicated corner
    NormalSet lnormals;

    lnormals.insert(std::make_pair(0, Vector3D( 0.,  0., -1.))); // Bottom face
    lnormals.insert(std::make_pair(1, Vector3D( 0., -1.,  0.))); // South face
    lnormals.insert(std::make_pair(3, Vector3D(-1.,  0.,  0.))); // West face
    lnormals.insert(std::make_pair(2, Vector3D( 1.,  0.,  0.))); // East face
    lnormals.insert(std::make_pair(6, Vector3D( 0.,  1.,  0.))); // North face
    lnormals.insert(std::make_pair(4, Vector3D( 0.,  0.,  1.))); // Top face

    NormalSet onormals;

    // Orient the surface normals
    for(NormalSet::iterator I = lnormals.begin(); I != lnormals.end(); ++I) {
        I->second.rotate(l.m_orientation);
    }

    for(NormalSet::iterator I = onormals.begin(); I != onormals.end(); ++I) {
        I->second.rotate(o.m_orientation);
    }

    // Predict the collision using the generic mesh function
    return predictCollision(lbox, lnormals, l.m_velocity,
                            obox, onormals, o.m_velocity,
                            time, normal);
}

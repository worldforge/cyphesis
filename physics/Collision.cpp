// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#include "Collision.h"

#include "modules/Location.h"

#include "common/debug.h"

static const bool debug_flag = false;

bool getCollisionTime(const Vector3D & p,     // Position of point
                      const Vector3D & u,     // Velocity of point
                      // double point_time,   // Time since position set
                      const Vector3D & l,     // Position on plane
                      const Vector3D & n,     // Plane normal
                      const Vector3D & v,     // Velocity of plane
                      // double plane_time,   // Time since position set
                      double & time,          // Collision time return
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
// return value should indicate whether the point was infront of the plane
// before the collision.
//
{
    time = (  p.x() * n.x() - l.x() * n.x()
            + p.y() * n.y() - l.y() * n.y()
            + p.z() * n.z() - l.z() * n.z() ) /
           (  v.x() * n.x() + v.y() * n.y()
            + v.z() * n.z() - u.x() * n.x()
            - u.y() * n.y() - u.z() * n.z() );
    bool now_infront = (Dot(p - l, n) > 0.);
    bool collided = (time < 0.);
    return ((now_infront && !collided) || (!now_infront && collided));
}

// Returns true if first_collision has been updated
static
bool predictEntryExit(const CoordList & c,          // Vertices of this mesh
                      const Vector3D & u,           // Velocity of this mesh
                      const CoordList & o,          // Vertices of other mesh
                      const NormalSet & n,          // Normals of other mesh
                      const Vector3D & v,           // Velocity of other mesh
                      double & first_collision,     // Time first vertex enters
                      Vector3D & normal)            // Returned collision normal
{
    // Check l vertices against o surfaces
    Vector3D entry_normal;
    bool ret = false;
    CoordList::const_iterator I = c.begin();
    for (; I != c.end(); ++I) { // Iterate over vertices
        debug(std::cout << "outer loop" << std::endl << std::flush;);
        double last_vertex_entry = -100, first_vertex_exit = 100, time;
        NormalSet::const_iterator J = n.begin();
        for (; J != n.end(); ++J) { // Iterate over surfaces
            const Vector3D & s_pos = o[J->first];
            const Vector3D & s_norm = J->second;
            debug(std::cout << "Testing vertex " << *I << " to surface "
                            << s_pos << ": " << s_norm;);
            if (getCollisionTime(*I, u, s_pos, s_norm, v, time, entry_normal)) {
                debug(std::cout << " Collision at " << time;);
                // We are colliding from infront
                if (time > last_vertex_entry) {
                    debug(std::cout << " new";);
                    last_vertex_entry = time;
                }
            } else {
                debug(std::cout << " Emergence at " << time;);
                // We are colliding fron behind
                if (time < first_vertex_exit) {
                    first_vertex_exit = time;
                }
            }
            debug(std::cout << std::endl << std::flush;);
        }
        if ((last_vertex_entry < first_vertex_exit) &&
            (last_vertex_entry < first_collision)) {
            first_collision = last_vertex_entry;
            ret = true;
            // FIXME Normal! Need to get it from above
            // Also tricker because normal is 
        }
    }
    return ret;
}

bool predictCollision(const CoordList & l,    // Vertices of this mesh
                      const NormalSet & ln,   // Normals of this mesh
                      const Vector3D & u,     // Velocity of this mesh
                      const CoordList & o,    // Vertices of other mesh
                      const NormalSet & on,   // Normals of other mesh
                      const Vector3D & v,     // Velocity of other mesh
                      double & time,          // Returned time to collision
                      Vector3D & n)           // Returned collision normal
{
    debug(std::cout << "l with o normals" << std::endl << std::flush; );
    bool lo = predictEntryExit(l, u, o, on, v, time, n);
    debug(std::cout << "o with l normals" << std::endl << std::flush; );
    bool ol = predictEntryExit(o, v, l, ln, u, time, n);
    if (ol) {
        n = -n;
    }
    return (lo || ol);
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


bool predictCollision(const Location & l,  // This location
                      const Location & o,  // Other location
                      double & time,       // Returned time to collision
                      Vector3D & normal)   // Returned normal acting on l
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

// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#include "Collision.h"

#include "modules/Location.h"

#include "common/debug.h"

#include <iostream>

static const bool debug_flag = false;

////////////////////////// COLLISION //////////////////////////

bool getCollisionTime(const Vector3D & p,     // Position of point
                      const Vector3D & u,     // Velocity of point
                      // float point_time,   // Time since position set
                      const Vector3D & l,     // Position on plane
                      const Vector3D & n,     // Plane normal
                      const Vector3D & v,     // Velocity of plane
                      // float plane_time,   // Time since position set
                      float & time)          // Collision time return
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
    // Set now_infront to true if point is currently in front of the plane
    bool now_infront = (Dot(p - l, n) > 0.);
    // Set collided to true if the collision has alread occured
    bool collided = (time < 0.);
    // Return true if the collision direction is from infront,
    // whether it has already happened on not
    return ((now_infront && !collided) || (!now_infront && collided));
}

// Returns true if first_collision has been updated
static
bool predictEntryExit(const CoordList & c,          // Vertices of this mesh
                      const Vector3D & u,           // Velocity of this mesh
                      const CoordList & o,          // Vertices of other mesh
                      const NormalSet & n,          // Normals of other mesh
                      const Vector3D & v,           // Velocity of other mesh
                      float & first_collision,     // Time first vertex enters
                      Vector3D & normal)            // Returned collision normal
{
    // Check l vertices against o surfaces
    Vector3D entry_normal;
    bool ret = false, already = false;
    CoordList::const_iterator I = c.begin();
    for (; I != c.end(); ++I) { // Iterate over vertices
        debug(std::cout << "outer loop" << std::endl << std::flush;);
        float last_vertex_entry = -100, first_vertex_exit = 100, time;
        NormalSet::const_iterator J = n.begin();
        for (; J != n.end(); ++J) { // Iterate over surfaces
            const Vector3D & s_pos = o[J->first];
            const Vector3D & s_norm = J->second;
            debug(std::cout << "Testing vertex " << *I << " to surface "
                            << s_pos << ": " << s_norm;);
            if (getCollisionTime(*I, u, s_pos, s_norm, v, time)) {
                debug(std::cout << " Collision at " << time;);
                // We are colliding from infront ie entering
                if (time > last_vertex_entry) {
                    debug(std::cout << " new";);
                    last_vertex_entry = time;
                    entry_normal = s_norm;
                }
            } else {
                debug(std::cout << " Emergence at " << time;);
                // We are colliding fron behind ie exitint
                if (time < first_vertex_exit) {
                    first_vertex_exit = time;
                }
            }
            debug(std::cout << std::endl << std::flush;);
        }
        debug(std::cout << last_vertex_entry << ":"
                        << first_vertex_exit << ":"
                        << first_collision << std::endl << std::flush;);
        if ((last_vertex_entry < first_vertex_exit) &&
            (last_vertex_entry < first_collision)) {
            if (last_vertex_entry >= 0.) {
                first_collision = last_vertex_entry;
                debug(std::cout << "hit" << std::endl << std::flush;);
                ret = true;
                normal = entry_normal;
            } else {
                // Indicate that one or more vertices is already in collision
                already = true;
            }
        }
    }
    // If one or more vertices are already in collision, and some are yet to
    // collide, then we consider that the collision is immediate.
    if (ret && already) {
        first_collision = 0.f;
        // It is possible that we need to modify the normal here, perhaps
        // to cancel out velocity completely.
    }
    return ret;
}

bool predictCollision(const CoordList & l,    // Vertices of this mesh
                      const NormalSet & ln,   // Normals of this mesh
                      const Vector3D & u,     // Velocity of this mesh
                      const CoordList & o,    // Vertices of other mesh
                      const NormalSet & on,   // Normals of other mesh
                      const Vector3D & v,     // Velocity of other mesh
                      float & time,           // Returned time to collision
                      Vector3D & n)           // Returned collision normal
{
    debug(std::cout << u << ", " << v << std::endl << std::flush; );
    debug(std::cout << "l with o normals" << std::endl << std::flush; );
    bool lo = predictEntryExit(l, u, o, on, v, time, n);
    debug(std::cout << "o with l normals" << std::endl << std::flush; );
    bool ol = predictEntryExit(o, v, l, ln, u, time, n);
    if (ol) {
        // If ol is true then the collision is in the opposite direction,
        // and the normal reaction needs to be reversed.
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
                      float & time,       // Returned time to collision
                      Vector3D & normal)   // Returned normal acting on l
// Predict collision between 2 entity locations
// Returns whether the collision will occur
{
    // FIXME Handle entities which have no box - just one vertex I think
    // FIXME THe mesh conversion process below should probably be eliminated
    // by generating the data when bBox or orienation are changed.
    // This would also allow us to have other mesh shapes

    assert(l.m_bBox.isValid());
    assert(o.m_bBox.isValid());

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
    lbox[7] = WFMath::Vector<3>(ln.x(), lf.y(), lf.z());

    obox[0] = WFMath::Vector<3>(on.x(), on.y(), on.z());
    obox[1] = WFMath::Vector<3>(of.x(), on.y(), on.z());
    obox[2] = WFMath::Vector<3>(of.x(), of.y(), on.z());
    obox[3] = WFMath::Vector<3>(on.x(), of.y(), on.z());
    obox[4] = WFMath::Vector<3>(on.x(), on.y(), of.z());
    obox[5] = WFMath::Vector<3>(of.x(), on.y(), of.z());
    obox[6] = WFMath::Vector<3>(of.x(), of.y(), of.z());
    obox[7] = WFMath::Vector<3>(on.x(), of.y(), of.z());

    // Set up a set of surface normals, each with an assoicated corner
    NormalSet lnormals;

    lnormals.insert(std::make_pair(0, Vector3D( 0.,  0., -1.))); // Bottom face
    lnormals.insert(std::make_pair(1, Vector3D( 0., -1.,  0.))); // South face
    lnormals.insert(std::make_pair(3, Vector3D(-1.,  0.,  0.))); // West face
    lnormals.insert(std::make_pair(2, Vector3D( 1.,  0.,  0.))); // East face
    lnormals.insert(std::make_pair(6, Vector3D( 0.,  1.,  0.))); // North face
    lnormals.insert(std::make_pair(4, Vector3D( 0.,  0.,  1.))); // Top face

    NormalSet onormals(lnormals);

    // Orient the surface normals and box corners
    if (l.m_orientation.isValid()) {
        NormalSet::iterator I = lnormals.begin();
        for(; I != lnormals.end(); ++I) {
            I->second.rotate(l.m_orientation);
        }
        for(int i = 0; i < 8; ++i) {
            lbox[i].rotate(l.m_orientation);
        }
    }

    if (o.m_orientation.isValid()) {
        NormalSet::iterator I = onormals.begin();
        for(; I != onormals.end(); ++I) {
            I->second.rotate(o.m_orientation);
        }
        for(int i = 0; i < 8; ++i) {
            obox[i].rotate(o.m_orientation);
        }
    }

    // Translate the box corners
    for(int i = 0; i < 8; ++i) {
        lbox[i] += l.m_pos;
        obox[i] += o.m_pos;
    }

    assert(l.m_velocity.isValid());
    Vector3D notMoving(0., 0., 0.);

    bool oMoving = o.m_velocity.isValid();
    const Vector3D & o_velocity = oMoving ? o.m_velocity : notMoving;

    assert(o_velocity.isValid());

    // Predict the collision using the generic mesh function
    return predictCollision(lbox, lnormals, l.m_velocity,
                            obox, onormals, o_velocity,
                            time, normal);
}

////////////////////////// EMERGENCE //////////////////////////

bool getEmergenceTime(const Vector3D & p,    // Position of point
                      const Vector3D & u,    // Velocity of point
                      // float point_time,   // Time since position set
                      const Vector3D & l,    // Position on plane
                      const Vector3D & n,    // Plane normal
                      const Vector3D & v,    // Velocity of plane
                      // float plane_time,   // Time since position set
                      float & time)          // Emergence time return
{
    return !getCollisionTime(p, u, l, n, v, time);
}

static float min(float a, float b, float c)
{
    if (a < b) {
        if (a < c) {
            return a;
        } else {
            return c;
        }
    } else if (b < c) {
        return b;
    } else {
        return c;
    }
}

bool predictEmergence(const CoordList & l,         // Vertices of this mesh
                      const Vector3D & u,          // Velocity of this mesh
                      const WFMath::AxisBox<3> & o,// Bounding box of container
                      float & time)                // Returned time to emergence
{
    const WFMath::Point<3> & on = o.lowCorner();
    const WFMath::Point<3> & of = o.highCorner();
    float mintime = 4;
    bool flag = false;

    for(CoordList::const_iterator I = l.begin(); I != l.end(); ++I) {
        float xtime = (u.x() >= 0.f) ? ((of.x() - I->x()) / u.x())
                                   : ((on.x() - I->x()) / u.x());
        float ytime = (u.y() >= 0.f) ? ((of.y() - I->y()) / u.y())
                                   : ((on.y() - I->y()) / u.y());
        float ztime = (u.z() >= 0.f) ? ((of.z() - I->z()) / u.z())
                                   : ((on.z() - I->z()) / u.z());
        float ctime = min(xtime, ytime, ztime);
        debug(std::cout << xtime << ":" << ytime << ":" << ztime << ":" << ctime
                        << std::endl << std::flush;);
        if (ctime < mintime) {
            mintime = ctime;
        }
        if (ctime > time) {
            time = ctime;
            flag = true;
        }
    }

    return flag;
}

bool predictEmergence(const Location & l,  // This location
                      const Location & o,  // Other location
                      float & time)        // Returned time to collision
{
    // We are predicting emergence of l from its parent o
    // Orientation of o is irrelevant, as children and their relative
    // position are also oriented, so o's bbox is axis aligned.
    // In fact the only feature of o we are interested is its bbox, so
    // we could drop the Location, and take the bbox instead.
    // So all we need to do is get oriented points for l, and check
    // when they will first leave the axis aligned bounding values
    const WFMath::Point<3> & ln = l.m_bBox.lowCorner();
    const WFMath::Point<3> & lf = l.m_bBox.highCorner();

    CoordList lbox(8);

    lbox[0] = WFMath::Vector<3>(ln.x(), ln.y(), ln.z());
    lbox[1] = WFMath::Vector<3>(lf.x(), ln.y(), ln.z());
    lbox[2] = WFMath::Vector<3>(lf.x(), lf.y(), ln.z());
    lbox[3] = WFMath::Vector<3>(ln.x(), lf.y(), ln.z());
    lbox[4] = WFMath::Vector<3>(ln.x(), ln.y(), lf.z());
    lbox[5] = WFMath::Vector<3>(lf.x(), ln.y(), lf.z());
    lbox[6] = WFMath::Vector<3>(lf.x(), lf.y(), lf.z());
    lbox[7] = WFMath::Vector<3>(ln.x(), lf.y(), lf.z());

    // Orient the box corners
    if (l.m_orientation.isValid()) {
        for(int i = 0; i < 8; ++i) {
            lbox[i].rotate(l.m_orientation);
        }
    }

    // Translate the box corners
    for(int i = 0; i < 8; ++i) {
        lbox[i] += l.m_pos;
    }

    assert(l.m_velocity.isValid());

    // We are now ready to carry out the next step
    return predictEmergence(lbox, l.m_velocity, o.m_bBox, time);
}

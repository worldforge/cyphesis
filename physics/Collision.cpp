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
//
//
//                                   0
//


bool predictCollision(const Location & l,
                      const Location & o,
                      double & time,
                      Vector3D & normal)
{
    const WFMath::Point<3> & ln = l.m_bBox.lowCorner();
    const WFMath::Point<3> & lf = l.m_bBox.highCorner();
    const WFMath::Point<3> & on = o.m_bBox.lowCorner();
    const WFMath::Point<3> & of = o.m_bBox.highCorner();

    std::vector<WFMath::Vector<3> > lbox(8), obox(8);

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

    for(int i = 0; i < 8; ++i) {
        lbox[i].rotate(l.m_orientation);
        obox[i].rotate(o.m_orientation);
    }

    // FIXME Lots more to write yet
    return false;
}

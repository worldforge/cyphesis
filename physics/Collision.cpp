// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#include "modules/Location.h"

#include "Collision.h"

//
// This is the vertex layout used by the 2 3Dbox functions.
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


bool timeToHit(const Location & l,
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
}

// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2003-2005 Alistair Riddoch
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

// $Id: Locationtest.cpp,v 1.7 2006-10-26 00:48:16 alriddoch Exp $

#include "modules/Location.h"

#include <cassert>

int main()
{
    {
        Location testloc;

        assert(testloc.m_loc == 0);
        assert(!testloc.pos().isValid());
        assert(!testloc.velocity().isValid());
        assert(!testloc.orientation().isValid());
    }

    {
        Location testloc(0);

        assert(testloc.m_loc == 0);
        assert(!testloc.pos().isValid());
        assert(!testloc.velocity().isValid());
        assert(!testloc.orientation().isValid());
    }

    {
        Point3D testPos;
        Location testloc(0, testPos);

        assert(!testPos.isValid());

        assert(testloc.m_loc == 0);
        assert(!testloc.pos().isValid());
        assert(!testloc.velocity().isValid());
        assert(!testloc.orientation().isValid());
    }

    {
        Point3D testPos(0,0,0);
        Location testloc(0, testPos);

        assert(testPos.isValid());

        assert(testloc.m_loc == 0);
        assert(testloc.pos().isValid());
        assert(!testloc.velocity().isValid());
        assert(!testloc.orientation().isValid());
    }

    {
        Point3D testPos(0,0,0);
        Vector3D testVel;
        Location testloc(0, testPos, testVel);

        assert(testPos.isValid());
        assert(!testVel.isValid());

        assert(testloc.m_loc == 0);
        assert(testloc.pos().isValid());
        assert(!testloc.velocity().isValid());
        assert(!testloc.orientation().isValid());
    }

    {
        Point3D testPos(0,0,0);
        Vector3D testVel(0,0,0);
        Location testloc(0, testPos, testVel);

        assert(testPos.isValid());
        assert(testVel.isValid());

        assert(testloc.m_loc == 0);
        assert(testloc.pos().isValid());
        assert(testloc.velocity().isValid());
        assert(!testloc.orientation().isValid());

        Quaternion testOrientation(1, 0, 0, 0);
        assert(testOrientation.isValid());
        testloc.m_orientation = testOrientation;
        assert(testloc.orientation().isValid());
    }

    {
        Location testloc;

        testloc.m_bBox = BBox(Point3D(0,0,0), Point3D(1,1,1));

        assert(testloc.m_loc == 0);
        assert(!testloc.pos().isValid());
        assert(!testloc.velocity().isValid());
        assert(!testloc.orientation().isValid());
        assert(testloc.bBox().isValid());

        testloc.setBBox(BBox(Point3D(1,1,1), Point3D(2,2,2)));

        assert(testloc.squareBoxSize() == 3.f);
        assert(testloc.boxSize() == sqrtf(3.f));

        assert(testloc.squareRadius() == 12.f);
        assert(testloc.radius() == sqrtf(12.f));

        // Check cached values have been changed
    }

    return 0;
}

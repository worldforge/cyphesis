// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003-2005 Alistair Riddoch

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

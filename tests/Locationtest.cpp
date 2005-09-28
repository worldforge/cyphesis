// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

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

    return 0;
}

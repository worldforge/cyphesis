// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#include "physics/Collision.h"

#include "modules/Location.h"

#include <iostream>

#include <cassert>

int main()
{
    int ret = 0;
    {
        CoordList coords(1, Point3D(0, 0, 0));
        Vector3D velocity(1, 0, 0);
        BBox container(WFMath::Point<3>(-1, -1, -1), WFMath::Point<3>(1,1,1));
        float time = 0;

        bool res = predictEmergence(coords, velocity, container, time);

        std::cout << "Vertex emergence predicted after " << time
                  << " seconds" << std::endl << std::flush;

        velocity = Vector3D(0.1, 0.1, 0.1);
        time = 0;

        res = predictEmergence(coords, velocity, container, time);

        std::cout << "Vertex emergence predicted after " << time
                  << " seconds" << std::endl << std::flush;

        velocity = Vector3D(0.2, 0.2, -0.2);
        time = 0;

        res = predictEmergence(coords, velocity, container, time);

        std::cout << "Vertex emergence predicted after " << time
                  << " seconds" << std::endl << std::flush;

    }

    {
        float time = 0;

        Location a(0, Point3D(0,0,0), Vector3D(-1,1,1));
        a.m_bBox = BBox(WFMath::Point<3>(-1, -1, -1), WFMath::Point<3>(1,1,1));
        a.m_orientation = Quaternion(Vector3D(1,1,1), 45);

        Location b(0, Point3D(5,0,0));
        b.m_bBox = BBox(WFMath::Point<3>(-5, -5, -5), WFMath::Point<3>(5,5,5));

        predictEmergence(a, b, time);

        std::cout << "Vertex emergence predicted after " << time
                  << " seconds" << std::endl << std::flush;

    }

    return ret;
}

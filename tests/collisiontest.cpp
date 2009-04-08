// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2003 Alistair Riddoch
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

// $Id$

#include "physics/Collision.h"

#include "modules/Location.h"

#include <iostream>

#include <cassert>

int main()
{
    int ret = 0;
    {
        const Point3D position(-1.2, 1, 0);
        const Vector3D velocity(1.1, 0.9, 0);
        const Point3D plane(1, 1, 0);
        const Vector3D normal(-1, 1, 0);
        const Vector3D plane_velocity(-1, 0, 0);
        float time = -1;

        bool infront = getCollisionTime(position, velocity,
                                        plane, normal, plane_velocity,
                                        time);

        std::cout << "Vertex collision predicted after " << time
                  << " seconds, from "
                  << (infront ? "infront of" : "behind")
                  << " the plane" << std::endl << std::flush;
    }

    {
        CoordList mesha;
        NormalSet meshan;
        Vector3D meshav = Vector3D(0.1, 1, 0);

        mesha.push_back(Point3D(0,0,0));
        mesha.push_back(Point3D(1,0,0));
        mesha.push_back(Point3D(0,1,0));
        mesha.push_back(Point3D(0,0,1));
        meshan.insert(std::make_pair(0, Vector3D(0, 0, -1)));
        meshan.insert(std::make_pair(1, Vector3D(0, -1, 0)));
        meshan.insert(std::make_pair(2, Vector3D(-1, 0, 0)));
        meshan.insert(std::make_pair(3, Vector3D(1, 1, 1)));

        CoordList meshb;
        NormalSet meshbn;

        meshb.push_back(Point3D(5,-1,-1));
        meshb.push_back(Point3D(7,-1,-1));
        meshb.push_back(Point3D(5,2,-1));
        meshb.push_back(Point3D(5,-1,2));
        meshbn.insert(std::make_pair(0, Vector3D(0, 0, -1)));
        meshbn.insert(std::make_pair(1, Vector3D(0, -1, 0)));
        meshbn.insert(std::make_pair(2, Vector3D(-1, 0, 0)));
        meshbn.insert(std::make_pair(3, Vector3D(1, 1, 1)));

        Vector3D meshbv = Vector3D(-0.1, 1, 0);

        float time = 100;
        Vector3D normal;

        bool collided = predictCollision(mesha, meshan, meshav,
                                         meshb, meshbn, meshbv,
                                         time, normal);
        if (collided) {
            std::cout << "Mesh collision predicted after " << time
                      << " seconds" << std::endl << std::flush;
        } else {
            std::cout << "No mesh collision" << std::endl << std::flush;
            ret = 1;
        }
    }

    {
        Location a(0, Point3D(0,0,0), Vector3D(0.1,0,0));
        a.m_bBox = BBox(WFMath::Point<3>(-1, -1, -1), WFMath::Point<3>(1,1,1));
        a.m_orientation = Quaternion(Vector3D(1,1,1), 45);

        Location b(0, Point3D(5,0,0), Vector3D(-0.1,0,0));
        b.m_bBox = BBox(WFMath::Point<3>(-1, -1, -1), WFMath::Point<3>(1,1,1));
        b.m_orientation = Quaternion(Vector3D(1,1,1), 20);

        float time = 100;
        Vector3D normal;

        bool collided = predictCollision(a, b, time, normal);

        if (collided) {
            std::cout << "Location collision predicted after " << time
                      << " seconds" << std::endl << std::flush;
        } else {
            std::cout << "No location collision" << std::endl << std::flush;
            ret = 1;
        }
    }

    return ret;
}

// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#include "physics/Collision.h"

#include <cassert>

int main()
{
    {
        const Vector3D position(-1.2, 1, 0);
        const Vector3D velocity(1.1, 0.9, 0);
        const Vector3D plane(1, 1, 0);
        const Vector3D normal(-1, 1, 0);
        const Vector3D plane_velocity(-1, 0, 0);
        double time = -1;
        Vector3D collision_normal;

        bool infront = getCollisionTime(position, velocity,
                                        plane, normal, plane_velocity,
                                        time, collision_normal);

        std::cout << "Vertex collision predicted after " << time
                  << " seconds, from "
                  << (infront ? "infront of" : "behind")
                  << " the plane" << std::endl << std::flush;
    }

    {
        CoordList mesha;
        NormalSet meshan;
        Vector3D meshav = Vector3D(0.1, 1, 0);

        mesha.push_back(Vector3D(0,0,0));
        mesha.push_back(Vector3D(1,0,0));
        mesha.push_back(Vector3D(0,1,0));
        mesha.push_back(Vector3D(0,0,1));
        meshan.insert(std::make_pair(0, Vector3D(0, 0, -1)));
        meshan.insert(std::make_pair(1, Vector3D(0, -1, 0)));
        meshan.insert(std::make_pair(2, Vector3D(-1, 0, 0)));
        meshan.insert(std::make_pair(3, Vector3D(1, 1, 1)));

        CoordList meshb;
        NormalSet meshbn;

        meshb.push_back(Vector3D(5,-1,-1));
        meshb.push_back(Vector3D(7,-1,-1));
        meshb.push_back(Vector3D(5,2,-1));
        meshb.push_back(Vector3D(5,-1,2));
        meshbn.insert(std::make_pair(0, Vector3D(0, 0, -1)));
        meshbn.insert(std::make_pair(1, Vector3D(0, -1, 0)));
        meshbn.insert(std::make_pair(2, Vector3D(-1, 0, 0)));
        meshbn.insert(std::make_pair(3, Vector3D(1, 1, 1)));

        Vector3D meshbv = Vector3D(-0.1, 1, 0);

        double time = 100;
        Vector3D normal;

        bool collided = predictCollision(mesha, meshan, meshav,
                                         meshb, meshbn, meshbv,
                                         time, normal);
        if (collided) {
            std::cout << "Mesh collision predicted after " << time
                      << " seconds" << std::endl << std::flush;
        } else {
            std::cout << "No mesh collision" << std::endl << std::flush;
        }
    }

    return 0;
}

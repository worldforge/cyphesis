// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#include "physics/Vector3D.h"

#include <cassert>

int main()
{
    const Vector3D position(-1.2, 1, 0);
    const Vector3D velocity(1.1, 0.9, 0);
    const Vector3D plane(1, 1, 0);
    const Vector3D normal(-1, 1, 0);
    const Vector3D plane_velocity(-1, 0, 0);
    double time = -1;
    Vector3D collision_normal;

    bool infront = timeToHit(position, velocity,
                             plane, normal, plane_velocity,
                             time, collision_normal);

    std::cout << "Collision predicted after " << time << " seconds, currently "
              << (infront ? "infront of" : "behind")
              << " the plane" << std::endl << std::flush;

    return 0;
}

// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#include "physics/Collision.h"

#include "modules/Location.h"

#include <cassert>

int main()
{
    int ret = 0;
    {
        CoordList coords(1, Vector3D(0, 0, 0));
        Vector3D velocity(1, 0, 0);
        BBox container(WFMath::Point<3>(-1, -1, -1), WFMath::Point<3>(1,1,1));
        float time = 0;

        bool alreadyOut = predictEmergence(coords, velocity, container, time);

        std::cout << "Vertex emergence predicted after " << time
                  << " seconds" << std::endl << std::flush;
    }

    return ret;
}

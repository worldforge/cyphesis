// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#include <wfmath/point.h>
#include <wfmath/quaternion.h>

#include <iostream>

int main()
{
    int ret = 0;

    {
        const WFMath::Point<3> position(-1.2, 1, 0);

        const WFMath::Point<3> transformed = position.toParentCoords(WFMath::Point<3>(0,2,0), WFMath::Quaternion(1,0,0,0));

        std::cout << position << ", " << transformed << std::endl << std::flush;
    }

    {
        const WFMath::Point<3> position(-1.2, 1, 0);

        const WFMath::Point<3> transformed = position.toLocalCoords(WFMath::Point<3>(0,2,0), WFMath::Quaternion(1,0,0,0));

        std::cout << position << ", " << transformed << std::endl << std::flush;
    }

    {
        WFMath::Vector<3> direction(0, 1, 0);
        const WFMath::Quaternion quat(0.707, 0, 0, 0.707);

        std::cout << direction << std::endl << std::flush;

        std::cout << ", " << direction.rotate(quat) << std::endl << std::flush;
    }

    return ret;
}

// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2004 Alistair Riddoch
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


#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

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

        std::cout << direction.rotate(quat) << std::endl << std::flush;
    }

    {
        WFMath::Vector<3> direction(0, 1, 0);
        const WFMath::Quaternion quat(-1, 0, 0, 0);

        std::cout << direction << std::endl << std::flush;

        std::cout << direction.rotate(quat) << std::endl << std::flush;
    }

    {
        WFMath::Quaternion quat(1, 0, 0, 0);
        const WFMath::Quaternion rot(0.707, 0, 0, 0.707);
        std::cout << quat << std::endl << std::flush;
        quat *= rot;
        std::cout << quat << std::endl << std::flush;
        quat /= rot;
        std::cout << quat << std::endl << std::flush;
    }

    return ret;
}

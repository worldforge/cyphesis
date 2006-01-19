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

#include "rulesets/Entity.h"

#include "modules/Location.h"

// RelPos to self: (0,0,0)
// RelPos ent -> tlve: (-1,-1,0)
// RelPos tlve -> ent: (1,1,0)
// RelPos ent1 -> ent2: (2,0,0)
// RelPos ent3 -> ent4: (4,0,0)
// RelPos ent3 -> ent4: (-1.36914e-07,-4,0)
// RelPos ent3 -> ent4: (-2,-4,0)
// RelPos ent3 -> ent4: (-4,-2,0)

int main()
{
    int ret = 0;

    {
        Entity tlve("0", 0), ent("1", 1);

        ent.m_location.m_loc = &tlve;
        ent.m_location.m_pos = Point3D(1, 1, 0);
        ent.m_location.m_orientation = WFMath::Quaternion().identity();

        Point3D relPos = relativePos(ent.m_location, ent.m_location);

        std::cout << "RelPos to self: " << relPos << std::endl << std::flush;

        relPos = relativePos(ent.m_location, tlve.m_location);

        std::cout << "RelPos ent -> tlve: " << relPos
                  << std::endl << std::flush;

        relPos = relativePos(tlve.m_location, ent.m_location);

        std::cout << "RelPos tlve -> ent: " << relPos
                  << std::endl << std::flush;

        ent.m_location.m_loc = 0;
    }

    {
        Entity tlve("0", 0), ent1("1", 1), ent2("2", 2);

        ent1.m_location.m_loc = &tlve;
        ent1.m_location.m_pos = Point3D(-1, 1, 0);
        ent1.m_location.m_orientation = WFMath::Quaternion().identity();

        ent2.m_location.m_loc = &tlve;
        ent2.m_location.m_pos = Point3D(1, 1, 0);
        ent2.m_location.m_orientation = WFMath::Quaternion().identity();

        Point3D relPos = relativePos(ent1.m_location, ent2.m_location);

        std::cout << "RelPos ent1 -> ent2: " << relPos
                  << std::endl << std::flush;

        ent1.m_location.m_loc = 0;
        ent2.m_location.m_loc = 0;
    }

    {
        Entity tlve("0", 0), ent1("1", 1), ent2("2", 2),
               ent3("3", 3), ent4("4", 4);

        ent1.m_location.m_loc = &tlve;
        ent1.m_location.m_pos = Point3D(-1, 1, 0);
        ent1.m_location.m_orientation = WFMath::Quaternion().identity();

        ent2.m_location.m_loc = &tlve;
        ent2.m_location.m_pos = Point3D(1, 1, 0);
        ent2.m_location.m_orientation = WFMath::Quaternion().identity();

        ent3.m_location.m_loc = &ent1;
        ent3.m_location.m_pos = Point3D(-1, 1, 0);
        ent3.m_location.m_orientation = WFMath::Quaternion().identity();

        ent4.m_location.m_loc = &ent2;
        ent4.m_location.m_pos = Point3D(1, 1, 0);
        ent4.m_location.m_orientation = WFMath::Quaternion().identity();

        Point3D relPos = relativePos(ent3.m_location, ent4.m_location);

        std::cout << "RelPos ent3 -> ent4: " << relPos
                  << std::endl << std::flush;

        ent1.m_location.m_loc = 0;
        ent2.m_location.m_loc = 0;
        ent3.m_location.m_loc = 0;
        ent4.m_location.m_loc = 0;
    }

    {
        Entity tlve("0", 0), ent1("1", 1), ent2("2", 2), ent3("3", 3), ent4("4", 4);

        ent1.m_location.m_loc = &tlve;
        ent1.m_location.m_pos = Point3D(-1, 1, 0);
        ent1.m_location.m_orientation = WFMath::Quaternion().identity();

        ent2.m_location.m_loc = &tlve;
        ent2.m_location.m_pos = Point3D(1, 1, 0);
        ent2.m_location.m_orientation = WFMath::Quaternion().identity();

        ent3.m_location.m_loc = &ent1;
        ent3.m_location.m_pos = Point3D(-1, 1, 0);
        ent3.m_location.m_orientation = WFMath::Quaternion(2, M_PI / 2.f);

        ent4.m_location.m_loc = &ent2;
        ent4.m_location.m_pos = Point3D(1, 1, 0);
        ent4.m_location.m_orientation = WFMath::Quaternion().identity();

        Point3D relPos = relativePos(ent3.m_location, ent4.m_location);

        std::cout << "RelPos ent3 -> ent4: " << relPos
                  << std::endl << std::flush;

        ent1.m_location.m_loc = 0;
        ent2.m_location.m_loc = 0;
        ent3.m_location.m_loc = 0;
        ent4.m_location.m_loc = 0;
    }

    {
        Entity tlve("0", 0), ent1("1", 1), ent2("2", 2),
               ent3("3", 3), ent4("4", 4);

        ent1.m_location.m_loc = &tlve;
        ent1.m_location.m_pos = Point3D(-1, 1, 0);
        ent1.m_location.m_orientation = WFMath::Quaternion().identity();

        ent2.m_location.m_loc = &tlve;
        ent2.m_location.m_pos = Point3D(1, 1, 0);
        ent2.m_location.m_orientation = WFMath::Quaternion(2, -M_PI / 2.f);

        ent3.m_location.m_loc = &ent1;
        ent3.m_location.m_pos = Point3D(-1, 1, 0);
        ent3.m_location.m_orientation = WFMath::Quaternion(2, M_PI / 2.f);

        ent4.m_location.m_loc = &ent2;
        ent4.m_location.m_pos = Point3D(1, 1, 0);
        ent4.m_location.m_orientation = WFMath::Quaternion().identity();

        Point3D relPos = relativePos(ent3.m_location, ent4.m_location);

        std::cout << "RelPos ent3 -> ent4: " << relPos
                  << std::endl << std::flush;

        ent1.m_location.m_loc = 0;
        ent2.m_location.m_loc = 0;
        ent3.m_location.m_loc = 0;
        ent4.m_location.m_loc = 0;
    }

    {
        Entity tlve("0", 0), ent1("1", 1), ent2("2", 2),
               ent3("3", 3), ent4("4", 4);

        ent1.m_location.m_loc = &tlve;
        ent1.m_location.m_pos = Point3D(-1, 1, 0);
        ent1.m_location.m_orientation = WFMath::Quaternion(2, M_PI / 2.f);

        ent2.m_location.m_loc = &tlve;
        ent2.m_location.m_pos = Point3D(1, 1, 0);
        ent2.m_location.m_orientation = WFMath::Quaternion().identity();

        ent3.m_location.m_loc = &ent1;
        ent3.m_location.m_pos = Point3D(-1, 1, 0);
        ent3.m_location.m_orientation = WFMath::Quaternion(2, M_PI / 2.f);

        ent4.m_location.m_loc = &ent2;
        ent4.m_location.m_pos = Point3D(1, 1, 0);
        ent4.m_location.m_orientation = WFMath::Quaternion().identity();

        Point3D relPos = relativePos(ent3.m_location, ent4.m_location);
        Vector3D distance = distanceTo(ent3.m_location, ent4.m_location);

        std::cout << "RelPos ent3 -> ent4: " << relPos
                  << " Distance ent3 -> ent4: " << distance
                  << std::endl << std::flush;

        ent1.m_location.m_loc = 0;
        ent2.m_location.m_loc = 0;
        ent3.m_location.m_loc = 0;
        ent4.m_location.m_loc = 0;
    }

    return ret;
}

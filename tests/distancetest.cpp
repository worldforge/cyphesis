// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#include "rulesets/Entity.h"

#include "modules/Location.h"

// Distance to self: (0,0,0)
// Distance ent -> tlve: (-1,-1,0)
// Distance tlve -> ent: (1,1,0)
// Distance ent1 -> ent2: (2,0,0)
// Distance ent3 -> ent4: (4,0,0)
// Distance ent3 -> ent4: (-1.36914e-07,-4,0)
// Distance ent3 -> ent4: (-2,-4,0)
// Distance ent3 -> ent4: (-4,-2,0)

int main()
{
    int ret = 0;

    {
        Entity tlve("0"), ent("1");

        ent.m_location.m_loc = &tlve;
        ent.m_location.m_pos = Point3D(1, 1, 0);
        ent.m_location.m_orientation = WFMath::Quaternion().identity();

        Point3D relPos = relativePos(ent.m_location, ent.m_location);

        std::cout << "Distance to self: " << relPos << std::endl << std::flush;

        relPos = relativePos(ent.m_location, tlve.m_location);

        std::cout << "Distance ent -> tlve: " << relPos
                  << std::endl << std::flush;

        relPos = relativePos(tlve.m_location, ent.m_location);

        std::cout << "Distance tlve -> ent: " << relPos
                  << std::endl << std::flush;
    }

    {
        Entity tlve("0"), ent1("1"), ent2("2");

        ent1.m_location.m_loc = &tlve;
        ent1.m_location.m_pos = Point3D(-1, 1, 0);
        ent1.m_location.m_orientation = WFMath::Quaternion().identity();

        ent2.m_location.m_loc = &tlve;
        ent2.m_location.m_pos = Point3D(1, 1, 0);
        ent2.m_location.m_orientation = WFMath::Quaternion().identity();

        Point3D relPos = relativePos(ent1.m_location, ent2.m_location);

        std::cout << "Distance ent1 -> ent2: " << relPos
                  << std::endl << std::flush;
    }

    {
        Entity tlve("0"), ent1("1"), ent2("2"), ent3("3"), ent4("4");

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

        std::cout << "Distance ent3 -> ent4: " << relPos
                  << std::endl << std::flush;
    }

    {
        Entity tlve("0"), ent1("1"), ent2("2"), ent3("3"), ent4("4");

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

        std::cout << "Distance ent3 -> ent4: " << relPos
                  << std::endl << std::flush;
    }

    {
        Entity tlve("0"), ent1("1"), ent2("2"), ent3("3"), ent4("4");

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

        std::cout << "Distance ent3 -> ent4: " << relPos
                  << std::endl << std::flush;
    }

    {
        Entity tlve("0"), ent1("1"), ent2("2"), ent3("3"), ent4("4");

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

        std::cout << "Distance ent3 -> ent4: " << relPos
                  << std::endl << std::flush;
    }

    return ret;
}

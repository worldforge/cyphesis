// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#include "rulesets/Entity.h"

#include "modules/Location.h"

int main()
{
    int ret = 0;

    {
        Entity tlve("0"), ent("1");

        ent.m_location.m_loc = &tlve;
        ent.m_location.m_pos = Point3D(1, 1, 0);
        ent.m_location.m_orientation = WFMath::Quaternion().identity();

        float relPos = distance(ent.m_location, ent.m_location);

        std::cout << "Distance to self: " << relPos << std::endl << std::flush;
    }

    {
        Entity tlve("0"), ent1("1"), ent2("2");

        ent1.m_location.m_loc = &tlve;
        ent1.m_location.m_pos = Point3D(-1, 1, 0);
        ent1.m_location.m_orientation = WFMath::Quaternion().identity();

        ent2.m_location.m_loc = &tlve;
        ent2.m_location.m_pos = Point3D(1, 1, 0);
        ent2.m_location.m_orientation = WFMath::Quaternion().identity();

        float relPos = distance(ent1.m_location, ent2.m_location);

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

        float relPos = distance(ent3.m_location, ent4.m_location);

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

        float relPos = distance(ent3.m_location, ent4.m_location);

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

        float relPos = distance(ent3.m_location, ent4.m_location);

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

        float relPos = distance(ent3.m_location, ent4.m_location);

        std::cout << "Distance ent3 -> ent4: " << relPos
                  << std::endl << std::flush;
    }

    return ret;
}

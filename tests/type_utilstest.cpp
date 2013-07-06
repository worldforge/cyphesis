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

#include "common/type_utils_impl.h"

#include "physics/Vector3D.h"
#include "physics/Quaternion.h"

#include <wfmath/atlasconv.h>

#include <cassert>

typedef std::vector<Quaternion> OrientationList;

using Atlas::Message::ListType;

int main()
{
    {
        CoordList pointList;

        Atlas::Message::ListType point;
        point.push_back(1.5);
        point.push_back(1.5);
        point.push_back(1.5);

        Atlas::Message::ListType pointData;
        pointData.push_back(point);
        pointData.push_back(point);
        pointData.push_back(point);
        pointData.push_back(point);
        pointData.push_back(point);
        pointData.push_back(point);

        assert(point.size() == 3);

        assert(pointData.size() == 6);

        assert(pointList.size() == 0);

        objectListFromMessage<Point3D>(pointData, pointList);

        assert(pointList.size() == 6);

        CoordList::const_iterator I = pointList.begin();
        CoordList::const_iterator Iend = pointList.end();
        for (; I != Iend; ++I) {
            const Point3D & q = *I;
            assert(q == Point3D(1.5, 1.5, 1.5));
        }
    }
    {
        VectorList pointList;

        ListType point;
        point.push_back(1.5);
        point.push_back(1.5);
        point.push_back(1.5);

        ListType pointData;
        pointData.push_back(point);
        pointData.push_back(point);
        pointData.push_back(point);
        pointData.push_back(point);
        pointData.push_back(point);
        pointData.push_back(point);

        assert(point.size() == 3);

        assert(pointData.size() == 6);

        assert(pointList.size() == 0);

        objectListFromMessage<Vector3D>(pointData, pointList);

        assert(pointList.size() == 6);

        VectorList::const_iterator I = pointList.begin();
        VectorList::const_iterator Iend = pointList.end();
        for (; I != Iend; ++I) {
            const Vector3D & q = *I;
            assert(q == Vector3D(1.5, 1.5, 1.5));
        }
    }
    {
        OrientationList pointList;

        ListType point;
        point.push_back(0);
        point.push_back(0);
        point.push_back(0);
        point.push_back(1);

        ListType pointData;
        pointData.push_back(point);
        pointData.push_back(point);
        pointData.push_back(point);
        pointData.push_back(point);
        pointData.push_back(point);
        pointData.push_back(point);

        assert(point.size() == 4);

        assert(pointData.size() == 6);

        assert(pointList.size() == 0);

        objectListFromMessage<Quaternion>(pointData, pointList);

        assert(pointList.size() == 6);

        OrientationList::const_iterator I = pointList.begin();
        OrientationList::const_iterator Iend = pointList.end();
        for (; I != Iend; ++I) {
            const Quaternion & q = *I;
            assert(q == Quaternion().identity());
        }
    }
    {
        ListType list(1, "1");
        IdList id_list;

        int res = idListFromAtlas(list, id_list);
        assert(res == 0);
        assert(id_list.size() == list.size());

        list.clear();
        idListasObject(id_list, list);
        assert(id_list.size() == list.size());

        list = ListType(1, 2.0);
        res = idListFromAtlas(list, id_list);
        assert(res == -1);
        assert(id_list.size() == 0);
    }
}

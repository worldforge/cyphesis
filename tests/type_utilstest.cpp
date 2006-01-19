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

#include "common/type_utils.h"

#include "physics/Vector3D.h"

#include <wfmath/atlasconv.h>

#include <cassert>

int main()
{
    {
        std::vector<Point3D> pointList;

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

        objectListFromMessage<Point3D, CoordList>(pointData, pointList);

        assert(pointList.size() == 6);
    }

    // FIXME More tests please Vector3D, Quaternion, etc.
}

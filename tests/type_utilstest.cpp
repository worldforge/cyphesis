// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

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

// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2003 Alistair Riddoch

#include "BBox.h"

#include "common/const.h"

WFMath::CoordType boxSquareSize(const BBox & box)
{
    if (!box.isValid()) {
        return consts::minSqrBoxSize;
    }

    WFMath::CoordType ans = 0;

    for(int i = 0; i < 3; ++i) {
        ans += square(box.highCorner()[i] - box.lowCorner()[i]);
    }

    return ans;
}

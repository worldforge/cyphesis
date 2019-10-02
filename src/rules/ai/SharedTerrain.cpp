/*
 Copyright (C) 2015 erik

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifdef HAVE_CONFIG_H
#endif

#include "SharedTerrain.h"

#include <Mercator/Segment.h>

SharedTerrain::SharedTerrain() :
        m_terrain(new Mercator::Terrain())
{
}

std::vector<SharedTerrain::BasePointDefinition> SharedTerrain::setBasePoints(const std::vector<BasePointDefinition>& basepoints)
{
    std::vector<BasePointDefinition> changedPoints;
    for (auto& basepointDef : basepoints) {
        Mercator::BasePoint existingPoint;
        if (!m_terrain->getBasePoint(basepointDef.x, basepointDef.y, existingPoint)
                || (existingPoint.height() != basepointDef.basePoint.height() || existingPoint.falloff() != basepointDef.basePoint.falloff()
                        || existingPoint.roughness() != basepointDef.basePoint.roughness())) {
            m_terrain->setBasePoint(basepointDef.x, basepointDef.y, basepointDef.basePoint);
            changedPoints.push_back(basepointDef);
        }
    }
    return changedPoints;
}

void SharedTerrain::blitHeights(int xMin, int xMax, int yMin, int yMax, std::vector<float>& heights) const
{
    int segmentResolution = m_terrain->getResolution();
    int xSize = xMax - xMin;

    int segmentXMin = static_cast<int>(std::lround(floor(xMin / (double)segmentResolution)));
    int segmentXMax = static_cast<int>(std::lround(floor(xMax / (double)segmentResolution)));
    int segmentYMin = static_cast<int>(std::lround(floor(yMin / (double)segmentResolution)));
    int segmentYMax = static_cast<int>(std::lround(floor(yMax / (double)segmentResolution)));

    for (int segmentX = segmentXMin; segmentX <= segmentXMax; ++segmentX) {
        for (int segmentY = segmentYMin; segmentY <= segmentYMax; ++segmentY) {

            int segmentXStart = segmentX * segmentResolution;
            int segmentYStart = segmentY * segmentResolution;
            int dataXOffset = segmentXStart - xMin;
            int dataYOffset = segmentYStart - yMin;

            int xStart = std::max(xMin - segmentXStart, 0);
            int yStart = std::max(yMin - segmentYStart, 0);
            int xEnd = std::min<int>(xMax - segmentXStart, segmentResolution);
            int yEnd = std::min<int>(yMax - segmentYStart, segmentResolution);

            Mercator::Segment* segment = m_terrain->getSegmentAtIndex(segmentX, segmentY);
            if (segment) {
                if (!segment->isValid()) {
                    segment->populate();
                }

                for (int x = xStart; x < xEnd; ++x) {
                    for (int y = yStart; y < yEnd; ++y) {
                        heights[((dataYOffset + y) * xSize) + (dataXOffset + x)] = segment->get(x, y);
                    }
                }
            } else {
                //No valid segment found; fill with default value of -10.
                for (int x = xStart; x < xEnd; ++x) {
                    for (int y = yStart; y < yEnd; ++y) {
                        heights[((dataYOffset + y) * xSize) + (dataXOffset + x)] = -10;
                    }
                }

            }
        }
    }
}

const Mercator::Terrain& SharedTerrain::getTerrain() const
{
    return *m_terrain;
}



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
#include "config.h"
#endif

#include <rulesets/mind/SharedTerrain.h>

#include <Mercator/Segment.h>

#include <cmath>

SharedTerrain::SharedTerrain() : m_terrain(new Mercator::Terrain())
{
}

SharedTerrain::~SharedTerrain()
{
}

void SharedTerrain::setBasePoints(const std::vector<BasePointDefinition>& basepoints)
{
    for (auto& basepointDef : basepoints) {
        Mercator::BasePoint existingPoint;
        if (!m_terrain->getBasePoint(basepointDef.x, basepointDef.y, existingPoint) ||
                (existingPoint.height() != basepointDef.basePoint.height() || existingPoint.falloff() != basepointDef.basePoint.falloff() || existingPoint.roughness() != basepointDef.basePoint.roughness())) {
            m_terrain->setBasePoint(basepointDef.x, basepointDef.y, basepointDef.basePoint);
        }
    }
}

void SharedTerrain::blitHeights(int xMin, int xMax, int yMin, int yMax, std::vector<float>& heights) const
{
    for (int i = 0; i < ((xMax - xMin) * (yMax - yMin)); ++i) {
        heights[i] = 5;
    }
    return;

    int segmentResolution = m_terrain->getResolution();
    int xSize = xMax - xMin;

    int segmentXMin = std::lround(floor(xMin / (double)segmentResolution));
    int segmentXMax = std::lround(floor(xMax / (double)segmentResolution));
    int segmentYMin = std::lround(floor(yMin / (double)segmentResolution));
    int segmentYMax = std::lround(floor(yMax / (double)segmentResolution));

    for (int segmentX = segmentXMin; segmentX <= segmentXMax; ++segmentX) {
        for (int segmentY = segmentYMin; segmentY <= segmentYMax; ++segmentY) {

            Mercator::Segment* segment = m_terrain->getSegment(segmentX, segmentY);
            if (!segment->isValid()) {
                segment->populate();
            }

            int segmentXStart = segmentX * segmentResolution;
            int segmentYStart = segmentY * segmentResolution;
            int dataXOffset = segmentXStart - xMin;
            int dataYOffset = segmentYStart - yMin;

            int xStart = std::max(xMin - segmentXStart, 0);
            int yStart = std::max(yMin - segmentYStart, 0);
            int xEnd = std::min<int>(xMax - segmentXStart, segmentResolution);
            int yEnd = std::min<int>(yMax - segmentYStart, segmentResolution);

            for (int x = xStart; x < xEnd; ++x) {
                for (int y = yStart; y < yEnd; ++y) {
                    heights[((dataYOffset + y) * xSize) + (dataXOffset + x)] = segment->get(x, y);
                }
            }
        }
    }
}



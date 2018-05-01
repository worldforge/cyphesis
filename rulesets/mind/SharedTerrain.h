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
#ifndef RULESETS_MIND_SHAREDTERRAIN_H_
#define RULESETS_MIND_SHAREDTERRAIN_H_

#include "navigation/IHeightProvider.h"

#include <Mercator/Terrain.h>

#include <vector>


/**
 * @brief A terrain representation that's shared between multiple entities.
 *
 */
class SharedTerrain : public IHeightProvider
{
    public:

        struct BasePointDefinition {
            int x;
            int y;
            Mercator::BasePoint basePoint;
        };

        SharedTerrain();

        ~SharedTerrain() override = default;

        /**
         * @brief Sets base points.
         *
         * Only those that have changed are processed. It's thus safe to call this from each entity sharing this instance.
         * @param basepoints
         */
        std::vector<BasePointDefinition> setBasePoints(const std::vector<BasePointDefinition>& basepoints);

        void blitHeights(int xMin, int xMax, int yMin, int yMax, std::vector<float>& heights) const override;

        const Mercator::Terrain& getTerrain() const;

    private:

        std::unique_ptr<Mercator::Terrain> m_terrain;
};

#endif /* RULESETS_MIND_SHAREDTERRAIN_H_ */

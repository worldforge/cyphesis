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
#ifndef AICLIENT_AWARENESSSTORE_H_
#define AICLIENT_AWARENESSSTORE_H_

#include <wfmath/axisbox.h>
#include <wfmath/point.h>

#include <unordered_map>
#include <memory>

class IHeightProvider;
class Awareness;
class LocatedEntity;


class AwarenessStore
{
    public:
        AwarenessStore(float agentRadius, float agentHeight, IHeightProvider& heightProvider, const WFMath::AxisBox<3>& extent, int tileSize = 64);
        virtual ~AwarenessStore();

        std::shared_ptr<Awareness> requestAwareness(LocatedEntity& domainEntity);

    private:
        /**
         * @brief The radius of the agents.
         */
        float mAgentRadius;
        float mAgentHeight;

        IHeightProvider& mHeightProvider;
        const WFMath::AxisBox<3>& mExtent;


        int mTileSize;

        std::unordered_map<int, std::weak_ptr<Awareness>> m_awarenesses;
};

#endif /* AICLIENT_AWARENESSSTORE_H_ */

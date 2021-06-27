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
#ifndef RULESETS_MIND_AWARENESSSTORE_H_
#define RULESETS_MIND_AWARENESSSTORE_H_

#include <wfmath/axisbox.h>
#include <wfmath/point.h>

#include <string>
#include <unordered_map>
#include <memory>

struct IHeightProvider;

class Awareness;

class LocatedEntity;
class MemEntity;

class AwarenessStore
{
    public:
        AwarenessStore(float agentRadius,
                       float agentHeight,
                       float stepHeight,
                       IHeightProvider& heightProvider,
                       int tileSize = 64);

        virtual ~AwarenessStore() = default;

        std::shared_ptr<Awareness> requestAwareness(const MemEntity& domainEntity);

    private:
        /**
         * @brief The radius of the agents.
         */
        float mAgentRadius;
        float mAgentHeight;
        float mStepHeight;

        IHeightProvider& mHeightProvider;


        int mTileSize;

        /**
         * @brief A map of existing awarenesses, ordered by the id of the domain entity.
         */
        std::unordered_map<long, std::weak_ptr<Awareness>> m_awarenesses;
};

#endif /* RULESETS_MIND_AWARENESSSTORE_H_ */

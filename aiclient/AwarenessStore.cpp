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

#include "navigation/Awareness.h"
#include "rulesets/LocatedEntity.h"

#include <aiclient/AwarenessStore.h>

AwarenessStore::AwarenessStore(float agentRadius, float agentHeight, IHeightProvider& heightProvider, const WFMath::AxisBox<3>& extent, int tileSize) :
        mAgentRadius(agentRadius), mAgentHeight(agentHeight), mHeightProvider(heightProvider), mExtent(extent), mTileSize(tileSize)
{
}

AwarenessStore::~AwarenessStore()
{
}

std::shared_ptr<Awareness> AwarenessStore::requestAwareness(LocatedEntity& domainEntity)
{
    //Check if there's already an awareness for the domain entity.
    auto I = m_awarenesses.find(domainEntity.getIntId());
    if (I != m_awarenesses.end()) {
        //check if it's still valid
        if (!I->second.expired()) {
            return I->second.lock();
        }
    }

    auto awareness = std::make_shared < Awareness > (domainEntity, mAgentRadius, mAgentHeight, mHeightProvider, mExtent, mTileSize);
    m_awarenesses.insert(std::make_pair(domainEntity.getIntId(), std::weak_ptr < Awareness > (awareness)));
    return awareness;
}


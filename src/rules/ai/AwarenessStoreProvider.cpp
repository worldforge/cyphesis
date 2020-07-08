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

#include "AwarenessStoreProvider.h"
#include "common/TypeNode.h"
#include "common/debug.h"
#include "common/Property.h"

#include <wfmath/ball.h>
#include <wfmath/atlasconv.h>

static const bool debug_flag = false;


AwarenessStoreProvider::AwarenessStoreProvider(IHeightProvider& heightProvider)
        : m_heightProvider(heightProvider)
{
}

AwarenessStore& AwarenessStoreProvider::getStore(const TypeNode* type, int tileSize)
{
    auto I = m_awarenessStores.find(type->name());
    if (I != m_awarenessStores.end()) {
        debug_print("Reusing awareness store for type " << type->name() << ".");
        return I->second;
    }


    double agentHeight = 2;
    float agentRadius = 0.4;
    auto propertyI = type->defaults().find("bbox");
    if (propertyI != type->defaults().end()) {
        auto& propBase = propertyI->second;
        Atlas::Message::Element data;
        propBase->get(data);
        WFMath::AxisBox<3> bbox{};
        bbox.fromAtlas(data);
        agentHeight = bbox.highCorner().y() - bbox.lowCorner().y();

        WFMath::AxisBox<2> agent2dBbox(WFMath::Point<2>(bbox.lowCorner().x(), bbox.lowCorner().z()), WFMath::Point<2>(bbox.highCorner().x(), bbox.highCorner().z()));
        agentRadius = std::max(0.2, agent2dBbox.boundingSphere().radius()); //Don't make the radius smaller than 0.2 meters, to avoid too many cells
    }
    double stepHeight = 0.2;
    auto stepFactorI = type->defaults().find("step_factor");
    if (stepFactorI != type->defaults().end()) {
        Atlas::Message::Element data;
        stepFactorI->second->get(data);
        if (data.isNum()) {
            stepHeight = agentHeight * data.asNum();
        }
    }

    return m_awarenessStores.emplace(type->name(), AwarenessStore(agentRadius, (float) agentHeight, stepHeight, m_heightProvider, tileSize)).first->second;

}


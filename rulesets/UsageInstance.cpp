/*
 Copyright (C) 2018 Erik Ogenvik

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

#include <rulesets/entityfilter/Providers.h>
#include "UsageInstance.h"
#include "LocatedEntity.h"
#include "BaseWorld.h"

using Atlas::Message::Element;
using Atlas::Message::ListType;
using Atlas::Message::MapType;
using Atlas::Objects::Root;
using Atlas::Objects::Operation::Action;
using Atlas::Objects::Operation::Use;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Entity::RootEntity;

std::pair<bool, std::string> UsageInstance::isValid() const
{

    if (definition.constraint) {
        EntityFilter::QueryContext queryContext{*tool, actor.get(), tool.get()};
        queryContext.entity_lookup_fn = [](const std::string& id) { return BaseWorld::instance().getEntity(id);};

        if (!definition.constraint->match(queryContext)) {
            return {false, "Constraint does not match."};
        }
    }

    for (size_t i = 0; i < targets.size(); ++i) {
        auto& entry = targets[i];
        if (entry.m_parent->isDestroyed()) {
            return {false, String::compose("Target nr. %1 is destroyed.", i)};
        }
        EntityFilter::QueryContext queryContext{*entry.m_parent, actor.get(), tool.get()};
        queryContext.entity_lookup_fn = [](const std::string& id) { return BaseWorld::instance().getEntity(id);};
        if (!definition.targets[i]->match(queryContext)) {
            return {false, String::compose("Target nr. %1 does not match the filter.", i)};
        }
    }

    for (size_t i = 0; i < consumed.size(); ++i) {
        auto& entry = consumed[i];
        if (entry.m_parent->isDestroyed()) {
            return {false, String::compose("Consumable nr. %1 is destroyed.", i)};
        }
        EntityFilter::QueryContext queryContext{*entry.m_parent, actor.get(), tool.get()};
        queryContext.entity_lookup_fn = [](const std::string& id) { return BaseWorld::instance().getEntity(id);};
        if (!definition.consumed[i]->match(queryContext)) {
            return {false, String::compose("Consumable nr. %1 does not match the filter.", i)};
        }
    }

    return {true, ""};
}

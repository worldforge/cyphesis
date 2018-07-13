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

#include "UsagesProperty.h"
#include "common/TypeNode.h"
#include "entityfilter/Providers.h"

void UsagesProperty::set(const Atlas::Message::Element& val)
{
    Property::set(val);

    m_usages.clear();

    for (auto& entry : m_data) {
        if (entry.isMap()) {
            auto operationI = entry.Map().find("operation");
            if (operationI == entry.Map().end() || !operationI->second.isString()) {
                continue;
            }
            auto taskI = entry.Map().find("task");
            if (taskI == entry.Map().end() || !taskI->second.isString()) {
                continue;
            }
            std::unique_ptr<EntityFilter::Filter> filterPtr;
            auto targetI = entry.Map().find("filter");
            if (targetI != entry.Map().end() && targetI->second.isString()) {
                auto filter = targetI->second.String();
                filterPtr.reset(new EntityFilter::Filter(filter, new EntityFilter::ProviderFactory()));

            }
            m_usages.emplace_back(Usage{operationI->second.String(), taskI->second.String(), std::move(filterPtr)});
        }
    }


//    m_targetsAndTheirOperations.clear();
//    for (auto& entry : m_data) {
//        if (entry.isMap()) {
//            auto operationI = entry.Map().find("operation");
//            if (operationI == entry.Map().end() || !operationI->second.isString()) {
//                continue;
//            }
//            std::string target;
//            auto targetI = entry.Map().find("target");
//            if (targetI != entry.Map().end() && targetI->second.isString()) {
//                target = targetI->second.String();
//            }
//            m_targetsAndTheirOperations[target].push_back(operationI->second.String());
//        }
//    }
}

void UsagesProperty::install(TypeNode* typeNode, const std::string&)
{
//    auto operationsProp = new Property<Atlas::Message::ListType>();
//    for (auto& activation : m_usages) {
//        operationsProp->data().emplace_back(activation.operation);
//    }
//    operationsProp->addFlags(per_ephem);
//    typeNode->injectProperty("operations", operationsProp);
}

std::string UsagesProperty::findMatchingTask(const std::string& operation, LocatedEntity* target) const
{
    for (auto& usage : m_usages) {
        if (usage.operation == operation) {
            if (usage.filter) {
                if (!usage.filter->match(*target)) {
                    continue;
                }
            }
            return usage.task;
        }
    }
    return "";
}

/*
 Copyright (C) 2020 Erik Ogenvik

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

#include "TypeUpdateCoordinator.h"
#include "rules/LocatedEntity.h"
#include "Account.h"
#include "common/debug.h"
#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>

static const bool debug_flag = false;

TypeUpdateCoordinator::TypeUpdateCoordinator(Inheritance& inheritance, WorldRouter& worldRouter, ServerRouting& serverRouting)
{
    /**
        * When types are updated we will send an "change" op to all connected clients.
        */
    inheritance.typesUpdated.connect([&](const std::map<const TypeNode*, TypeNode::PropertiesUpdate> typeNodes) {
        //Send Change ops to all clients
        if (!typeNodes.empty()) {
            Atlas::Objects::Operation::Change change;
            std::vector<Atlas::Objects::Root> args;
            for (auto& entry: typeNodes) {
                auto typeNode = entry.first;
                Atlas::Objects::Entity::Anonymous o;
                o->setObjtype(typeNode->description(Visibility::PRIVATE)->getObjtype());
                o->setId(typeNode->name());
                args.emplace_back(o);
            }
            change->setArgs(args);

            auto& accounts = serverRouting.getAccounts();
            OpVector res;
            for (auto& entry : accounts) {
                entry.second->operation(change, res);
            }

            debug_print("WorldRouter::messageToClients {"
                                << change->getParent() << ":"
                                << change->getFrom() << ":" << change->getTo() << "}")

            //Go through all world entities and check if they need to be updated
            auto& entities = worldRouter.getEntities();
            for (auto& entry : entities) {
                auto entity = entry.second;
                auto I = typeNodes.find(entity->getType());
                if (I != typeNodes.end()) {
                    auto typeNode = I->first;
//                    for (auto& removedPropName : I->second.removedProps) {
//                        if (entity->getProperties().find(removedPropName) == entity->getProperties().end()) {
//                            auto prop = typeNode->defaults().find(removedPropName)->second;
//                            prop->remove(entity, removedPropName);
//                        }
//                    }
                    for (auto& changedPropName : I->second.changedProps) {
                        if (entity->getProperties().find(changedPropName) == entity->getProperties().end()) {
                            auto& prop = typeNode->defaults().find(changedPropName)->second;
                            prop->apply(*entity);
                            entity->propertyApplied(changedPropName, *prop);
                        }
                    }
                    for (auto& newPropName : I->second.changedProps) {
                        if (entity->getProperties().find(newPropName) == entity->getProperties().end()) {
                            auto& prop = typeNode->defaults().find(newPropName)->second;
                            prop->apply(*entity);
                            entity->propertyApplied(newPropName, *prop);
                        }
                    }
                }
            }
        }
    });
}

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

#include "ContainerAccessProperty.h"
#include "BaseWorld.h"
#include "rules/Domain.h"
#include <Atlas/Objects/Entity.h>

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Message::ListType;
using Atlas::Message::FloatType;

ContainerAccessProperty::ContainerAccessProperty()
        : PropertyBase(prop_flag_persistence_ephem)
{
}

int ContainerAccessProperty::get(Element& ent) const
{
    ListType list;
    for (auto& entity: m_entities) {
        list.emplace_back(entity.first);
    }
    ent = list;

    return 0;
}

void ContainerAccessProperty::set(const Element& ent)
{
    auto oldEntities = std::move(m_entities);
    if (ent.isList()) {
        for (auto entry: ent.List()) {
            if (entry.isString()) {
                auto I = oldEntities.find(entry.String());
                if (I != oldEntities.end()) {
                    m_entities.emplace(std::move(*I));
                    oldEntities.erase(I);
                } else {
                    auto entity = BaseWorld::instance().getEntity(entry.String());
                    if (entity) {
                        m_entities.emplace(entity->getId(), ObservationEntry{entity});
                    }
                }
            }
        }
    }
    for (auto& entry : oldEntities) {
        if (entry.second.disconnectFn) {
            auto& fn = *entry.second.disconnectFn;
            if (fn) {
                fn();
            }
        }
        std::vector<Atlas::Objects::Root> args;
        for (auto& child : entry.second.observedEntities) {
            Atlas::Objects::Entity::Anonymous anon;
            anon->setId(child->getId());
            args.push_back(std::move(anon));
        }

        Atlas::Objects::Operation::Disappearance disappearance;
        disappearance->setArgs(std::move(args));
        disappearance->setTo(entry.second.observer->getId());
        entry.second.observer->sendWorld(std::move(disappearance)); //Should really be done by the domain entity...
        //entity->sendWorld(std::move(disappearance));
    }
}

ContainerAccessProperty* ContainerAccessProperty::copy() const
{
    return new ContainerAccessProperty(*this);
}



void ContainerAccessProperty::apply(LocatedEntity* entity)
{
    for (auto I = m_entities.begin(); I != m_entities.end();) {
        auto& entry = *I;
        if (!entry.second.disconnectFn) {
            auto& observer = entry.second.observer;
            double reach = 0;
            auto reachProp = observer->getPropertyType<double>("reach");
            if (reachProp) {
                reach = reachProp->data();
            }
            auto observerationCallback = entity->m_location.m_parent->getDomain()->observeCloseness(*observer, *entity, reach, [this, entity, observer]() {
                //TODO: collect observers and send disappear

//                Atlas::Objects::Operation::Look look;
//                look->setFrom(observer->getId());
//                look->setTo(entity->getId());
//                Atlas::Objects::Entity::Anonymous anon;
//                anon->setId(entity->getId());
//                look->setArgs1(std::move(anon));
//                observer->sendWorld(look);

                auto J = m_entities.find(observer->getId());
                if (J != m_entities.end()) {
                    if (!J->second.observedEntities.empty()) {

                        std::vector<Atlas::Objects::Root> args;
                        for (auto& child : J->second.observedEntities) {
                            Atlas::Objects::Entity::Anonymous anon;
                            anon->setId(child->getId());
                            args.push_back(std::move(anon));
                        }

                        Atlas::Objects::Operation::Disappearance disappearance;
                        disappearance->setArgs(std::move(args));
                        disappearance->setTo(observer->getId());
                        entity->sendWorld(std::move(disappearance));
                    }
                    m_entities.erase(J);
                }
//                std::vector<LocatedEntity*> visibleEntities;
//                if (entity->m_contains) {
//                    for (auto& child : *entity->m_contains) {
//                        if (child->isVisibleForOtherEntity(observer.get())) {
//                            visibleEntities.push_back(child.get());
//                        }
//                    }
//                }
//
//                std::vector<Atlas::Objects::Root> args;
//                for (auto& child : visibleEntities) {
//                    if (!child->isVisibleForOtherEntity(observer.get())) {
//                        Atlas::Objects::Entity::Anonymous anon;
//                        anon->setId(child->getId());
//                        args.push_back(std::move(anon));
//                    }
//                }
//                if (!args.empty()) {
//                    Atlas::Objects::Operation::Disappearance disappearance;
//                    disappearance->setArgs(std::move(args));
//                    disappearance->setTo(observer->getId());
//                    entity->sendWorld(std::move(disappearance));
//
//                }

            });
            if (observerationCallback) {
                entry.second.disconnectFn = observerationCallback;
                entity->getDomain()->getVisibleEntitiesFor(*observer, entry.second.observedEntities);
                if (!entry.second.observedEntities.empty()) {
                    std::vector<Atlas::Objects::Root> args;
                    for (auto& child : entry.second.observedEntities) {
                        Atlas::Objects::Entity::Anonymous anon;
                        anon->setId(child->getId());
                        args.push_back(std::move(anon));
                    }

                    Atlas::Objects::Operation::Appearance appearance;
                    appearance->setArgs(std::move(args));
                    appearance->setTo(observer->getId());
                    entity->sendWorld(std::move(appearance));
                }

                ++I;
            } else {
                I = m_entities.erase(I);
            }
        } else {
            ++I;
        }
    }
}

bool ContainerAccessProperty::hasEntity(const LocatedEntity& entity) const
{
    return m_entities.find(entity.getId()) != m_entities.end();
}

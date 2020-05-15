/*
 Copyright (C) 2019 Erik Ogenvik

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

#include "ContainerDomain.h"
#include "rules/LocatedEntity.h"

#include "common/TypeNode.h"
#include "AmountProperty.h"
#include "StackableDomain.h"
#include "ModeDataProperty.h"
#include "ContainersActiveProperty.h"

#include <Atlas/Objects/Anonymous.h>

#include <unordered_set>
#include <common/operations/Update.h>
#include "BaseWorld.h"

ContainerDomain::ContainerDomain(LocatedEntity& entity) :
        Domain(entity),
        mContainerAccessProperty(new ContainerAccessProperty(*this))
{
    entity.makeContainer();
    entity.setProperty(ContainerAccessProperty::property_name, std::unique_ptr<ContainerAccessProperty>(mContainerAccessProperty));
}

void ContainerDomain::addEntity(LocatedEntity& entity)
{
    //Check if entity should be stacked.
    if (StackableDomain::stackIfPossible(m_entity, entity)) {
        return;
    }

    entity.m_location.resetTransformAndMovement();

    entity.removeFlags(entity_clean);

    //Reset any mode_data properties when moving to this domain.
    if (auto prop = entity.modPropertyClassFixed<ModeDataProperty>()) {
        prop->clearData();
    }

    for (auto& entry: m_entities) {
//        if (entry.second.observer->hasFlags(entity_admin) || entity.hasFlags(entity_contained_visible)) {
        entry.second.observedEntities.push_back(&entity);
        //       }
    }

}

void ContainerDomain::removeEntity(LocatedEntity& entity)
{
    //Nothing special to do for this domain.
}

bool ContainerDomain::isEntityVisibleFor(const LocatedEntity& observingEntity, const LocatedEntity& observedEntity) const
{
    //If the observing entity is the same as the one the domain belongs to it can see everything.
    if (&observingEntity == &m_entity) {
        return true;
    }

    if (observingEntity.flags().hasFlags(entity_admin)) {
        return true;
    }

    if (observedEntity.hasFlags(entity_contained_visible)) {
        return true;
    }

    //Entities can only be seen by outside observers if the outside entity can reach this.
    return hasEntity(observingEntity);


    // return observingEntity.canReach(m_entity.m_location);
}

void ContainerDomain::getVisibleEntitiesFor(const LocatedEntity& observingEntity, std::list<LocatedEntity*>& entityList) const
{
    if (observingEntity.canReach(m_entity.m_location)) {
        if (m_entity.m_contains) {
            for (auto& entity : *m_entity.m_contains) {
                if (isEntityVisibleFor(observingEntity, *entity)) {
                    entityList.push_back(entity.get());
                }
            }
        }
    }
}

std::list<LocatedEntity*> ContainerDomain::getObservingEntitiesFor(const LocatedEntity& observedEntity) const
{
    std::list<LocatedEntity*> list;
    for (auto& entry: m_entities) {
        if (entry.second.observer->hasFlags(entity_admin) || observedEntity.hasFlags(entity_contained_visible)) {
            list.push_back(entry.second.observer.get());
        } else {
            if (std::find(entry.second.observedEntities.begin(), entry.second.observedEntities.end(), &observedEntity) != entry.second.observedEntities.end()) {
                list.push_back(entry.second.observer.get());
            }
        }
    }
    if (m_entity.hasFlags(entity_perceptive)) {
        list.push_back(&m_entity);
    }
    return list;
}

bool ContainerDomain::isEntityReachable(const LocatedEntity& reachingEntity, float reach, const LocatedEntity& queriedEntity, const WFMath::Point<3>& positionOnQueriedEntity) const
{
    return hasEntity(reachingEntity);
}

std::vector<Domain::CollisionEntry> ContainerDomain::queryCollision(const WFMath::Ball<3>& sphere) const
{
    std::vector<Domain::CollisionEntry> entries;
    if (m_entity.m_contains) {
        entries.reserve(m_entity.m_contains->size());
        for (auto& child: *m_entity.m_contains) {
            entries.emplace_back(Domain::CollisionEntry{child.get(), WFMath::Point<3>::ZERO(), 0.0f});
        }
    }
    return entries;
}

boost::optional<std::function<void()>> ContainerDomain::observeCloseness(LocatedEntity& reacher, LocatedEntity& target, double reach, std::function<void()> callback)
{
    if (m_entity.m_contains) {
        auto I = std::find_if(m_entity.m_contains->begin(), m_entity.m_contains->end(), [&target](const Ref<LocatedEntity>& child) { return child.get() == &target; });
        if (I != m_entity.m_contains->end()) {
            //TODO: add listeners for when the entity leaves the container
            return boost::optional<std::function<void()>>([this]() {

            });

        }
    }
    return boost::none;
}

void ContainerDomain::setObservers(std::vector<std::string> observerIds)
{
    std::set<std::string> existingObservers;
    for (auto& entry: m_entities) {
        existingObservers.emplace(entry.first);
    }

    for (auto& newId : observerIds) {
        auto I = existingObservers.find(newId);
        if (I != existingObservers.end()) {
            existingObservers.erase(I);
        } else {
            //Doesn't exist, needs to be added
            addObserver(newId);
        }
    }

    //Any ids left should be removed.

    for (auto& id: existingObservers) {
        removeObserver(id);
    }

}

void ContainerDomain::addObserver(std::string& entityId)
{
    auto observer = BaseWorld::instance().getEntity(entityId);
    if (observer) {
        double reach = 0;
        auto reachProp = observer->getPropertyType<double>("reach");
        if (reachProp) {
            reach = reachProp->data();
        }
        auto observerationCallback = m_entity.m_location.m_parent->getDomain()->observeCloseness(*observer, m_entity, reach, [this, observer]() {
            auto J = m_entities.find(observer->getId());
            if (J != m_entities.end()) {
                if (!J->second.observedEntities.empty()) {

                    std::vector<Atlas::Objects::Root> args;
                    for (auto& child : J->second.observedEntities) {
                        Atlas::Objects::Entity::Anonymous anon;
                        anon->setId(child->getId());
                        args.push_back(std::move(anon));
                    }

                    auto containersActiveProperty = observer->requirePropertyClassFixed<ContainersActiveProperty>();
                    containersActiveProperty->getActiveContainers().erase(m_entity.getId());
                    observer->applyProperty(ContainersActiveProperty::property_name, containersActiveProperty);

                    Atlas::Objects::Operation::Update update;
                    update->setTo(observer->getId());
                    observer->sendWorld(std::move(update));


                    Atlas::Objects::Operation::Disappearance disappearance;
                    disappearance->setArgs(std::move(args));
                    disappearance->setTo(observer->getId());
                    m_entity.sendWorld(std::move(disappearance));
                }
                m_entities.erase(J);
            }


        });
        if (observerationCallback) {
            auto containersActiveProperty = observer->requirePropertyClassFixed<ContainersActiveProperty>();
            containersActiveProperty->getActiveContainers().insert(m_entity.getId());
            observer->applyProperty(ContainersActiveProperty::property_name, containersActiveProperty);

            Atlas::Objects::Operation::Update update;
            update->setTo(observer->getId());
            observer->sendWorld(std::move(update));

            auto& entry = m_entities[entityId];
            entry.observer = observer;

            entry.disconnectFn = observerationCallback;
            getVisibleEntitiesFor(*observer, entry.observedEntities);
            if (!entry.observedEntities.empty()) {
                std::vector<Atlas::Objects::Root> args;
                for (auto& child : entry.observedEntities) {
                    Atlas::Objects::Entity::Anonymous anon;
                    anon->setId(child->getId());
                    args.push_back(std::move(anon));
                }

                Atlas::Objects::Operation::Appearance appearance;
                appearance->setArgs(std::move(args));
                appearance->setTo(observer->getId());
                m_entity.sendWorld(std::move(appearance));
            }

        }
    }

}

void ContainerDomain::removeObserver(const std::basic_string<char>& entityId)
{
    auto I = m_entities.find(entityId);
    if (I != m_entities.end()) {
        auto& entry = *I;
        if (entry.second.disconnectFn) {
            auto& fn = *entry.second.disconnectFn;
            if (fn) {
                fn();
            }
        }

        auto& observer = entry.second.observer;

        auto containersActiveProperty = observer->requirePropertyClassFixed<ContainersActiveProperty>();
        containersActiveProperty->getActiveContainers().erase(m_entity.getId());
        observer->applyProperty(ContainersActiveProperty::property_name, containersActiveProperty);

        //Send an update to handle the ContainersActiveProperty being changed.
        Atlas::Objects::Operation::Update update;
        update->setTo(observer->getId());
        observer->sendWorld(std::move(update));

        std::vector<Atlas::Objects::Root> args;
        for (auto& child : entry.second.observedEntities) {
            Atlas::Objects::Entity::Anonymous anon;
            anon->setId(child->getId());
            args.push_back(std::move(anon));
        }

        Atlas::Objects::Operation::Disappearance disappearance;
        disappearance->setArgs(std::move(args));
        disappearance->setTo(entry.second.observer->getId());
        observer->sendWorld(std::move(disappearance)); //Should really be done by the domain entity...
        //entity->sendWorld(std::move(disappearance));

        m_entities.erase(I);
    }
}

bool ContainerDomain::hasEntity(const LocatedEntity& entity) const
{
    return m_entities.find(entity.getId()) != m_entities.end();
}
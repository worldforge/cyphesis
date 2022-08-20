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
#include "BaseWorld.h"

#include <Atlas/Objects/Anonymous.h>

#include <unordered_set>
#include <common/operations/Update.h>
#include <algorithm>
#include "rules/AtlasProperties.h"
#include "rules/PhysicalProperties.h"

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

    //TODO: make these kind of reset actions on the domain being moved out of instead. Like PhysicalDomain.
    //entity.m_location.resetTransformAndMovement();

    entity.removeFlags(entity_clean);

    //Reset any mode_data properties when moving to this domain.
    if (auto prop = entity.modPropertyClassFixed<ModeDataProperty>()) {
        prop->clearData();
    }

    for (auto& entry: m_reachingEntities) {
        entry.second.observedEntities.push_back(&entity);
    }

}

void ContainerDomain::removeEntity(LocatedEntity& entity)
{
    for (auto& entry: m_reachingEntities) {
        entry.second.observedEntities.remove(&entity);
        //Since closeness observations might be updated as part of the callbacks we need to take extra precautions when iterating over them.
        auto& closenessObservations = entry.second.closenessObservations;
        if (!closenessObservations.empty()) {
            std::vector<ClosenessObserverEntry*> toRemove;
            for (auto& observation : closenessObservations) {
                if (&observation->target == &entity) {
                    toRemove.push_back(observation);
                }
            }
            for (auto observation : toRemove) {
                auto I = closenessObservations.find(observation);
                if (I != closenessObservations.end()) {
                    auto J = m_closenessObservations.find(observation);
                    auto observationEntry = std::move(*J);
                    m_closenessObservations.erase(J);
                    observationEntry.second->callback();
                }
            }
        }
    }
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
    return hasObserverRegistered(observingEntity);
}

void ContainerDomain::getVisibleEntitiesFor(const LocatedEntity& observingEntity, std::list<LocatedEntity*>& entityList) const
{
    if (observingEntity.canReach(EntityLocation{&m_entity})) {
        if (m_entity.m_contains) {
            for (auto& entity : *m_entity.m_contains) {
                if (isEntityVisibleFor(observingEntity, *entity)) {
                    entityList.push_back(entity.get());
                }
            }
        }
    }
}

std::vector<LocatedEntity*> ContainerDomain::getObservingEntitiesFor(const LocatedEntity& observedEntity) const
{
    std::vector<LocatedEntity*> list;
    for (auto& entry: m_reachingEntities) {
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
    return hasObserverRegistered(reachingEntity);
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
    if (m_entity.m_contains && hasObserverRegistered(reacher)) {
        auto observerI = m_reachingEntities.find(reacher.getId());
        if (observerI != m_reachingEntities.end()) {
            auto I = std::find_if(m_entity.m_contains->begin(), m_entity.m_contains->end(), [&target](const Ref<LocatedEntity>& child) { return child.get() == &target; });
            if (I != m_entity.m_contains->end()) {
                auto observerEntry = observerI->second;

                auto obs = new ClosenessObserverEntry{reacher.getId(), target, callback};
                observerI->second.closenessObservations.insert(obs);
//                targetEntry->closenessObservations.insert(obs);
                m_closenessObservations.emplace(obs, std::unique_ptr<ClosenessObserverEntry>(obs));
                return boost::optional<std::function<void()>>([this, obs]() {
                    auto J = m_closenessObservations.find(obs);
                    if (J != m_closenessObservations.end()) {
                        auto reacherI = m_reachingEntities.find(J->second->reacherEntityId);
                        if (reacherI != m_reachingEntities.end()) {
                            reacherI->second.closenessObservations.erase(obs);
                        }
//                    targetEntry->closenessObservations.erase(obs);
                        m_closenessObservations.erase(obs);
                    }
                });


            }

        }

    }
    return boost::none;
}

void ContainerDomain::setObservers(std::vector<std::string> observerIds)
{
    std::set<std::string> existingObservers;
    for (auto& entry: m_reachingEntities) {
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

        auto closenessSeveredCallback = [this, entityId]() {
            removeObserver(entityId);
        };

        //We only allow observers that either
        // 1) themselves reside in a Physical Domain, to which the container is a (nested) child
        // 2) are a (nested) parent to the container
        std::vector<std::function<void()>> disconnectFunctions;

        //Handle the simple case where the container and the ancestor have the same parent.
        if (observer->m_parent == m_entity.m_parent) {
            auto parentEntity = m_entity.m_parent;
            if (parentEntity && parentEntity->getDomain()) {
                auto disconnectFn = parentEntity->getDomain()->observeCloseness(*observer, m_entity, reach, closenessSeveredCallback);
                if (disconnectFn) {
                    disconnectFunctions.emplace_back(std::move(*disconnectFn));
                } else {
                    //Could not establish closeness observer, abort.
                    return;
                }
            } else {
                //Either null parent, or the parent had no domain
                return;
            }

        } else {
            //First find the domain which contains the observer, as well as if the observer has a domain itself.
            LocatedEntity* domainEntity = observer->m_parent;
            LocatedEntity* topObserverEntity = observer.get();
            Domain* observerParentDomain = nullptr;

            while (domainEntity != nullptr) {
                if (domainEntity == &m_entity) {
                    //The observer is a child of the container.
                    //TODO: how to handle this?
                    break;
                }

                observerParentDomain = domainEntity->getDomain();
                if (observerParentDomain) {
                    break;
                }
                topObserverEntity = domainEntity;
                domainEntity = domainEntity->m_parent;
            }

            //The parent entity of the observer (possible null), although unlikely.
            auto* observerParentEntity = observer->m_parent;
            //The domain of the observer (possible null).
            Domain* observerOwnDomain = observer->getDomain();

            //Now walk upwards from the container entity until we reach either the observer's parent domain entity,
            //or the observer itself
            std::vector<LocatedEntity*> toAncestors;
            toAncestors.reserve(4); //four seems like a suitable number
            //This will contain the entity which is the common ancestor.
            LocatedEntity* ancestorEntity = &m_entity;
            //This will contain the domain of the common ancestor.
            Domain* ancestorDomain = nullptr;

            while (true) {

                if (ancestorEntity == observer.get()) {
                    ancestorDomain = observerOwnDomain;
                    break;
                }
                if (ancestorEntity == observerParentEntity) {
                    ancestorDomain = observerParentDomain;
                    break;
                }
                if (ancestorEntity == topObserverEntity) {
                    break;
                }
                toAncestors.push_back(ancestorEntity);
                ancestorEntity = ancestorEntity->m_parent;
                if (ancestorEntity == nullptr) {
                    //Could find no common ancestor; can't interact.
                    return;
                }
            }


            LocatedEntity* immediateObserver = observer.get();
            //Now walk back down the toAncestors list, creating closeness checks for each domain
            for (auto I = toAncestors.rbegin(); I != toAncestors.rend(); ++I) {
                LocatedEntity* ancestor = *I;
                if (ancestorDomain) {
                    auto disconnectFn = ancestorDomain->observeCloseness(*immediateObserver, *ancestor, reach, closenessSeveredCallback);
                    if (disconnectFn) {
                        disconnectFunctions.emplace_back(std::move(*disconnectFn));
                    } else {
                        //Could not establish closeness observer, abort.
                        //Clean up first.
                        for (auto& fn : disconnectFunctions) {
                            fn();
                        }
                        return;
                    }
                }
                ancestorDomain = ancestor->getDomain();
            }
        }


        //If we get here we have successfully created closeness observers for all domains between the observer and the container.
        auto& containersActiveProperty = observer->requirePropertyClassFixed<ContainersActiveProperty>();
        containersActiveProperty.getActiveContainers().insert(m_entity.getId());
        observer->applyProperty(containersActiveProperty);

        observer->enqueueUpdateOp();

        auto& entry = m_reachingEntities[entityId];
        entry.observer = observer;

        entry.disconnectFunctions = std::move(disconnectFunctions);
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

void ContainerDomain::removeObserver(const std::basic_string<char>& entityId)
{
    auto I = m_reachingEntities.find(entityId);
    if (I != m_reachingEntities.end()) {
        auto entry = std::move(I->second);
        m_reachingEntities.erase(I);
        for (auto& disconnectFn : entry.disconnectFunctions) {
            if (disconnectFn) {
                disconnectFn();
            }
        }

        auto& observer = entry.observer;

        auto& containersActiveProperty = observer->requirePropertyClassFixed<ContainersActiveProperty>();
        containersActiveProperty.getActiveContainers().erase(m_entity.getId());
        observer->applyProperty(containersActiveProperty);

        //Send an update to handle the ContainersActiveProperty being changed.
        observer->enqueueUpdateOp();

        std::vector<Atlas::Objects::Root> args;
        for (auto& child : entry.observedEntities) {
            Atlas::Objects::Entity::Anonymous anon;
            anon->setId(child->getId());
            args.push_back(std::move(anon));
        }

        Atlas::Objects::Operation::Disappearance disappearance;
        disappearance->setArgs(std::move(args));
        disappearance->setTo(entry.observer->getId());
        m_entity.sendWorld(std::move(disappearance));

        for (auto closenessEntry: entry.closenessObservations) {
            auto J = m_closenessObservations.find(closenessEntry);
            if (J != m_closenessObservations.end()) {
                //Hold on to an instance while we call callbacks and erase it.
                auto observerInstance = std::move(J->second);
                m_closenessObservations.erase(J);
                closenessEntry->callback();
            }
        }
    }
}

bool ContainerDomain::hasObserverRegistered(const LocatedEntity& entity) const
{
    return m_reachingEntities.find(entity.getId()) != m_reachingEntities.end();
}

void ContainerDomain::removed()
{
    while (!m_reachingEntities.empty()) {
        removeObserver(m_reachingEntities.rbegin()->first);
    }
//    //Copy to allow modifications to the field during callbacks.
//    auto entities = std::move(m_reachingEntities);
//    for (auto& entry : entities) {
//        for (auto& disconnectFn : entry.second.disconnectFunctions) {
//            if (disconnectFn) {
//                disconnectFn();
//            }
//        }
//    }
}

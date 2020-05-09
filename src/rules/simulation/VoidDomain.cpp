/*
 Copyright (C) 2014 Erik Ogenvik

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

#include "rules/LocatedEntity.h"
#include "common/operations/Update.h"
#include "VoidDomain.h"
#include "ModeProperty.h"
#include "ModeDataProperty.h"

VoidDomain::VoidDomain(LocatedEntity& entity)
        : Domain(entity)
{
}

bool VoidDomain::isEntityVisibleFor(const LocatedEntity& observingEntity, const LocatedEntity& observedEntity) const
{
    //The entity to which the domain belongs can see its content, otherwise nothing can be seen.
    return &observingEntity == &m_entity;
}

void VoidDomain::getVisibleEntitiesFor(const LocatedEntity& observingEntity,
                                       std::list<LocatedEntity*>& entityList) const
{
    //Can't see anything
}


void VoidDomain::addEntity(LocatedEntity& entity)
{

    entity.m_location.m_pos = WFMath::Point<3>::ZERO();
    entity.m_location.m_orientation = WFMath::Quaternion::IDENTITY();
    entity.m_location.m_velocity = WFMath::Vector<3>::ZERO();
    entity.m_location.m_angularVelocity = WFMath::Vector<3>::ZERO();
    entity.removeFlags(entity_clean);


    //Reset any mode_data properties when moving to this domain.
    if (auto prop = entity.getPropertyClassFixed<ModeDataProperty>()) {
        if (prop->getMode() != ModeProperty::Mode::Unknown) {
            entity.setAttrValue(ModeDataProperty::property_name, Atlas::Message::Element());
            Atlas::Objects::Operation::Update update;
            update->setTo(entity.getId());
            entity.sendWorld(update);
        }
    }

}

void VoidDomain::removeEntity(LocatedEntity& entity)
{

}

bool VoidDomain::isEntityReachable(const LocatedEntity& reachingEntity, float reach, const LocatedEntity& queriedEntity, const WFMath::Point<3>& positionOnQueriedEntity) const
{
    return &reachingEntity == &m_entity;
}

boost::optional<std::function<void()>> VoidDomain::observeCloseness(LocatedEntity& reacher, LocatedEntity& target, double reach, std::function<void()> callback)
{
    return boost::none;
}


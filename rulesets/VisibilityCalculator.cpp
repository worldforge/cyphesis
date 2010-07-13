// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2010 Erik Hjortsberg <erik.hjortsberg@gmail.com>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

#include "VisibilityCalculator.h"
#include "Entity.h"
#include "modules/Location.h"
#include "rulesets/VisDistProperty.h"
#include "common/const.h"


VisibilityCalculator::VisibilityCalculator(const LocatedEntity& located_entity)
: m_entity(located_entity), m_entity_visibility_distance_square(-1)
{
    //If the entity has a visibility distance property set, we'll use that
    //for determining what other entities perceive this.
    //If not, we'll do a check against the size of the bounding box.
    //To increase performance we'll check if the property is set here, and
    //cache the value.
    const Entity* entity = dynamic_cast<const Entity*>(&located_entity);
    if (entity && entity->getFlags() & entity_visdist) {
        const VisDistProperty* vis_dist_prop =
                static_cast<const VisDistProperty*>
                (m_entity.getProperty("visdist"));
        double visibility_distance = vis_dist_prop->data();
        m_entity_visibility_distance_square = visibility_distance * visibility_distance;
    }
}


bool VisibilityCalculator::isEntityVisibleFor(const LocatedEntity& observing_entity)
{
    if (m_entity_visibility_distance_square > -1) {
        // Calculate square distance to target
        return squareDistance(m_entity.m_location, observing_entity.m_location) <
                m_entity_visibility_distance_square;
    } else {
        // Where broadcasts go depends on type of op
        float fromSquSize = m_entity.m_location.squareBoxSize();
        // Calculate square distance to target
        float dist = squareDistance(m_entity.m_location,
                observing_entity.m_location);
        float view_factor = fromSquSize / dist;
        if (view_factor > consts::square_sight_factor) {
            return true;
        }
    }
    return false;
}

unsigned int VisibilityCalculator::calculateMovementChanges(
        const LocatedEntity& observing_entity,
        const Point3D& old_entity_pos)
{
    unsigned int result = 0;
    float old_dist = squareDistance(observing_entity.m_location.pos(),
            old_entity_pos),
          new_dist = squareDistance(observing_entity.m_location.pos(),
                  m_entity.m_location.pos()),
          squ_size = observing_entity.m_location.squareBoxSize();

    const Entity * viewer = dynamic_cast<const Entity *>(&observing_entity);
    assert(viewer != 0);
    if (viewer->isPerceptive()) {
        if (m_entity_visibility_distance_square > -1) {
            bool was_in_range = old_dist < m_entity_visibility_distance_square;
            bool is_in_range = new_dist < m_entity_visibility_distance_square;

            if (was_in_range) {
                result |= was_seeing;
            } else {
                result |= wasnt_seeing;
            }
            if (is_in_range) {
                result |= is_seeing;
            } else {
                result |= isnt_seeing;
            }

        } else {
            float fromSquSize = m_entity.m_location.squareBoxSize();
            bool was_in_range = ((fromSquSize / old_dist) > consts::square_sight_factor),
                 is_in_range = ((fromSquSize / new_dist) > consts::square_sight_factor);

            if (was_in_range) {
                result |= was_seeing;
            } else {
                result |= wasnt_seeing;
            }
            if (is_in_range) {
                result |= is_seeing;
            } else {
                result |= isnt_seeing;
            }
        }
    }

    //Check if the observer entity has the "visdist" property set. If so, we'll
    //use that to determine whether the current entity can see the observer.
    if (viewer->getFlags() & entity_visdist) {
        const VisDistProperty* vis_dist_prop =
                static_cast<const VisDistProperty*>
                (viewer->getProperty("visdist"));
        double visibility_distance_squ = vis_dist_prop->data() * vis_dist_prop->data();

        bool could_see = old_dist < visibility_distance_squ;
        bool can_see = new_dist < visibility_distance_squ;

        if (could_see) {
            result |= was_seen;
        } else {
            result |= was_unseen;
        }
        if (can_see) {
            result |= is_seen;
        } else {
            result |= is_unseen;
        }
    } else {
        bool could_see = ((squ_size / old_dist) > consts::square_sight_factor),
             can_see = ((squ_size / new_dist) > consts::square_sight_factor);
        if (could_see) {
            result |= was_seen;
        } else {
            result |= was_unseen;
        }
        if (can_see) {
            result |= is_seen;
        } else {
            result |= is_unseen;
        }
    }
    return result;
}


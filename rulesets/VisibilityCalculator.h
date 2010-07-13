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

#ifndef VISIBILITYCALCULATOR_H_
#define VISIBILITYCALCULATOR_H_

#include "physics/Vector3D.h"

class LocatedEntity;

/// \brief Used for calculating visibility for entities.
///
/// This is kept separate from the Entity class to better allow for different
/// visibility algorithms.
/// To improve performance an instance of this is created for each entity
/// you wish to calculate visibility for. This is because it allows us to cache
/// values which might otherwise be expensive to lookup.
///
/// When calculating visibility we'll first check if there's a "visdist" value
/// set. If so, this value determines the distance for when the entity is
/// visible.
/// If no such property is set, visibility will instead be determined by
/// checking the size of the bounding box of the entity.
class VisibilityCalculator
{
    public:

        /// \brief The observing entity was seen by the observed entity.
        static const unsigned int was_seen = 1 << 1;
        /// \brief The observing entity wasn't seen by the observed entity.
        static const unsigned int was_unseen = 1 << 2;
        /// \brief The observing entity is seen by the observed entity.
        static const unsigned int is_seen = 1 << 3;
        /// \brief The observing entity isn't seen by the observed entity.
        static const unsigned int is_unseen = 1 << 4;
        /// \brief The observed entity was seen by the observing entity.
        static const unsigned int was_seeing = 1 << 5;
        /// \brief The observed entity wasn't seen by the observing entity.
        static const unsigned int wasnt_seeing = 1 << 6;
        /// \brief The observed entity is seen by the observing entity.
        static const unsigned int is_seeing = 1 << 7;
        /// \brief The observed entity isn't seen by the observing entity.
        static const unsigned int isnt_seeing = 1 << 8;

        /// \brief Ctor.
        ///
        /// \param locatedEntity The entity which we want to check if other
        ///                         entities can see.
        VisibilityCalculator(const LocatedEntity& located_entity);

        /// \brief Checks whether the current entity can be seen by the passed
        ///         in entity.
        ///
        /// \param observingEntity The observing entity, for which we want to
        ///                         test whether it can observe the current
        ///                         entity.
        bool isEntityVisibleFor(const LocatedEntity& observing_entity);

        /// \brief Calculates any visibility changes which are a result of an
        ///         entity moving in the world.
        ///
        /// \param observingEntity The observing entity.
        /// \param oldEntityPos The old position of the current entity.
        /// \returns A combination of the visibility flags defined in this
        ///             class.
        unsigned int calculateMovementChanges(const LocatedEntity&
                observing_entity, const Point3D& old_entity_pos);

    private:

        /// \brief The entity which other entities will be observing.
        const LocatedEntity& m_entity;

        /// \brief If the "visdist" property is set, this is that value
        ///         squared value.
        ///
        /// If no "visdist" property is set this will be -1.
        double m_entity_visibility_distance_square;
};

#endif /* VISIBILITYCALCULATOR_H_ */

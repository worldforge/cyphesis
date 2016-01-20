// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2016 Erik Ogenvik
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

#ifndef RULESETS_FORCESPROPERTY_H_
#define RULESETS_FORCESPROPERTY_H_

#include "common/Property.h"
#include "physics/Transform.h"

#include <wfmath/vector.h>
#include <wfmath/quaternion.h>

/**
 * \brief Handles forces applied to the entity.
 *
 * Various forces can be applied to the entity, by either other entities or other components.
 * These are all specified in this property.
 *
 * It's expected that the keys used for entries in this map should be selected so that each
 * entry is easily identified as coming from a certain entity or component. For entities
 * one should thus use the entity id as key.
 *
 * \ingroup PropertyClasses
 */
class ForcesProperty: public PropertyBase {
    public:
        static const std::string property_name;

        ForcesProperty();

        /**
         * Accessor for the map of external forces.
         */
        std::map<std::string, WFMath::Vector<3>>& forces();
        /**
         * Accessor for the map of external forces.
         */
        const std::map<std::string, WFMath::Vector<3>>& forces() const;

        virtual int get(Atlas::Message::Element &) const;
        virtual void set(const Atlas::Message::Element &);
        virtual ForcesProperty * copy() const;

    protected:

        std::map<std::string, WFMath::Vector<3>> mForces;
};

#endif /* RULESETS_FORCESPROPERTY_H_ */

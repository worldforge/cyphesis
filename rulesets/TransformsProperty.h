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

#ifndef RULESETS_TRANSFORMSPROPERTY_H_
#define RULESETS_TRANSFORMSPROPERTY_H_

#include "common/Property.h"
#include "physics/Transform.h"

#include <wfmath/vector.h>
#include <wfmath/quaternion.h>

/**
 * \brief Handles transformations applied to the entity.
 *
 * The final position and orientation of the entity, as contained in Location,
 * is calculated from these values. By keeping the various transformations separate
 * it's possible to undo or separate them when needed.
 *
 * This property will foremost contain transformations set on the entity itself,
 * contained in mTranslate, mRotate and mTranslateScaled.
 *
 * In addition, external entities and components have the ability to add transformations
 * through the "mExternal" map. It's expected that the keys used for entries in this map
 * should be selected so that each entry is easily identified without any collisions.
 *
 * \ingroup PropertyClasses
 */
class TransformsProperty: public PropertyBase {
    public:
        static const std::string property_name;

        TransformsProperty();

        /**
         * Accessor for the map of external transformations.
         */
        std::map<std::string, Transform>& external();
        /**
         * Accessor for the map of external transformations.
         */
        const std::map<std::string, Transform>& external() const;

        /**
         * Gets the entity owned translation.
         */
        WFMath::Vector<3>& getTranslate();

        /**
         * Gets the entity owned translation.
         */
        const WFMath::Vector<3>& getTranslate() const;

        /**
         * Gets the entity owned rotation.
         */
        WFMath::Quaternion& getRotate();

        /**
         * Gets the entity owned rotation.
         */
        const WFMath::Quaternion& getRotate() const;

        /**
         * Gets the entity owned scaled translation.
         * This translation is calculated using the size of the entity. The values are normalized.
         */
        WFMath::Vector<3>& getTranslateScaled();

        /**
         * Gets the entity owned scaled translation.
         * This translation is calculated using the size of the entity. The values are normalized.
         */
        const WFMath::Vector<3>& getTranslateScaled() const;

        virtual void apply(LocatedEntity *);
        virtual int get(Atlas::Message::Element &) const;
        virtual void set(const Atlas::Message::Element &);
        virtual TransformsProperty * copy() const;

    protected:
        Transform mTransform;

        std::map<std::string, Transform> mExternal;
};

#endif /* RULESETS_TRANSFORMSPROPERTY_H_ */

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

#ifndef RULESETS_MODEPROPERTY_H_
#define RULESETS_MODEPROPERTY_H_

#include "common/Property.h"

/**
 * \brief Defines the physical mode of the entity.
 *
 * The "mode" determines how the entity interacts with other entities.
 * For entities that shouldn't be moved, use "planted" or "fixed".
 * For entities that should react to physics, use "free".
 * \ingroup PropertyClasses
 */
class ModeProperty : public PropertyBase
{
    public:

        enum class Mode
        {
                /**
                 * Planted entities are stuck to the terrain. Their y-position is determined by the terrain at their origo.
                 * They can optionally also have an offset, specified in "planted_offset" or "planted_scaled_offset".
                 * Planted entities are not affected by physics, but other free entities will react with them.
                 * If an entity is planted on a water body, it's considered to be "floating", i.e. it will be planted on top
                 * of the water body.
                 * The attribute "mode_data" contains a reference to the other entity on which an entity is planted on.
                 */
                    Planted,

                /**
                 * Fixed entities are fixed in the world. They are not affected by terrain.
                 * Fixed entities are not affected by physics, but other free entities will react with them.
                 */
                    Fixed,

                /**
                 * Free entities are handled by the physics engine.
                 */
                    Free,

                /**
                 * Submerged entities are like Free, but are submerged in liquid (such as an ocean).
                 */
                    Submerged,
                /**
                 * Like a free entity, but will emit Hit operations when it hits another entity.
                 *
                 * The "mode_data" property can be used to keep information about which entity caused the projectile to get moving.
                 */
                    Projectile,
                /**
                 * This mode is used when the mode string isn't recognized.
                 */
                    Unknown

        };

        static constexpr const char* property_name = "mode";
        static constexpr const char* property_atlastype = "string";

        ModeProperty();

        ~ModeProperty() override = default;

        void apply(LocatedEntity&) override;

        ModeProperty* copy() const override;

        void set(const Atlas::Message::Element& val) override;

        int get(Atlas::Message::Element & val) const override;

        ModeProperty::Mode getMode() const
        {
            return m_mode;
        }

        std::string data() const;

        static ModeProperty::Mode parseMode(const std::string& mode);
        static std::string encodeMode(ModeProperty::Mode mode);

    protected:
        ModeProperty(const ModeProperty& rhs) = default;
        Mode m_mode;

};

#endif /* RULESETS_MODEPROPERTY_H_ */

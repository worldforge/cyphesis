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

#ifndef CYPHESIS_MODEDATAPROPERTY_H
#define CYPHESIS_MODEDATAPROPERTY_H

#include <boost/variant.hpp>
#include <modules/WeakEntityRef.h>
#include <boost/optional.hpp>
#include "common/Property.h"
#include "ModeProperty.h"

/**
 * Contains extra data for the "mode" of the entity.
 * Currently it can contain data specific to "planted" entities (specifically on which entity they are planted)
 * as well as data for "projectile" entities (specifically which actor that fired them as well as any extra data such as "damage").
 *
 * \ingroup PropertyClasses
 */
class ModeDataProperty : public PropertyBase
{
    public:

        static constexpr const char* property_name = "mode_data";
        static constexpr const char* property_atlastype = "map";

        struct NullData
        {
        };

        struct PlantedOnData
        {
            /**
             * The entity on to which this entity is attached.
             */
            boost::optional<long> entityId;

            /**
             * An optional attachment.
             */
            boost::optional<std::string> attachment;

            /**
             * Returns true if either "entity" or "attachment" is set.
             * @return
             */
            explicit operator bool() const
            {
                return (entityId || attachment);
            }
        };

        struct ProjectileData
        {
            Ref<LocatedEntity> entity;

            Atlas::Message::MapType extra;
        };

        ModeDataProperty();

        ~ModeDataProperty() override = default;

        ModeDataProperty::PlantedOnData& getPlantedOnData();

        const ModeDataProperty::PlantedOnData& getPlantedOnData() const;

        ModeDataProperty::ProjectileData& getProjectileData();

        const ModeDataProperty::ProjectileData& getProjectileData() const;

        ModeProperty::Mode getMode() const;

        ModeDataProperty* copy() const override;

        void set(const Atlas::Message::Element& val) override;

        int get(Atlas::Message::Element& val) const override;

        static ModeDataProperty::PlantedOnData parsePlantedData(const Atlas::Message::MapType& map);

        void setPlantedData(const Atlas::Message::MapType& map);

        void setPlantedData(ModeDataProperty::PlantedOnData data);

        void setProjectileData(const Atlas::Message::MapType& map);

        void setProjectileData(ModeDataProperty::ProjectileData data);

        void clearData();

    protected:
        ModeDataProperty(const ModeDataProperty& rhs) = default;


        typedef boost::variant<NullData, PlantedOnData, ProjectileData> ModeData;

        ModeData mData;

        ModeProperty::Mode mMode;


};


#endif //CYPHESIS_MODEDATAPROPERTY_H

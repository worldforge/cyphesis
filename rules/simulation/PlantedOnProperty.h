/*
 Copyright (C) 2018 Erik Ogenvik

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

#ifndef CYPHESIS_PLANTEDONPROPERTY_H
#define CYPHESIS_PLANTEDONPROPERTY_H


#include <boost/optional.hpp>
#include "modules/WeakEntityRef.h"
#include "common/Property.h"

/**
 * Used on entities that are planted on other entities.
 *
 * The property consists of both an Entity reference as well as a string "attachment".
 * The usage of these values is domain specific.
 */
class PlantedOnProperty : public PropertyBase
{
    public:

        struct Data
        {
            /**
             * The entity on to which this entity is attached.
             */
            WeakEntityRef entity;

            /**
             * An optional attachment.
             */
            boost::optional<std::string> attachment;

            /**
             * Returns true if either "entity" or "attachment" is set.
             * @return
             */
            constexpr explicit operator bool() const
            {
                return (entity || attachment);
            }
        };

        static constexpr const char* property_atlastype = "map";
        static constexpr const char* property_name = "planted_on";

        explicit PlantedOnProperty() = default;

        int get(Atlas::Message::Element& val) const override;

        void set(const Atlas::Message::Element& val) override;

        void add(const std::string& val, Atlas::Message::MapType& map) const override;

        void add(const std::string& val, const Atlas::Objects::Entity::RootEntity& ent) const override;

        PlantedOnProperty* copy() const override;

        const Data& data() const;

        Data& data();

        static Data parse(const Atlas::Message::Element& val);

    protected:

        Data m_data;


};


#endif //CYPHESIS_PLANTEDONPROPERTY_H

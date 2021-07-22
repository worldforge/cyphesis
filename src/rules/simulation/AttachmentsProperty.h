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

#ifndef CYPHESIS_ATTACHMENTSPROPERTY_H
#define CYPHESIS_ATTACHMENTSPROPERTY_H

#include "common/Property.h"
#include "rules/entityfilter/Filter.h"
#include "modules/Ref.h"

#include <memory>

/**
 * Handles attachments on an entity.
 *
 * This property defines the various attachments available, with optional Entity Filter constraints.
 * When entities are attached new "attached_*" soft properties are created with references to the
 * entities. The "mode_data" property of the attached entity is also updated.
 *
 * To attach something a Wield op needs to be sent. This property will take care of listening to it
 * and making the required changes on all involved entities.
 * \ingroup PropertyClasses
 */
class AttachmentsProperty : public PropertyBase
{
    public:

        static constexpr const char* property_atlastype = "map";
        static constexpr const char* property_name = "attachments";


        struct Attachment
        {
            std::string contraint;
            std::unique_ptr<EntityFilter::Filter> filter;
        };

        explicit AttachmentsProperty(std::uint32_t flags = 0);

        AttachmentsProperty(const AttachmentsProperty& rhs) = delete;

        void install(LocatedEntity&, const std::string&) override;

        void remove(LocatedEntity&, const std::string& name) override;

        HandlerResult operation(LocatedEntity&,
                                const Operation&,
                                OpVector&) override;

        int get(Atlas::Message::Element& val) const override;

        void set(const Atlas::Message::Element& val) override;


    protected:

        std::map<std::string, Attachment> m_data;

        AttachmentsProperty* copy() const override;

        static Ref<LocatedEntity> extractEntityRef(const Atlas::Message::Element& val);

};


#endif //CYPHESIS_ATTACHMENTSPROPERTY_H

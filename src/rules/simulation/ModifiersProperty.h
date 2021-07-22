/*
 Copyright (C) 2020 Erik Ogenvik

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

#ifndef CYPHESIS_MODIFIERSPROPERTY_H
#define CYPHESIS_MODIFIERSPROPERTY_H

#include "common/Property.h"

/**
 * Exposes the current active modifiers as a protected "_modifiers" attribute.
 * This is a read only property which can only be applied on an instance.
 * \ingroup PropertyClasses
 */
class ModifiersProperty : public PropertyBase
{
    public:
        static constexpr const char* property_name = "_modifiers";
        static constexpr const char* property_atlastype = "list";

        ModifiersProperty();

        ~ModifiersProperty() override = default;

        ModifiersProperty* copy() const override;

         void install(LocatedEntity&, const std::string &) override;

        void set(const Atlas::Message::Element& val) override;

        int get(Atlas::Message::Element& val) const override;


    protected:
        ModifiersProperty(const ModifiersProperty& rhs) = default;

        LocatedEntity* m_entity;


};


#endif //CYPHESIS_MODIFIERSPROPERTY_H

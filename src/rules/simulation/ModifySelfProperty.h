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

#ifndef CYPHESIS_MODIFYSELFPROPERTY_H
#define CYPHESIS_MODIFYSELFPROPERTY_H


#include "ModifyProperty.h"

class ModifySelfProperty : public PropertyBase
{
    public:
        static constexpr const char* property_name = "modify_self";
        static constexpr const char* property_atlastype = "map";

        ModifySelfProperty();

        ModifySelfProperty(const ModifySelfProperty& rhs);

        ~ModifySelfProperty() override = default;


        void remove(LocatedEntity&, const std::string& name) override;

        void install(LocatedEntity& owner, const std::string& name) override;

        ModifySelfProperty* copy() const override;

        void set(const Atlas::Message::Element& val) override;

        int get(Atlas::Message::Element& val) const override;

        void apply(LocatedEntity& e) override;

    protected:

        std::map<std::string, ModifyEntry> m_modifyEntries;
        Atlas::Message::Element m_data;

        /**
       * Entity specific state. Since we need to listen to changes in properties we need to keep track of connections for these signals.
       */
        struct State
        {
            sigc::connection updatedConnection;
        };

        static PropertyInstanceState<State> sInstanceState;

        void setData(const Atlas::Message::Element& val);

        void checkIfActive(LocatedEntity& entity);

};


#endif //CYPHESIS_MODIFYSELFPROPERTY_H

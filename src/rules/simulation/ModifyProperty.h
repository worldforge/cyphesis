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

#ifndef CYPHESIS_MODIFYPROPERTY_H
#define CYPHESIS_MODIFYPROPERTY_H

#include <rules/entityfilter/Filter.h>
#include <rules/Modifier.h>
#include <sigc++/connection.h>
#include <common/PropertyInstanceState.h>
#include "common/Property.h"
#include <set>


/**
* Containing information about how to modify a parent.
*/
struct ModifyEntry
{
    /**
     * An optional constraint.
     */
    std::unique_ptr<EntityFilter::Filter> constraint;

    /**
     * The modifiers which will be applied.
     */
    std::map<std::string, std::unique_ptr<Modifier>> modifiers;
    /**
     * A list of observed properties of the parent entity. Whenever one of the properties changes the modification should be re-evaluated.
     */
    std::set<std::string> observedProperties;

    static ModifyEntry parseEntry(const Atlas::Message::MapType& observedPropertiesEntry);

};

/**
 * A property which allows an entity to modify properties of a parent entity.
 *
 * When such an entity is added as a child to another entity the modifiers defined can be applied, if the constraints are fulfilled.
 * This allows us to model various effects, such as armor and weapons affecting the character which wears them.
 * \ingroup PropertyClasses
 */
class ModifyProperty : public PropertyBase
{
    public:
        static constexpr const char* property_name = "modify";
        static constexpr const char* property_atlastype = "list";

        ModifyProperty();

        ModifyProperty(const ModifyProperty& rhs);

        ~ModifyProperty() override = default;

        void apply(LocatedEntity&) override;

        void remove(LocatedEntity&, const std::string& name) override;

        void install(LocatedEntity& owner, const std::string& name) override;

        ModifyProperty* copy() const override;

        void set(const Atlas::Message::Element& val) override;

        int get(Atlas::Message::Element& val) const override;


    protected:


        std::vector<ModifyEntry> m_modifyEntries;

        /**
         * Entity specific state. Since we need to listen to both changes in properties as well as in the parent location we need to also keep track of connections for these signals.
         */
        struct State
        {
            sigc::connection containeredConnection;
            sigc::connection parentEntityPropertyUpdateConnection;
            LocatedEntity* parentEntity; //Need to keep track of latest parent as there's no specific signal for when location changes.
        };

        static PropertyInstanceState<State> sInstanceState;

        Atlas::Message::Element m_data;

        void newLocation(State& state, LocatedEntity& entity, LocatedEntity* parent);

        void checkIfActive(State& state, LocatedEntity& entity);

        void setData(const Atlas::Message::Element& val);

};


#endif //CYPHESIS_MODIFYPROPERTY_H

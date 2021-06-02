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

#include <rules/entityfilter/Providers.h>
#include "ModifySelfProperty.h"
#include "rules/LocatedEntity.h"
#include "BaseWorld.h"
#include "common/Inheritance.h"
#include "ModifiersProperty.h"

PropertyInstanceState<ModifySelfProperty::State> ModifySelfProperty::sInstanceState;

ModifySelfProperty::ModifySelfProperty() = default;

ModifySelfProperty::ModifySelfProperty(const ModifySelfProperty& rhs)
        : PropertyBase(rhs)
{
    setData(rhs.m_data);
}

void ModifySelfProperty::apply(LocatedEntity& entity)
{
    //Whenever a the value is changed and the property is applied we need to clear out all applied modifiers.
    auto& activeModifiers = entity.getActiveModifiers();
    auto I = activeModifiers.find(&entity);
    if (I != activeModifiers.end()) {
        auto modifiers = I->second;
        for (auto& entry: modifiers) {
            //Note that the modifier pointer points to an invalid memory location! We can only remove it; not touch it otherwise.
            entity.removeModifier(entry.first, entry.second);
        }
    }

    checkIfActive(entity);

}

void ModifySelfProperty::remove(LocatedEntity& owner, const std::string& name)
{
    checkIfActive(owner);
    auto* state = sInstanceState.getState(owner);
    if (state) {
        state->updatedConnection.disconnect();
    }
    sInstanceState.removeState(owner);
}

void ModifySelfProperty::install(LocatedEntity& owner, const std::string& name)
{
    auto state = std::make_unique<ModifySelfProperty::State>();
    checkIfActive(owner);

    sInstanceState.addState(owner, std::move(state));
}

ModifySelfProperty* ModifySelfProperty::copy() const
{
    return new ModifySelfProperty(*this);
}

void ModifySelfProperty::setData(const Atlas::Message::Element& val)
{
    m_data = val;
    m_modifyEntries.clear();

    if (val.isMap()) {
        auto& map = val.Map();
        for (auto& entry : map) {
            if (entry.second.isMap()) {
                auto modifyEntry = ModifyEntry::parseEntry(entry.second.Map());
                if (!modifyEntry.modifiers.empty()) {
                    m_modifyEntries.emplace(entry.first, std::move(modifyEntry));
                }
            }
        }
    }
}


void ModifySelfProperty::set(const Atlas::Message::Element& val)
{
    setData(val);
}

int ModifySelfProperty::get(Atlas::Message::Element& val) const
{
    val = m_data;
    return 0;
}

void ModifySelfProperty::checkIfActive(LocatedEntity& entity)
{
    std::set<std::pair<std::string, Modifier*>> activatedModifiers;
    for (auto& entry: m_modifyEntries) {
        auto& modifyEntry = entry.second;
        bool apply;
        if (!modifyEntry.constraint) {
            apply = true;
        } else {
            EntityFilter::QueryContext queryContext{entity, &entity};
            queryContext.entity_lookup_fn = [](const std::string& id) { return BaseWorld::instance().getEntity(id); };
            queryContext.type_lookup_fn = [](const std::string& id) { return Inheritance::instance().getType(id); };
            apply = modifyEntry.constraint->match(queryContext);
        }

        if (apply) {
            for (auto& appliedModifierEntry : modifyEntry.modifiers) {
                activatedModifiers.emplace(appliedModifierEntry.first, appliedModifierEntry.second.get());
            }
        }
    }

    auto& activeModifiers = entity.getActiveModifiers();
    auto I = activeModifiers.find(&entity);
    if (I != activeModifiers.end()) {
        //There were already modifiers active. Check the difference and add or remove.

        auto modifiersCopy = I->second;
        for (auto& appliedModifierEntry : activatedModifiers) {
            auto pair = std::make_pair(appliedModifierEntry.first, appliedModifierEntry.second);
            if (modifiersCopy.find(pair) == modifiersCopy.end()) {
                entity.addModifier(appliedModifierEntry.first, appliedModifierEntry.second, &entity);
                entity.requirePropertyClassFixed<ModifiersProperty>().addFlags(prop_flag_unsent);
            } else {
                modifiersCopy.erase(pair);
            }
        }

        //Those that are left should be removed.
        for (auto& modifierToRemove : modifiersCopy) {
            entity.removeModifier(modifierToRemove.first, modifierToRemove.second);
        }

    } else {
        if (!activatedModifiers.empty()) {
            //No active modifiers existed, just add all
            for (auto& appliedModifierEntry : activatedModifiers) {
                entity.addModifier(appliedModifierEntry.first, appliedModifierEntry.second, &entity);
            }
            entity.requirePropertyClassFixed<ModifiersProperty>().addFlags(prop_flag_unsent);
        }
    }
}




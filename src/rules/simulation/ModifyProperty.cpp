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

#include "common/Inheritance.h"
#include "common/AtlasQuery.h"
#include "ModifyProperty.h"
#include "rules/entityfilter/ProviderFactory.h"
#include "BaseWorld.h"
#include "ModifiersProperty.h"


PropertyInstanceState<ModifyProperty::State> ModifyProperty::sInstanceState;

ModifyEntry ModifyEntry::parseEntry(const Atlas::Message::MapType& entryMap)
{
    ModifyEntry modifyEntry;
    AtlasQuery::find<std::string>(entryMap, "constraint", [&](const std::string& constraintEntry) {
        modifyEntry.constraint = std::make_unique<EntityFilter::Filter>(constraintEntry, EntityFilter::ProviderFactory());
    });
    AtlasQuery::find<Atlas::Message::ListType>(entryMap, "observed_properties", [&](const Atlas::Message::ListType& observedPropertiesEntry) {
        for (auto& propertyEntry: observedPropertiesEntry) {
            if (propertyEntry.isString()) {
                modifyEntry.observedProperties.emplace(propertyEntry.String());
            }
        }
    });

    AtlasQuery::find<Atlas::Message::MapType>(entryMap, "modifiers", [&](const Atlas::Message::MapType& modifiersMap) {
        for (auto& modifierEntry : modifiersMap) {
            if (modifierEntry.second.isMap()) {
                auto& valueMap = modifierEntry.second.Map();
                auto I = valueMap.find("default");
                if (I != valueMap.end()) {
                    modifyEntry.modifiers.emplace(modifierEntry.first, std::make_unique<DefaultModifier>(I->second));
                }
                I = valueMap.find("append");
                if (I != valueMap.end()) {
                    modifyEntry.modifiers.emplace(modifierEntry.first, std::make_unique<AppendModifier>(I->second));
                }
                I = valueMap.find("prepend");
                if (I != valueMap.end()) {
                    modifyEntry.modifiers.emplace(modifierEntry.first, std::make_unique<PrependModifier>(I->second));
                }
                I = valueMap.find("subtract");
                if (I != valueMap.end()) {
                    modifyEntry.modifiers.emplace(modifierEntry.first, std::make_unique<SubtractModifier>(I->second));
                }
                I = valueMap.find("add_fraction");
                if (I != valueMap.end()) {
                    modifyEntry.modifiers.emplace(modifierEntry.first, std::make_unique<AddFractionModifier>(I->second));
                }
            }
        }
    });
    return modifyEntry;
}

ModifyProperty::ModifyProperty() = default;

ModifyProperty::ModifyProperty(const ModifyProperty& rhs)
        : PropertyBase(rhs)
{
    setData(rhs.m_data);
}


void ModifyProperty::apply(LocatedEntity& entity)
{
    auto* state = sInstanceState.getState(entity);
    //Whenever the value is changed and the property is applied we need to clear out all applied modifiers.
    if (state->parentEntity) {
        auto& activeModifiers = state->parentEntity->getActiveModifiers();
        auto I = activeModifiers.find(&entity);
        if (I != activeModifiers.end()) {
            auto modifiers = I->second;
            //Important that we copy the modifiers since we'll be modifying them.
            for (auto& entry: modifiers) {
                //Note that the modifier pointer points to an invalid memory location! We can only remove it; not touch it otherwise.
                state->parentEntity->removeModifier(entry.first, entry.second);
            }
        }
    }

    checkIfActive(*state, entity);
}


ModifyProperty* ModifyProperty::copy() const
{
    return new ModifyProperty(*this);
}


void ModifyProperty::setData(const Atlas::Message::Element& val)
{
    m_data = val;
    m_modifyEntries.clear();

    if (val.isList()) {
        auto& list = val.List();
        for (auto& entry : list) {
            if (entry.isMap()) {
                auto modifyEntry = ModifyEntry::parseEntry(entry.Map());

                if (!modifyEntry.modifiers.empty()) {
                    m_modifyEntries.emplace_back(std::move(modifyEntry));
                }
            }
        }
    }
}


void ModifyProperty::set(const Atlas::Message::Element& val)
{
    setData(val);
}

int ModifyProperty::get(Atlas::Message::Element& val) const
{
    val = m_data;
    return 0;
}

void ModifyProperty::remove(LocatedEntity& owner, const std::string& name)
{
    auto* state = sInstanceState.getState(owner);
    if (state) {
        newLocation(*state, owner, nullptr);
        state->containeredConnection.disconnect();
    }
    sInstanceState.removeState(owner);
}

void ModifyProperty::install(LocatedEntity& owner, const std::string& name)
{
    auto state = new ModifyProperty::State();
    state->parentEntity = owner.m_parent;
    state->containeredConnection.disconnect();
    state->containeredConnection = owner.containered.connect([this, state, &owner](const Ref<LocatedEntity>& oldLocation) {
        if (state->parentEntity != owner.m_parent) {
            newLocation(*state, owner, owner.m_parent);
            checkIfActive(*state, owner);
        }
    });
    if (owner.m_parent) {
        newLocation(*state, owner, owner.m_parent);
    }

    sInstanceState.addState(owner, std::unique_ptr<ModifyProperty::State>(state));
}


void ModifyProperty::newLocation(State& state, LocatedEntity& entity, LocatedEntity* parent)
{
    if (state.parentEntity) {
        auto& activeModifiers = state.parentEntity->getActiveModifiers();
        auto I = activeModifiers.find(&entity);
        if (I != activeModifiers.end() && !I->second.empty()) {
            //Important that we copy the modifiers, since we'll be altering the map.
            auto modifiers = I->second;
            for (auto& entry: modifiers) {
                state.parentEntity->removeModifier(entry.first, entry.second);
            }
            state.parentEntity->requirePropertyClassFixed<ModifiersProperty>().addFlags(prop_flag_unsent);
        }
    }
    state.parentEntity = parent;
    state.parentEntityPropertyUpdateConnection.disconnect();
    if (state.parentEntity) {
        state.parentEntityPropertyUpdateConnection = state.parentEntity->propertyApplied.connect(
                [&](const std::string& propertyName, const PropertyBase&) {
                    for (auto& modifyEntry : m_modifyEntries) {
                        if (modifyEntry.observedProperties.find(propertyName) != modifyEntry.observedProperties.end()) {
                            checkIfActive(state, entity);
                            return;
                        }
                    }
                });
    }
}

void ModifyProperty::checkIfActive(State& state, LocatedEntity& entity)
{
    if (state.parentEntity) {
        std::set<std::pair<std::string, Modifier*>> activatedModifiers;

        for (auto& modifyEntry: m_modifyEntries) {
            bool apply;
            if (!modifyEntry.constraint) {
                apply = true;
            } else {
                EntityFilter::QueryContext queryContext{entity, state.parentEntity};
                queryContext.entity_lookup_fn = [&entity, &state](const std::string& id) {
                    // This might be applied before the entity or its parent has been added to the world.
                    if (id == entity.getId()) {
                        return Ref<LocatedEntity>(&entity);
                    } else if (id == state.parentEntity->getId()) {
                        return Ref<LocatedEntity>(state.parentEntity);
                    } else {
                        return BaseWorld::instance().getEntity(id);
                    }
                };
                queryContext.type_lookup_fn = [](const std::string& id) { return Inheritance::instance().getType(id); };
                apply = modifyEntry.constraint->match(queryContext);
            }

            if (apply) {
                for (auto& appliedModifierEntry : modifyEntry.modifiers) {
                    activatedModifiers.emplace(appliedModifierEntry.first, appliedModifierEntry.second.get());
                }
            }

//
//            if (apply) {
//                if (state.activeModifiers.find(&modifyEntry) == state.activeModifiers.end()) {
//                    for (auto& appliedModifierEntry : modifyEntry.modifiers) {
//                        state.parentEntity->addModifier(appliedModifierEntry.first, appliedModifierEntry.second.get(), &entity);
//                    }
//                    state.activeModifiers.insert(&modifyEntry);
//                    state.parentEntity->requirePropertyClassFixed<ModifiersProperty>()->addFlags(prop_flag_unsent);
//                }
//            } else {
//                if (state.activeModifiers.find(&modifyEntry) != state.activeModifiers.end()) {
//                    for (auto& appliedModifierEntry : modifyEntry.modifiers) {
//                        state.parentEntity->removeModifier(appliedModifierEntry.first, appliedModifierEntry.second.get());
//                    }
//                    state.activeModifiers.erase(&modifyEntry);
//                    state.parentEntity->requirePropertyClassFixed<ModifiersProperty>()->addFlags(prop_flag_unsent);
//                }
//            }
        }

        auto& activeModifiers = state.parentEntity->getActiveModifiers();
        auto I = activeModifiers.find(&entity);
        if (I != activeModifiers.end()) {
            //There were already modifiers active. Check the difference and add or remove.

            auto modifiersCopy = I->second;
            for (auto& appliedModifierEntry : activatedModifiers) {
                auto pair = std::make_pair(appliedModifierEntry.first, appliedModifierEntry.second);
                if (modifiersCopy.find(pair) == modifiersCopy.end()) {
                    state.parentEntity->addModifier(appliedModifierEntry.first, appliedModifierEntry.second, &entity);
                    state.parentEntity->requirePropertyClassFixed<ModifiersProperty>().addFlags(prop_flag_unsent);
                } else {
                    modifiersCopy.erase(pair);
                }
            }

            //Those that are left should be removed.
            for (auto& modifierToRemove : modifiersCopy) {
                state.parentEntity->removeModifier(modifierToRemove.first, modifierToRemove.second);
            }

        } else {
            if (!activatedModifiers.empty()) {
                //No active modifiers existed, just add all
                for (auto& appliedModifierEntry : activatedModifiers) {
                    state.parentEntity->addModifier(appliedModifierEntry.first, appliedModifierEntry.second, &entity);
                }
                state.parentEntity->requirePropertyClassFixed<ModifiersProperty>().addFlags(prop_flag_unsent);
            }
        }
    }

}

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
#include "rules/entityfilter/Providers.h"
#include "BaseWorld.h"
#include "ModifiersProperty.h"


PropertyInstanceState<ModifyProperty::State> ModifyProperty::sInstanceState;

ModifyProperty::ModifyProperty() = default;

ModifyProperty::ModifyProperty(const ModifyProperty& rhs)
        : PropertyBase(rhs)
{
    setData(rhs.m_data);
}


void ModifyProperty::apply(LocatedEntity* e)
{

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
                ModifyEntry modifyEntry;
                auto& entryMap = entry.Map();
                AtlasQuery::find<std::string>(entryMap, "constraint", [&](const std::string& constraint) {
                    modifyEntry.constraint = std::make_unique<EntityFilter::Filter>(constraint, EntityFilter::ProviderFactory());
                });
                AtlasQuery::find<Atlas::Message::ListType>(entryMap, "observed_properties", [&](const Atlas::Message::ListType& observedProperties) {
                    for (auto& propertyEntry: observedProperties) {
                        if (propertyEntry.isString()) {
                            modifyEntry.observedProperties.emplace(propertyEntry.String());
                        }
                    }
                });

                AtlasQuery::find<Atlas::Message::MapType>(entryMap, "modifier", [&](const Atlas::Message::MapType& modifierMap) {
                    for (auto& modifierEntry : modifierMap) {
                        modifyEntry.propertyName = modifierEntry.first;
                        if (modifierEntry.second.isMap()) {
                            auto& valueMap = modifierEntry.second.Map();
                            auto I = valueMap.find("default");
                            if (I != valueMap.end()) {
                                modifyEntry.modifier = std::make_unique<DefaultModifier>(I->second);
                            }
                            I = valueMap.find("append");
                            if (I != valueMap.end()) {
                                modifyEntry.modifier = std::make_unique<AppendModifier>(I->second);
                            }
                            I = valueMap.find("prepend");
                            if (I != valueMap.end()) {
                                modifyEntry.modifier = std::make_unique<PrependModifier>(I->second);
                            }
                            I = valueMap.find("subtract");
                            if (I != valueMap.end()) {
                                modifyEntry.modifier = std::make_unique<SubtractModifier>(I->second);
                            }
                            I = valueMap.find("multiply");
                            if (I != valueMap.end()) {
                                modifyEntry.modifier = std::make_unique<MultiplyModifier>(I->second);
                            }
                        }
                    }
                });

                if (modifyEntry.modifier) {
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

void ModifyProperty::remove(LocatedEntity* owner, const std::string& name)
{
    auto* state = sInstanceState.getState(owner);
    newLocation(*state, *owner, nullptr);
    state->updatedConnection.disconnect();
    sInstanceState.removeState(owner);
}

void ModifyProperty::install(LocatedEntity* owner, const std::string& name)
{
    auto state = new ModifyProperty::State();
    state->parentEntity = owner->m_location.m_parent.get();
    state->updatedConnection.disconnect();
    state->updatedConnection = owner->updated.connect([&, state, owner]() {
        if (state->parentEntity == owner->m_location.m_parent.get()) {
            newLocation(*state, *owner, owner->m_location.m_parent.get());
        }
    });
    if (owner->m_location.m_parent) {
        newLocation(*state, *owner, owner->m_location.m_parent.get());
    }

    sInstanceState.addState(owner, std::unique_ptr<ModifyProperty::State>(state));
}


void ModifyProperty::newLocation(State& state, LocatedEntity& entity, LocatedEntity* parent)
{
    if (state.parentEntity) {
        for (auto& entry: state.activeModifiers) {
            state.parentEntity->removeModifier(entry->propertyName, entry->modifier.get());
            state.parentEntity->requirePropertyClassFixed<ModifiersProperty>()->addFlags(prop_flag_unsent);
        }
        state.activeModifiers.clear();
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
    checkIfActive(state, entity);
}

void ModifyProperty::checkIfActive(State& state, LocatedEntity& entity)
{
    if (state.parentEntity) {
        for (auto& modifyEntry: m_modifyEntries) {
            bool apply;
            if (!modifyEntry.constraint) {
                apply = true;
            } else {
                EntityFilter::QueryContext queryContext{entity, state.parentEntity};
                queryContext.entity_lookup_fn = [](const std::string& id) { return BaseWorld::instance().getEntity(id); };
                queryContext.type_lookup_fn = [](const std::string& id) { return Inheritance::instance().getType(id); };
                apply = modifyEntry.constraint->match(queryContext);
            }

            if (apply) {
                if (state.activeModifiers.find(&modifyEntry) == state.activeModifiers.end()) {
                    state.parentEntity->addModifier(modifyEntry.propertyName, modifyEntry.modifier.get(), &entity);
                    state.activeModifiers.insert(&modifyEntry);
                    state.parentEntity->requirePropertyClassFixed<ModifiersProperty>()->addFlags(prop_flag_unsent);
                }
            } else {
                if (state.activeModifiers.find(&modifyEntry) != state.activeModifiers.end()) {
                    state.parentEntity->removeModifier(modifyEntry.propertyName, modifyEntry.modifier.get());
                    state.activeModifiers.erase(&modifyEntry);
                    state.parentEntity->requirePropertyClassFixed<ModifiersProperty>()->addFlags(prop_flag_unsent);
                }
            }
        }
    }

}

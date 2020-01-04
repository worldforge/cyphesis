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

#include "ModifiersProperty.h"
#include "rules/LocatedEntity.h"
#include "rules/Modifier.h"

ModifiersProperty::ModifiersProperty() :
        PropertyBase(flag_instance | persistence_ephem),
        m_entity(nullptr)
{
}

ModifiersProperty* ModifiersProperty::copy() const
{
    return new ModifiersProperty(*this);
}

void ModifiersProperty::install(LocatedEntity* entity, const std::string&)
{
    m_entity = entity;
}

void ModifiersProperty::set(const Atlas::Message::Element& val)
{
    //This is read only.
}

int ModifiersProperty::get(Atlas::Message::Element& val) const
{
    Atlas::Message::ListType list;
    if (m_entity) {
        auto& props = m_entity->getProperties();
        for (auto& entry: props) {
            if (!entry.second.modifiers.empty()) {
                for (auto& modifierEntry : entry.second.modifiers) {
                    Atlas::Message::MapType map;
                    map["$eid"] = modifierEntry.second->getId();
                    //map["property"] = entry.first;

                    Atlas::Message::MapType modifierMap;
                    switch (modifierEntry.first->getType()) {
                        case Modifier::Type::Default:
                            modifierMap["default"] = modifierEntry.first->mValue;
                            break;
                        case Modifier::Type::Prepend:
                            modifierMap["prepend"] = modifierEntry.first->mValue;
                            break;
                        case Modifier::Type::Append:
                            modifierMap["append"] = modifierEntry.first->mValue;
                            break;
                        case Modifier::Type::Subtract:
                            modifierMap["subtract"] = modifierEntry.first->mValue;
                            break;
                        case Modifier::Type::Multiply:
                            modifierMap["multiply"] = modifierEntry.first->mValue;
                            break;
                    }

                    map["modifier"] = Atlas::Message::MapType{{entry.first, std::move(modifierMap)}};
                    list.emplace_back(std::move(map));
                }
            }
        }
    }
    val = list;
}

ModifiersProperty::ModifiersProperty(const ModifiersProperty& rhs) = default;

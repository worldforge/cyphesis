// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2008 Alistair Riddoch
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

#include "MindProperty.h"

#include "server/ExternalMindsManager.h"

#include <Atlas/Objects/Anonymous.h>

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Operation::Look;

MindProperty::MindProperty(const MindProperty& rhs) = default;

void MindProperty::set(const Element& val)
{
    m_language.clear();
    m_script.clear();
    if (!val.isMap()) {
        return;
    }
    const MapType& data = val.Map();

    {
        auto I = data.find("name");
        if (I == data.end()) {
            return;
        }
        if (!I->second.isString()) {
            return;
        }
        m_script = I->second.asString();
    }

    {
        auto I = data.find("language");
        if (I == data.end()) {
            return;
        }
        if (!I->second.isString()) {
            return;
        }
        m_language = I->second.asString();
    }
}

int MindProperty::get(Atlas::Message::Element& val) const
{
    MapType map{};
    if (!m_language.empty()) {
        map.emplace("language", m_language);
    }
    if (!m_script.empty()) {
        map.emplace("name", m_script);
    }
    val = map;
    return 0;
}


MindProperty* MindProperty::copy() const
{
    return new MindProperty(*this);
}

void MindProperty::apply(LocatedEntity& ent)
{
    //Only request possession if there's a value. This allows player controlled entities to be exempted.
    if (isMindEnabled()) {
        ExternalMindsManager::instance().requestPossession(ent);
    } else {
        ExternalMindsManager::instance().removeRequest(ent);
    }
}

void MindProperty::remove(LocatedEntity& ent, const std::string& name)
{
    ExternalMindsManager::instance().removeRequest(ent);
}


bool MindProperty::isMindEnabled() const
{
    return !m_language.empty() && !m_script.empty();
}


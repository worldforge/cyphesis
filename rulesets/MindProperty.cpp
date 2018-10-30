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

#include "server/PossessionAuthenticator.h"
#include "server/ExternalMindsManager.h"

#include "rulesets/BaseMind.h"

#include "common/operations/Setup.h"

#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/Operation.h>

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Operation::Setup;
using Atlas::Objects::Operation::Look;

MindProperty::MindProperty(const MindProperty & rhs) = default;

void MindProperty::set(const Element & val)
{
    Property<Atlas::Message::MapType>::set(val);
    if (!val.isMap()) {
        m_language.clear();
        m_script.clear();
        return;
    }
    const MapType & data = val.Map();
    if (data.empty()) {
        m_language.clear();
        m_script.clear();
        return;
    }

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

MindProperty * MindProperty::copy() const
{
    return new MindProperty(*this);
}

void MindProperty::apply(LocatedEntity * ent)
{
    ExternalMindsManager::instance().requestPossession(ent);
}

bool MindProperty::isMindEnabled() const
{
    return !m_language.empty() || !m_script.empty();
}


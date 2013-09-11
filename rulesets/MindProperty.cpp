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

#include "rulesets/Character.h"
#include "rulesets/MindFactory.h"
#include "rulesets/PythonScriptFactory.h"
#include "rulesets/BaseMind.h"

#include "common/atlas_helpers.h"
#include "common/compose.hpp"
#include "common/log.h"
#include "common/Setup.h"

#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/SmartPtr.h>

#include <iostream>

using Atlas::Message::Element;
using Atlas::Message::MapType;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Operation::Setup;
using Atlas::Objects::Operation::Look;

MindProperty::MindProperty(const MindProperty & rhs)
: m_language(rhs.m_language), m_script(rhs.m_script)
{
}

MindProperty::MindProperty()
{
}

MindProperty::~MindProperty()
{
}

void MindProperty::set(const Element & val)
{
    Property<Atlas::Message::MapType>::set(val);
    if (!val.isMap()) {
        return;
    }
    const MapType & data = val.Map();
    if (data.empty()) {
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

    //Only request a possession if there's a language and script specified.
    if (m_language.empty() || m_script.empty()) {
        return;
    }

    Character * chr = dynamic_cast<Character *>(ent);

    if (chr == nullptr) {
        log(NOTICE, "Mind property applied to non-character");
        return;
    }

    if (chr->m_externalMind != nullptr) {
        log(NOTICE, "Mind property character already has an external mind");
        return;
    }

    ExternalMindsManager::instance()->requestPossession(*chr, m_language, m_script);

}

bool MindProperty::isMindEnabled() const
{
    return m_language != "" || m_script != "";
}


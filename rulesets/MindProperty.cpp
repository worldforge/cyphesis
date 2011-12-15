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

// $Id$

#include "MindProperty.h"

#include "rulesets/Character.h"
#include "rulesets/MindFactory.h"
#include "rulesets/PythonScriptFactory.h"

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

MindProperty::MindProperty()
{
}

int MindProperty::get(Element & val) const
{
    return 0;
}

void MindProperty::set(const Element & val)
{
    log(NOTICE, "Mind property getting set");
    if (!val.isMap()) {
        return;
    }
    const MapType & data = val.Map();
    std::string script_package;
    std::string script_class;
    if (GetScriptDetails(data, "Foo", "Mind",
                         script_package, script_class) != 0) {
        return;
    }

    if (m_factory == 0) {
        m_factory = new MindFactory;
    }
    log(NOTICE, "Sorted");
    if (m_factory->m_scriptFactory != 0) {
        if (m_factory->m_scriptFactory->package() != script_package) {
            delete m_factory->m_scriptFactory;
            m_factory->m_scriptFactory = 0;
        }
    }
    if (m_factory->m_scriptFactory == 0) {
        PythonScriptFactory<BaseMind> * psf =
              new PythonScriptFactory<BaseMind>(script_package, script_class);
        if (psf->setup() == 0) {
            m_factory->m_scriptFactory = psf;
        } else {
            log(ERROR, String::compose("Python class \"%1.%2\" failed to load",
                                       script_package, script_class));
            delete psf;
            return;
        }
    }
}

void MindProperty::apply(Entity * ent)
{
    if (m_factory == 0) {
        log(NOTICE, "Mind property applied without factory");
        return;
    }

    Character * chr = dynamic_cast<Character *>(ent);

    if (chr == 0) {
        log(NOTICE, "Mind property applied to non-character");
        return;
    }

    if (chr->m_mind != 0) {
        log(NOTICE, "Mind property character already has a mind");
        return;
    }

    chr->m_mind = MindBuilder::instance()->newMind(ent->getId(),
                                                   ent->getIntId(),
                                                   ent->getType());

    Setup s;
    Anonymous setup_arg;
    setup_arg->setName("mind");
    s->setTo(ent->getId());
    s->setArgs1(setup_arg);
    ent->sendWorld(s);

    Look l;
    l->setTo(ent->getId());
    ent->sendWorld(l);

}

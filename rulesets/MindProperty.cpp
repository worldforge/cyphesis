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

#include "common/log.h"
#include "common/Setup.h"

#include <Atlas/Objects/Anonymous.h>
#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/SmartPtr.h>

#include <iostream>

using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Operation::Setup;
using Atlas::Objects::Operation::Look;

MindProperty::MindProperty()
{
}

void MindProperty::apply(Entity * ent)
{
    if (m_data.empty()) {
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

    chr->m_mind = MindFactory::instance()->newMind(ent->getId(),
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

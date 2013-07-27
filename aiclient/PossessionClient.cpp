/*
 Copyright (C) 2013 Erik Ogenvik

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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "PossessionClient.h"

#include "common/Possess.h"
#include "common/log.h"
#include "common/compose.hpp"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Entity.h>

PossessionClient::PossessionClient()
{

}

PossessionClient::~PossessionClient()
{
}

void PossessionClient::idle()
{

}

void PossessionClient::enablePossession()
{

    Atlas::Objects::Operation::Set set;
    set->setTo(m_playerId);
    set->setFrom(m_playerId);

    Atlas::Objects::Entity::Anonymous args;
    args->setId(m_playerId);
    args->setAttr("possessive", 1);
    args->setObjtype("object");

    set->setArgs1(args);

    m_connection.send(set);
}

void PossessionClient::operation(const Operation & op, OpVector & res)
{
    if (op->getClassNo() == Atlas::Objects::Operation::POSSESS_NO) {
        PossessOperation(op, res);
    } else {
        log(NOTICE, String::compose("Unknown operation %1 in PossessionClient",
                                        op->getParents().front()));
    }
}

void PossessionClient::PossessOperation(const Operation & op, OpVector & res)
{
    log(INFO, "Got possession request.");
}


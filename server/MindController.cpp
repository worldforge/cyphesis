/*
 Copyright (C) 2014 Erik Ogenvik

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

#include "MindController.h"

MindController::MindController()
{
}

MindController::~MindController()
}


void MindController::setAttribute(const Root& args) {
    //If the attribute "possessive" is set on the account it will control whether this
    //account acts as an external minds connection.
    if (args->hasAttr("possessive")) {
        const Element possessiveElement = args->getAttr("possessive");
        if (possessiveElement.isInt() && possessiveElement.asInt() != 0) {
            m_connection->setPossessionEnabled(true, getId());
        } else {
            m_connection->setPossessionEnabled(false, getId());
        }
    }
}

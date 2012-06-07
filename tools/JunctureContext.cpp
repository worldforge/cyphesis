// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2012 Alistair Riddoch
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

#include "JunctureContext.h"

#include <Atlas/Objects/RootOperation.h>
#include <Atlas/Objects/SmartPtr.h>

#include <iostream>

using Atlas::Objects::Operation::RootOperation;

JunctureContext::JunctureContext(Interactive & i,
                                 const std::string & id) : IdContext(i, id)
{
}

bool JunctureContext::accept(const RootOperation& op) const
{
    std::cout << "Checking juncture context to see if it matches"
              << std::endl << std::flush;
    if (m_refNo != 0L && !op->isDefaultRefno() && op->getRefno() == m_refNo) {
        std::cout << "It does!"
                  << std::endl << std::flush;
        return true;
    }

    return false;
}

int JunctureContext::dispatch(const RootOperation & op)
{
    std::cout << "Juncture dispatch!"
              << std::endl << std::flush;
    // If we get an info op here, it can mean something succeeded, like
    // connection or login.
    return 0;
}

std::string JunctureContext::repr() const
{
    return "junc";
}

bool JunctureContext::checkContextCommand(const struct command *)
{
    return false;
}

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


#include "AvatarContext.h"
#include "common/debug.h"

#include "tools/Interactive.h"

#include <iostream>

static const bool debug_flag = false;

using Atlas::Objects::Operation::RootOperation;

AvatarContext::AvatarContext(Interactive& i, const std::string& id) :
        IdContext(i, id)
{
}

bool AvatarContext::accept(const RootOperation& op) const
{
    debug_print("Checking avatar context to see if it matches")
    return m_refNo != 0L && !op->isDefaultRefno() && op->getRefno() == m_refNo;
}

int AvatarContext::dispatch(const RootOperation& op)
{
    if (op->getClassNo() == Atlas::Objects::Operation::SIGHT_NO) {
        std::cout << "Sight(" << std::endl;
        m_client.output(op->getArgs().front());
        std::cout << ")" << std::endl << std::flush;
    }
    if (!op->isDefaultRefno() && op->getRefno() == m_refNo) {
        m_refNo = 0L;
    }
    return 0;
}

std::string AvatarContext::repr() const
{
    return "avatar";
}

bool AvatarContext::checkContextCommand(const struct command*)
{
    return false;
}

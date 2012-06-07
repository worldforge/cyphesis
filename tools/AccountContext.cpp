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

#include "AccountContext.h"

#include "tools/Interactive.h"
#include "tools/JunctureContext.h"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/SmartPtr.h>

#include <iostream>

using Atlas::Objects::Root;
using Atlas::Objects::Operation::RootOperation;

using boost::shared_ptr;

AccountContext::AccountContext(Interactive & i,
                               const std::string & id,
                               const std::string & u) : IdContext(i, id),
                                                        m_username(u)
{
}

bool AccountContext::accept(const RootOperation& op) const
{
    std::cout << "Checking account context to see if it matches"
              << std::endl << std::flush;
    if (m_refNo != 0L && !op->isDefaultRefno() && op->getRefno() == m_refNo) {
        return true;
    }
    return false;
}

int AccountContext::dispatch(const RootOperation & op)
{
    std::cout << "Dispatching with account context to see if it matches"
              << std::endl << std::flush;
    if (op->getClassNo() == Atlas::Objects::Operation::INFO_NO &&
        !op->getArgs().empty()) {
        std::cout << "Dispatching info"
                  << std::endl << std::flush;
        const Root & ent = op->getArgs().front();
        if (ent->hasAttrFlag(Atlas::Objects::ID_FLAG) &&
            ent->hasAttrFlag(Atlas::Objects::PARENTS_FLAG) &&
            ent->getParents().size() > 0) {
            const std::string & type = ent->getParents().front();
            if (type == "juncture") {
                std::cout << "created juncture"
                          << std::endl << std::flush;
                m_client.addCurrentContext(shared_ptr<ObjectContext>(
                      new JunctureContext(m_client, ent->getId())));
            } else {
                std::cout << "created avatar"
                          << std::endl << std::flush;
            }
        } else {
        }
        
    }
    assert(m_refNo != 0L);
    m_refNo = 0L;
    return 0;
}

std::string AccountContext::repr() const
{
    return m_username;
}

bool AccountContext::checkContextCommand(const struct command *)
{
    return false;
}

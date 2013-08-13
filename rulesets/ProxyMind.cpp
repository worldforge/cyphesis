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

#include "ProxyMind.h"

#include "common/custom.h"

#include <Atlas/Objects/Operation.h>

ProxyMind::ProxyMind(const std::string & id, long intId) :
        BaseMind(id, intId)
{

}

ProxyMind::~ProxyMind()
{
}

void ProxyMind::operation(const Operation & op, OpVector & res)
{
    auto op_no = op->getClassNo();
    if (op_no == Atlas::Objects::Operation::THOUGHT_NO) {
        m_thoughts.push_back(op);
    } else if (op_no == Atlas::Objects::Operation::GET_NO) {
        if (!op->getArgs().empty()) {
            if (op->getArgs().front()->getClassNo() == Atlas::Objects::Operation::THOUGHT_NO) {
                res.insert(res.end(), m_thoughts.begin(), m_thoughts.end());
            }
        }
    }

    BaseMind::operation(op, res);
}

const std::vector<Atlas::Objects::Operation::RootOperation> ProxyMind::getThoughts() const
{
    return m_thoughts;
}

void ProxyMind::clearThoughts()
{
    m_thoughts.clear();
}




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
#include "common/Think.h"

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
    if (op_no == Atlas::Objects::Operation::THINK_NO) {
        if (!op->getArgs().empty()) {
            m_thoughts.insert(m_thoughts.end(), op->getArgs().begin(),
                    op->getArgs().end());
        }
        return;
    } else if (op_no == Atlas::Objects::Operation::COMMUNE_NO) {
        if (op->getArgs().empty()) {
            Atlas::Objects::Operation::Think think;
            if (op->getSerialno()) {
                think->setRefno(op->getSerialno());
            }
            think->setArgs(m_thoughts);
            res.push_back(think);
        }
        return;
    }

    BaseMind::operation(op, res);
}

const std::vector<Atlas::Objects::Root> ProxyMind::getThoughts() const
{
    return m_thoughts;
}

void ProxyMind::clearThoughts()
{
    m_thoughts.clear();
}


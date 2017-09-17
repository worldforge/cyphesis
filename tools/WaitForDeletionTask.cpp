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
#endif

#include "WaitForDeletionTask.h"

#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Entity.h>

WaitForDeletionTask::WaitForDeletionTask(const std::string& entityId)
: m_entityId(entityId)
{

}

void WaitForDeletionTask::setup(const std::string & arg, OpVector & res)
{
    //No need for setup here.
}

void WaitForDeletionTask::operation(const Operation & op, OpVector & res)
{
    if (op->getClassNo() == Atlas::Objects::Operation::SIGHT_NO) {
        if (!op->getArgs().empty()) {
            auto& innerOp = op->getArgs().front();
            if (innerOp->getClassNo() == Atlas::Objects::Operation::DELETE_NO) {
                auto deleteOp = Atlas::Objects::smart_dynamic_cast<Operation>(innerOp);
                if (!deleteOp->getArgs().empty()) {
                    auto args = deleteOp->getArgs().front();
                    if (args->getId() == m_entityId) {
                        m_complete = true;
                    }
                }
            }
        }
    }
}


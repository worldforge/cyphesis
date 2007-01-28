// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000,2001 Alistair Riddoch
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

// $Id: ExternalMind.cpp,v 1.20 2007-01-28 23:51:08 alriddoch Exp $

#include "ExternalMind.h"

#include "Connection_methods.h"

#include <Atlas/Objects/SmartPtr.h>
#include <Atlas/Objects/Operation.h>
#include <Atlas/Objects/Anonymous.h>

using Atlas::Message::Element;
using Atlas::Objects::Root;
using Atlas::Objects::smart_dynamic_cast;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::Entity::Anonymous;
using Atlas::Objects::Operation::Sight;
using Atlas::Objects::Operation::Imaginary;

ExternalMind::ExternalMind(Connection & connection,
                           const std::string & id, long intId) :
                           BaseEntity(id, intId), m_connection(connection)
{
}

ExternalMind::~ExternalMind()
{
    // This object does not own the associated entity, so is not
    // responsible for removing it from the connectio.
    // m_connection.removeObject(getIntId());
}

void ExternalMind::operation(const Operation & op, OpVector &)
{
    m_connection.send(op);

    // Here we see if there is anything we should be sending the user
    // extra info about. The initial demo implementation checks for
    // Set ops which make the characters status less than 0.1, and sends
    // emotes that the character is hungry.
    const std::vector<Root> & args = op->getArgs();
    if (op->getClassNo() == OP_SIGHT && !args.empty()) {
        Operation sub_op = smart_dynamic_cast<Operation>(args.front());
        if (sub_op.isValid()) {
            const std::vector<Root> & sub_args = sub_op->getArgs();
            if (sub_op->getClassNo() == OP_SET && !sub_args.empty()) {
                const Root & arg = sub_args.front();
                Element status_value;
                if (arg->getId() == getId() and
                    arg->copyAttr("status", status_value) == 0 and
                    status_value.isFloat() and status_value.Float() < 0.1) {

                    Anonymous imaginary_arg;
                    imaginary_arg->setId(getId());
                    if (status_value.Float() < 0.01) {
                        imaginary_arg->setAttr("description", "is starving.");
                    } else {
                        imaginary_arg->setAttr("description", "is hungry.");
                    }

                    Imaginary imaginary;
                    imaginary->setTo(getId());
                    imaginary->setFrom(getId());
                    imaginary->setArgs1(imaginary_arg);

                    Sight sight;
                    sight->setTo(getId());
                    sight->setFrom(getId());
                    sight->setArgs1(imaginary);

                    m_connection.send(sight);
                }
            }
        }
    }
}

// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2010 Alistair Riddoch
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

#include "Juncture.h"

#include "common/Connect.h"

#include "common/log.h"

#include <Atlas/Objects/Operation.h>

Juncture::Juncture(const std::string & id, long iid) : Router(id, iid),
                                                       m_socket(0),
                                                       m_peer(0)
{
}

Juncture::~Juncture()
{
}

void Juncture::operation(const Operation & op, OpVector & res)
{
    const OpNo op_no = op->getClassNo();
    switch (op_no) {
        case Atlas::Objects::Operation::LOGIN_NO:
            LoginOperation(op, res);
            break;
        case OP_INVALID:
            break;
        default:
            OtherOperation(op, res);
            break;
    }
}

void Juncture::LoginOperation(const Operation & op, OpVector & res)
{
    log(INFO, "Juncture got login");
}

void Juncture::OtherOperation(const Operation & op, OpVector & res)
{
    const int op_type = op->getClassNo();
    if (op_type == Atlas::Objects::Operation::CONNECT_NO) {
        return customConnectOperation(op, res);
    }
}

void Juncture::customConnectOperation(const Operation & op, OpVector & res)
{
    log(INFO, "Juncture got connect");
}

// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000-2004 Alistair Riddoch
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


#ifndef COMMON_OP_SWITCH_H
#define COMMON_OP_SWITCH_H

#define SUB_OP_SWITCH(_op, _sub_op_no, _result, _prefix) \
    switch (_sub_op_no) { \
        case Atlas::Objects::Operation::CREATE_NO: \
            _prefix ## CreateOperation(_op, _result); \
            break; \
        case Atlas::Objects::Operation::MOVE_NO: \
            _prefix ## MoveOperation(_op, _result); \
            break; \
        case Atlas::Objects::Operation::SET_NO: \
            _prefix ## SetOperation(_op, _result); \
            break; \
        case OP_INVALID: \
            break; \
        default: \
            break; \
    }

#endif // COMMON_OP_SWITCH_H

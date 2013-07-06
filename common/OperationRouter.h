// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2007 Alistair Riddoch
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


#ifndef COMMON_OPERATION_ROUTER_H
#define COMMON_OPERATION_ROUTER_H

#include <Atlas/Objects/ObjectsFwd.h>

#include <vector>

#define OP_INVALID (-1)

typedef Atlas::Objects::Operation::RootOperation Operation;

typedef std::vector<Operation> OpVector;

typedef enum {
    OPERATION_BLOCKED, // Handler has determined that op should stop here
    OPERATION_HANDLED, // Handler has done something, but op should continue
    OPERATION_IGNORED, // Handler has done nothing
} HandlerResult;

#endif // COMMON_OPERATION_ROUTER_H

// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2001 Alistair Riddoch
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

// $Id: Script.cpp,v 1.14 2006-12-24 14:42:06 alriddoch Exp $

#include "Script.h"

Script::Script()
{
}

/// \brief Script destructor
Script::~Script()
{
}

// FIXME Return value of operation should be an int with different
// return values for failure vs. not accepted. 0 for success obviously

/// \brief Pass an operation to the script for processing
///
/// @param opname The string representing the type of the operation
/// @param op The operation to be passed
/// @param res The result of the operation is returned here
/// @param sub_op The sub operation associated with some operations
/// @return true if operation was accepted, false if it was not handled
/// or an error occured.
bool Script::operation(const std::string & opname,
                       const Atlas::Objects::Operation::RootOperation & op,
                       OpVector & res,
                       const Atlas::Objects::Operation::RootOperation * sub_op)
{
   return false;
}

/// \brief Call a named function on the script, passing in the entity
///
/// This function is used when object have registered function names to be
/// valled any time a certain event occurs. Currently the machanism is only
/// used and supported in the mind code.
/// @param function name of the function to call in the script
/// @param entity which has triggered this hook
void Script::hook(const std::string & function, Entity * entity)
{
}

/// \brief Default object available for entities that don't have a script.
Script noScript;

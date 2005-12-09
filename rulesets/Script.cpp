// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 Alistair Riddoch

#include "Script.h"

Script::Script()
{
}

Script::~Script()
{
}

// FIXME Return value of operation should be an int with different
// return values for failure vs. not accepted. 0 for success obviously

/// \brief Pass an operation to the script for processing
///
/// @param type The string representing the type of the operation
/// @param op The operation to be passed
/// @param res The result of the operation is returned here
/// @return true if operation was accepted, false if it was not handled
/// or an error occured.
bool Script::operation(const std::string & opname,
                       const Atlas::Objects::Operation::RootOperation & op,
                       OpVector & res,
                       const Atlas::Objects::Operation::RootOperation * sub_op)
{
   return false;
}

void Script::hook(const std::string &, Entity *)
{
}

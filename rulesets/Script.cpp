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

bool Script::Operation(const std::string & opname,
                       const Atlas::Objects::Operation::RootOperation & op,
                       OpVector & res,
                       Atlas::Objects::Operation::RootOperation * sub_op)
{
   return false;
}

void Script::hook(const std::string &, Entity *)
{
}

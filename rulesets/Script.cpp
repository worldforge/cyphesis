// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 Alistair Riddoch

#include "Script.h"

using Atlas::Objects::Operation::RootOperation;

class Entity;

Script::Script()
{
}

Script::~Script()
{
}

bool Script::Operation(const std::string & opname, const RootOperation & op,
                        oplist & res, RootOperation * sub_op)
{
   return false;
}

void Script::hook(const std::string &, Entity *)
{
}

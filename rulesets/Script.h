// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 Alistair Riddoch

#ifndef SCRIPT_H
#define SCRIPT_H

#include <string>

#include <Atlas/Objects/Operation/RootOperation.h>

#include <common/types.h>

class Entity;

class Script {
  public:
    Script();
    virtual ~Script();
    virtual bool Operation(const string &,
                      const Atlas::Objects::Operation::RootOperation&, oplist&,
                      Atlas::Objects::Operation::RootOperation * sub_op=NULL);
    virtual void hook(const string &, Entity *);
};

#endif // SCRIPT_H

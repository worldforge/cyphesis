// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef CREATOR_H
#define CREATOR_H

#include "Character.h"

class Creator : public Character {
  public:
    Creator();
    virtual oplist sendMind(const RootOperation & msg);
    virtual oplist operation(const RootOperation & op);
    virtual oplist externalOperation(const RootOperation & op);
};

#endif // CREATOR_H

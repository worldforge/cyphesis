// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef RULESETS_CREATOR_H
#define RULESETS_CREATOR_H

#include "Character.h"

class Creator : public Character {
  public:
    Creator();
    virtual oplist sendMind(const RootOperation & msg);
    virtual oplist operation(const RootOperation & op);
    virtual oplist externalOperation(const RootOperation & op);
};

#endif // RULESETS_CREATOR_H

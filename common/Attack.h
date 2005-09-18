// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifndef COMMON_ATTACK_H
#define COMMON_ATTACK_H

#include <Atlas/Objects/Generic.h>

namespace Atlas { namespace Objects { namespace Operation {

extern int ATTACK_NO;

class Attack : public Generic
{
  public:
    Attack() {
        (*this)->setType("attack", ATTACK_NO);
    }
};

} } }

#endif // COMMON_ATTACK_H

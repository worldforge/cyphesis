// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifndef COMMON_ADD_H
#define COMMON_ADD_H

#include <Atlas/Objects/Generic.h>

namespace Atlas { namespace Objects { namespace Operation {

extern int ADD_NO;

class Add : public Generic
{
  public:
    Add() {
        (*this)->setType("add", ADD_NO);
    }
};

} } }

#endif // COMMON_ADD_H

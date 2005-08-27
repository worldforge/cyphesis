// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#ifndef COMMON_INHERITANCE_IMPL_H
#define COMMON_INHERITANCE_IMPL_H

#include "inheritance.h"

#include <Atlas/Objects/SmartPtr.h>
#include <Atlas/Objects/Operation.h>

template <class OpClass>
Operation OpFactory<OpClass>::newOperation()
{
    return OpClass();
}

template <class OpClass>
void OpFactory<OpClass>::newOperation(Operation & op)
{
    op = OpClass();
}

#endif // COMMON_INHERITANCE_IMPL_H

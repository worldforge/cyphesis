// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#ifndef COMMON_INHERITANCE_IMPL_H
#define COMMON_INHERITANCE_IMPL_H

#include "inheritance.h"

template <class OpClass>
RootOperation * OpFactory<OpClass>::newOperation()
{
    return new OpClass();
}

#endif // COMMON_INHERITANCE_IMPL_H

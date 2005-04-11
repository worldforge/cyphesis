// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#error This file has been removed from the build

#ifndef COMMON_REFNO_H
#define COMMON_REFNO_H

#include <Atlas/Objects/Operation/RootOperation.h>

typedef std::vector<Atlas::Objects::Operation::RootOperation *> OpVector;

static inline void setRefno(const OpVector& ret, const Operation & ref_op)
{
    OpVector::const_iterator Iend = ret.end();
    for(OpVector::const_iterator I = ret.begin(); I != Iend; ++I) {
        (*I)->setRefno(ref_op.getSerialno());
    }
}

#endif // COMMON_REFNO_H

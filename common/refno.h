// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#ifndef COMMON_REFNO_H
#define COMMON_REFNO_H

#include "types.h"

#include <Atlas/Objects/Operation/RootOperation.h>

static inline void setRefnoOp(RootOperation * op, const RootOperation & ref_op)
{
    op->setRefno(ref_op.getSerialno());
}

static inline void setRefno(const OpVector& ret, const RootOperation & ref_op)
{
    for(OpVector::const_iterator I = ret.begin(); I != ret.end(); I++) {
        setRefnoOp(*I, ref_op);
    }
}

#endif // COMMON_REFNO_H

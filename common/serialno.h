// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef COMMON_SERIALNO_H
#define COMMON_SERIALNO_H

extern int opSerialCount;

inline int opSerialNo()
{
    return ++opSerialCount;
}

#endif // COMMON_SERIALNO_H

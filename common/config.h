// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#ifndef COMMON_CONFIG_H
#define COMMON_CONFIG_H

#include "../config.h"

#if defined(HAVE_LIBDB_CXX) || defined(HAVE_LIBDB3_CXX)

#define CYPHESIS_USE_DB3

#endif // HAVE_LIBDB_CXX

#endif // COMMON_CONFIG_H

// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2004 Alistair Riddoch

#ifndef COMMON_NLS_H
#define COMMON_NLS_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#if ENABLE_NLS
# include <libintl.h>
# define _(Text) gettext (Text)
#else
# define _(Text) Text
#endif

#endif // COMMON_NLS_H

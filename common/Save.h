// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2004 Alistair Riddoch

#ifndef COMMON_SAVE_H
#define COMMON_SAVE_H

#error This file has been removed from the build

#include <Atlas/Objects/Operation/Get.h>

namespace Atlas { namespace Objects { namespace Operation {

class Save : public Get {
  protected:
    Save(const char *, const char *);
  public:
    Save();
    virtual ~Save();
    static Save Class();
};

} } }

#endif // COMMON_SAVE_H

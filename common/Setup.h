// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef COMMON_SETUP_H
#define COMMON_SETUP_H

namespace Atlas { namespace Objects { namespace Operation {

class Setup : public RootOperation {
  public:
    Setup();
    virtual ~Setup();
    static Setup Instantiate();
};

} } }

#endif // COMMON_SETUP_H

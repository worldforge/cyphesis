// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef COMMON_NOURISH_H
#define COMMON_NOURISH_H

namespace Atlas { namespace Objects { namespace Operation {

class Nourish : public RootOperation {
  public:
    Nourish();
    virtual ~Nourish();
    static Nourish Instantiate();
};

} } }

#endif // COMMON_NOURISH_H

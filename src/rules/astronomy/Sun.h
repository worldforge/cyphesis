// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch


#ifndef ASTRONOMY_SUN_H
#define ASTRONOMY_SUN_H

#include "Body.h"

// 

class Sun : public Body {
  private:
  public:
    Sun(double p, double r, double e = 0, double a = 0) : Body(p,r,e,a) { }
};

#endif // ASTRONOMY_SUN_H

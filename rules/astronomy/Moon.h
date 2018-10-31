// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch


#ifndef ASTRONOMY_MOON_H
#define ASTRONOMY_MOON_H

#include "Body.h"

// 

class Moon : public Body {
  private:
  public:
    Moon(double p, double r, double e = 0, double a = 0) : Body(p,r,e,a) { }
};

#endif // ASTRONOMY_MOON_H

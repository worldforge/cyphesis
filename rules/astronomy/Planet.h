// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch


#ifndef ASTRONOMY_PLANET_H
#define ASTRONOMY_PLANET_H

#include "Body.h"

// 

class Planet : public Body {
  private:
  public:
    Planet(double p, double r, double e = 0, double a = 0) : Body(p,r,e,a) { }
};

#endif // ASTRONOMY_PLANET_H

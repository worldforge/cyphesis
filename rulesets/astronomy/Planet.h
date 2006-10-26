// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

// $Id: Planet.h,v 1.3 2006-10-26 00:48:12 alriddoch Exp $

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

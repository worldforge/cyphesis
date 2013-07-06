// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch


#ifndef ASTRONOMY_WORLD_H
#define ASTRONOMY_WORLD_H

#include "Planet.h"

// This is used to store astronomical information about the world

class World : public Planet {
  private:
    double m_rotation_eccentricity;
  public:
    World(double p, double r, double e=0, double a=0) : Planet(p, r, e, a) { }
};

#endif // ASTRONOMY_WORLD_H

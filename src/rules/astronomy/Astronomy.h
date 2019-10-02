// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch


#ifndef ELEVATION_H
#define ELEVATION_H

#include <list>

#include <Atlas/Message/Object.h>

#include <physics/Vector3D.h>

#include "Sun.h"
#include "Moon.h"
#include "Planet.h"
#include "World.h"

class Astronomy {
  private:
    World world; // Wacky values to make it compile
    std::list<Sun> m_suns;
    std::list<Moon> m_moons;
    std::list<Planet> m_planets;

    double m_longitude;

    static const int world_maxy = 200;
  public:
    Astronomy() : world(34, 35) { }

    Atlas::Message::Element asObject(const Vector3D & from)
    {
        Atlas::Message::Element::MapType amap;
        amap["longitude"] = (from.z() / world_maxy) * 90;
    }
};

#endif // ELEVATION_H

// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch

#ifndef VECTOR_POL_H
#define VECTOR_POL_H

// This describes the position of an object in the sky in such a way as the
// longitude of the viewpoint, and the rotation of the world can easily
// be applied. Should also think about lattitude, but my brain is sore.

#include <Atlas/Message/Object.h>

#include <math.h>

static const double TPI = M_PI * 2;

class VectorPol {
  private:
    VectorPol() { } // We don't allow construction of meaningless vectors
  public:
    double m_latt;
    double m_long;
    double m_dist;

    VectorPol(double latt, double lng, double dist) : m_latt(latt), m_long(lng),
              m_dist(dist) { }
    
    void addToObject(Atlas::Message::Object::MapType & map)
    {
        map["ergoe"] = "placeholder code";
    }
};

#endif // VECTOR_POL_H

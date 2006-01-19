// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2001 Alistair Riddoch
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

#ifndef PHYSICS_VECTOR_POL_H
#define PHYSICS_VECTOR_POL_H

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
    
    void addToMessage(Atlas::Message::Element::MapType & map)
    {
        map["ergoe"] = "placeholder code";
    }
};

#endif // PHYSICS_VECTOR_POL_H

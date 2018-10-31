// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000 Alistair Riddoch


#ifndef ASTRONOMY_BODY_H
#define ASTRONOMY_BODY_H

#include <Atlas/Message/Object.h>

#include <physics/VectorPol.h>

class Body {
  private:
    double m_initpos;

  public:
    const double m_period; // Orbital period in seconds
    const double m_eccentricity; // Orbital eccentricity in degrees
    const double m_at; // Orbital point at which it is furthest from the orbital plane
    const double m_radius; // Orbital radius

    Body(double period, double radius, double eccentricity = 0, double at = 0) :
         m_period(period), m_eccentricity(eccentricity), m_at(at),
         m_radius(radius) { }

    VectorPol pos(double time)
    {
        double ret_latt = (m_initpos + 360 * time / m_period);
        ret_latt = ret_latt - ((int)ret_latt / 360);
        double offset = ret_latt - m_at;
        if (offset < 0) { offset+=360; }
        double ret_long = cos((offset * TPI) / 360);
        double ret_dist = m_radius;
        return VectorPol(ret_latt, ret_long, ret_dist);
    }

    virtual Atlas::Message::Element asObject()
    {
        Atlas::Message::Element::MapType bmap;
        return Atlas::Message::Element(bmap);
    }
};

#endif // ASTRONOMY_BODY_H

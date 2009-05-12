// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2009 Alistair Riddoch
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

// $Id$

#ifndef RULESETS_DOMAIN_H
#define RULESETS_DOMAIN_H

#include "physics/Vector3D.h"

#include <string>

class LocatedEntity;

/// \brief Base class for movement domains
///
/// The movement domain implements movement in the game world, including
/// visibility calculations, collision detection and physics.
/// Motion objects interact with the movement domain.
class Domain {
  private:
    /// Count of references held by other objects to this domain
    int m_refCount;
  protected:
    static Domain * m_instance;
  public:
    Domain();

    virtual ~Domain();

    static Domain * instance() {
        return m_instance;
    }

    /// \brief Increment the reference count on this domain
    void incRef() {
        ++m_refCount;
    }

    /// \brief Decrement the reference count on this domain
    void decRef() {
        if (--m_refCount <= 0) {
            delete this;
        }
    }

    virtual float constrainHeight(LocatedEntity *, const Point3D &,
                                  const std::string &);
};

#endif // RULESETS_DOMAIN_H

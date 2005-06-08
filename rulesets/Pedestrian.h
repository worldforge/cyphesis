// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000,2001 Alistair Riddoch

#ifndef RULESETS_PEDESTRIAN_H
#define RULESETS_PEDESTRIAN_H

#include "Movement.h"

class Entity;

/// \brief Class for handling movement of a Pedestrian Character
class Pedestrian : public Movement {
    // friend class Character;
  public:
    explicit Pedestrian(Entity & body);
    virtual ~Pedestrian();

    double getTickAddition(const Point3D & coordinates,
                           const Vector3D & velocity) const;
    int getUpdatedLocation(Location &);
    Operation * generateMove(const Location &);
};

#endif // RULESETS_PEDESTRIAN_H

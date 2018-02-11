// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000,2001 Alistair Riddoch
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


#ifndef RULESETS_PEDESTRIAN_H
#define RULESETS_PEDESTRIAN_H

#include "Movement.h"

/// \brief Class for handling movement of a Pedestrian Character
///
/// FIXME: this is all wrong. The Pedestrian/Movement classes should never issue movement updates with altered Location data, such as "pos".
/// In the event that we actually want to keep this (which I'm not sure we want) it should _only_ be able to alter "velocity", and nothing more.
/// However, we need to start implementing the concept of "force" rather than having random classes alter the velocity.
/// And code for making sure that movement is stopped when location is reached should perhaps go into the mind/client code instead.
class Pedestrian : public Movement {
    // friend class Character;
  public:
    explicit Pedestrian(LocatedEntity & body);
    ~Pedestrian() override = default;

    double getTickAddition(const Point3D & coordinates,
                           const Vector3D & velocity) const override;
    int getUpdatedLocation(Location &) override;
    Atlas::Objects::Operation::RootOperation generateMove(const Location &) override;
};

#endif // RULESETS_PEDESTRIAN_H

// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2005 Alistair Riddoch
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


#ifndef RULESETS_MOTION_H
#define RULESETS_MOTION_H

#include "physics/Vector3D.h"

#include <Atlas/Objects/ObjectsFwd.h>

#include <wfmath/vector.h>

#include <string>

class LocatedEntity;

/// \brief Base class for handling Entity movement
///
/// This class handles any kind of entity movement, handling the necessary
/// physics, collision detection etc. For example, when modelling character
/// movement, in addition to conventional gravity and collision detection
/// it is also necessary to model the fact that the character tracks the
/// surface of the terrain. This means that clients must adjust their
/// predicted position of characters to track the terrain, and must be
/// given the right cues to indicate that this is the right thing to do.
/// Similarly a boyant object needs to track the surface of the water,
/// including any procedural waves on the water.
class Motion {
  private:
    Motion(const Motion &) = delete;
    Motion & operator=(const Motion &) = delete;
  protected:
    LocatedEntity & m_entity;
    std::string m_mode;

    /// Refno of next expected update op
    long m_serialno;

    /// Collision predicted flag
    bool m_collision;
    /// Entity with which collision will occur
    LocatedEntity * m_collEntity;
    /// Normal to the collision surface
    Vector3D m_collNormal;

  public:
    explicit Motion(LocatedEntity & body);
    virtual ~Motion();

    float m_collisionTime;

    const std::string & mode() const {
        return m_mode;
    }

    long & serialno() {
        return m_serialno;
    }

    const bool collision() const {
        return m_collision;
    }

    void clearCollision() {
        m_collision = false;
    }
    
    /// \brief Set the mode the motion is currently in
    ///
    /// Configures motion handler to deal with a certain kind of movement.
    /// Typical examples are things like "walk", "run", "stand", "project",
    /// "float".
    virtual void setMode(const std::string & mode);

    /// \brief Constrain current location data.
    /// 
    /// Correct the current position and velocity to take account
    /// of current constraints. For example, if a character is walking, their
    /// their height will be adjusted to match that of the surface they are
    /// walking. This is not the same as falling from their true height onto
    /// a surface - that is a different movement type. This adjustment
    /// is a normal part of the process of tracking movement.
    virtual void adjustPosition();

    /// \brief Generate an update operation.
    ///
    /// Generate an Update operation scheduled to occur at an apropriate
    /// time for this movement. This is typically when an entity gets
    /// a move operation so it know when to schedule the next movement update.
    virtual Atlas::Objects::Operation::RootOperation * genUpdateOperation();
    
    /// \brief Generate a Move operation.
    ///
    /// Generate a Move operation scheduled to occur immediatly. This is
    /// generally called when an entity gets a Tick operation so it updates
    /// its location data, and broadcasts that info
    virtual Atlas::Objects::Operation::RootOperation * genMoveOperation();

    // Collision bullshit?
    float checkCollisions();

    // More Collision bullshit
    bool resolveCollision();

    friend class Motiontest;
};

#endif // RULESETS_MOTION_H

// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifndef RULESETS_MOTION_H
#define RULESETS_MOTION_H

#include "common/types.h"

class Entity;

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
  protected:
    Entity & m_entity;

  public:
    explicit Motion(Entity & body);
    virtual ~Motion();

    /// \brief Constrain current location data.
    /// 
    /// Correct the current position and velocity to take account
    /// of current constraints. For example, if a character is walking, their
    /// their height will be adjusted to match that of the surface they are
    /// walking. This is not the same as falling from their true height onto
    /// a surface - that is a different movement type. This adjustment
    /// is a normal part of the proces of tracking movement.
    virtual void adjustPostion();

    /// \brief Generate an update operation.
    ///
    /// Generate an Update operation scheduled to occur at an apropriate
    /// time for this movement. This is typically when an entity gets
    /// a move operation so it know when to schedule the next movement update.
    virtual Operation * genUpdateOperation();
    
    /// \brief Generate a Move operation.
    ///
    /// Generate a Move operation scheduled to occur immediatly. This is
    /// generally called when an entity gets a Tick operation so it updates
    /// its location data, and broadcasts that info
    virtual Operation * genMoveOperation();

    // Collision bullshit?
};

#endif // RULESETS_MOTION_H

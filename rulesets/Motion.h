// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2005 Alistair Riddoch

#ifndef RULESETS_MOTION_H
#define RULESETS_MOTION_H

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

    virtual what();
};

#endif // RULESETS_MOTION_H

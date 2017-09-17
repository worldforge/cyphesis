// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000-2004 Alistair Riddoch
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


#ifndef COMMON_CONST_H
#define COMMON_CONST_H

namespace consts {

  /// \defgroup Constants Global Configuration Constants
  ///
  /// These constants defined in common/config.h control the configuration
  /// of some aspects of the server which cannot be changed at runtime.
  /// Most commonly they are made constant for performance reasons, though
  /// sometimes it is because a change in the value at runtime could give
  /// confusing or undefined results.

  // @{

  /// \brief Should python code emit thinking ops
  static const int debug_thinking = 0;
  /// \brief Debug level for python code
  static const int debug_level = 0;

  /// \brief Length limit of string length for class ids and attributes
  static const unsigned int id_len = 32;

  /// \brief Scale factor for time. The may become non-constant
  static const float time_multiplier = 1.0;
  /// \brief In Real time how many seconds between ticks
  static const float basic_tick = time_multiplier * 3.0f;
  /// \brief Highest possible person velocity
  static const float base_velocity_coefficient = 5.0;
  static const float base_velocity = base_velocity_coefficient/time_multiplier;
  /// \brief Square versions of above, to avoid square roots in calculations
  static const float square_basic_tick = basic_tick * basic_tick;
  static const float square_base_velocity = base_velocity * base_velocity;

  /// \brief sin() of minimum angle subtended by visible object
  static const float sight_factor = 0.06f;
  static const float square_sight_factor = (sight_factor * sight_factor);
  /// \brief What is the minimum size of an object when calculating visibility
  static const float minSqrBoxSize = 0.25f;
  /// \brief What is the minimum size of an object when calculating visibility
  static const float minBoxSize = 0.5f;

  /// \brief Id of root world entity
  extern const char * rootWorldId;
  /// \brief Integer id of root world entity
  extern const long rootWorldIntId;
  /// \brief Version of the software we are running
  extern const char * version;
  /// \brief Identifier of this build, taken from "git rev-parse".
  extern const char * buildId;

  // @}
}

#endif // COMMON_CONST_H

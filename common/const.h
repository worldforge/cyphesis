// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2004 Alistair Riddoch

#ifndef COMMON_CONST_H
#define COMMON_CONST_H

namespace consts {

  /// \brief Should python code emit thinking ops
  static const int debug_thinking = 0;
  /// \brief Debug level for python code
  static const int debug_level = 0;

  /// \brief Scale factor for time. The may become non-constant
  static const float time_multiplier = 1.0;
  /// \brief In Real time how many seconds between ticks;
  static const float basic_tick = time_multiplier * 3.0;
  /// \brief Highest possible person velocity
  static const float base_velocity_coefficient = 5.0;
  static const float base_velocity = base_velocity_coefficient/time_multiplier;
  /// \brief Square versions of above, to avoid square roots in calculations
  static const float square_basic_tick = basic_tick * basic_tick;
  static const float square_base_velocity = base_velocity * base_velocity;

  /// \brief Are ranges for broadcast enabled.
  static const bool enable_ranges = true;
  /// \brief Range of broadcast sight ops
  static const float sight_range = 26.0;
  static const float square_sight_range = (sight_range * sight_range);
  /// \brief sin() of minimum angle subtended by visible object
  static const float sight_factor = 0.06;
  static const float square_sight_factor = (sight_factor * sight_factor);
  /// \brief Range of broadcast sound ops
  static const float hearing_range = 13.0;
  /// \brief Can things be omnipresent
  static const bool enable_omnipresence = false;
  /// \brief What is the minimum size of an object when calculating visibility
  static const float minSqrBoxSize = 0.25f;
  /// \brief What is the minimum size of an object when calculating visibility
  static const float minBoxSize = 0.5f;
  /// \brief Should we use a database
  static const bool enable_database = true;
  /// \brief Should world state be persistent
  static const bool enable_persistence = false;

  /// \brief Admin password used by admin account
  extern const char * defaultAdminPasswordHash;
  /// \brief Id of root world entity
  extern const char * rootWorldId;
  /// \brief Integer id of root world entity
  extern const long rootWorldIntId;
  /// \brief Version of the software we are running
  extern const char * version;
  /// \brief Time this server was built
  extern const char * buildTime;
  /// \brief Date this server was built
  extern const char * buildDate;

}

#endif // COMMON_CONST_H
